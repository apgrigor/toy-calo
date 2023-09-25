// 60% of the data will be used for training, 40% for validation
#define TRAINING_RATIO 0.6

#include "bdts.h"

#include "../utils/files.h"
#include "ROOT/RDataFrame.hxx"
#include "TFile.h"
#include "cuts.h"
// #include "TMVA/BinaryTree.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Factory.h"
#include "TMVA/RReader.hxx"

// linear mapping from [min, min + dif] to [0, 1]
inline float scale_to_01(float value, float min, float dif) {
    return (value - min) / dif;
}

inline TTree *fill_tree(
    std::string tree_name, std::string backing_file_name,
    const float min_energy, const float min_long_mean, const float min_lat_mean,
    const float energy_dif, const float long_mean_dif, const float lat_mean_dif,
    const size_t begin, const size_t end, const std::vector<float> energy_vec,
    const std::vector<float> long_mean_vec,
    const std::vector<float> lat_mean_vec
) {
    // TTrees unfortunately must store their data in some file otherwise you
    // get segfaults. Closing this TFile will also cause segfaults of course
    TFile *backing_file = files::recreate(backing_file_name);

    Float_t energy;
    Float_t long_mean;
    Float_t lat_mean;

    auto tree = new TTree(tree_name.c_str(), "");
    tree->Branch("energy", &energy, "energy/F");
    tree->Branch("long_mean", &long_mean, "long_mean/F");
    tree->Branch("lat_mean", &lat_mean, "lat_mean/F");

    for (size_t i = begin; i < end; i++) {
        energy = scale_to_01(energy_vec[i], min_energy, energy_dif);
        long_mean = scale_to_01(long_mean_vec[i], min_long_mean, long_mean_dif);
        lat_mean = scale_to_01(lat_mean_vec[i], min_lat_mean, lat_mean_dif);

        tree->Fill();
    }

    backing_file->Write();

    return tree;
}

class ModelData {
  public:
    size_t m_training_size;
    size_t m_validation_size;
    TTree *m_training_tree;
    TTree *m_validation_tree;

    ModelData(ROOT::RDataFrame df, UInt_t particle_type, std::string tree_name);
};

// only gets rows matching pid == particle_type, then splits it into 2
// TTrees that will be used for training and validation, according to
// TRAINING_RATIO. All data points are scaled such that they go from 0 to 1!
// I hardcoded which columns to copy!
ModelData::ModelData(
    ROOT::RDataFrame df, UInt_t particle_type, std::string tree_name
) {
    // min and max values for BDT input normalization. Note that we take them
    // for the ENTIRE dataset, not only the part we filter afterwards
    const float min_energy = df.Min("energy").GetValue();
    const float min_long_mean = df.Min("long_mean").GetValue();
    const float min_lat_mean = df.Min("lat_mean").GetValue();

    const float max_energy = df.Max("energy").GetValue();
    const float max_long_mean = df.Max("long_mean").GetValue();
    const float max_lat_mean = df.Max("lat_mean").GetValue();

    // I pre-calculate these because it's faster that way
    // could be even faster but I don't care enough
    const float energy_dif = max_energy - min_energy;
    const float long_mean_dif = max_long_mean - min_long_mean;
    const float lat_mean_dif = max_lat_mean - min_lat_mean;

    auto filtered_df = df.Filter(
        [&particle_type](UInt_t pid) { return pid == particle_type; }, {"pid"}
    );

    const size_t filtered_size = filtered_df.Count().GetValue();
    m_training_size = static_cast<size_t>(filtered_size * TRAINING_RATIO + 1);
    m_validation_size = filtered_size - m_training_size;

    // pull the RDataFrame data into vectors so they can be iterated over a
    // single thread, since TTrees aren't thread safe of course
    const std::vector<float> energy_vec =
        filtered_df.Take<float>("energy").GetValue();
    const std::vector<float> long_mean_vec =
        filtered_df.Take<float>("long_mean").GetValue();
    const std::vector<float> lat_mean_vec =
        filtered_df.Take<float>("lat_mean").GetValue();

    const size_t size = energy_vec.size();

    auto data_dir = std::string("data/model/");
    std::string training_filename =
        data_dir + tree_name + std::string("_training.root");
    std::string validation_filename =
        data_dir + tree_name + std::string("_validation.root");

    // training takes the vector data from 0 to m_training_size
    m_training_tree = fill_tree(
        tree_name + std::string("_training"), training_filename, min_energy,
        min_long_mean, min_lat_mean, energy_dif, long_mean_dif, lat_mean_dif, 0,
        m_training_size, energy_vec, long_mean_vec, lat_mean_vec
    );

    // validation takes the vector data from m_training_size to filtered_size
    m_validation_tree = fill_tree(
        tree_name + std::string("_validation"), validation_filename, min_energy,
        min_long_mean, min_lat_mean, energy_dif, long_mean_dif, lat_mean_dif,
        m_training_size, filtered_size, energy_vec, long_mean_vec, lat_mean_vec
    );
}

cuts::CutResult
bdts::train_and_validate(std::string model_dir, ROOT::RDataFrame df) {
    // training
    ModelData sig(df, 0, "egamma");
    ModelData bkg(df, 1, "hadron");

    std::string bdt_dirname = files::relative_filename("data/model/bdt");
    // I wanted to do this using an absolute path but DataLoader breaks, even
    // though this bug has supposedly been fixed... So enjoy this hack that
    // will break if the build process ever changes :)
    // https://root-forum.cern.ch/t/specify-absolute-path-for-dataloader-output/38541
    auto dataloader = new TMVA::DataLoader("../../../../data/model/bdt");

    const std::vector<std::string> vars{"energy", "long_mean", "lat_mean"};
    for (const auto &var : vars) {
        dataloader->AddVariable(var);
    }
    dataloader->AddSignalTree(sig.m_training_tree, 1.0);
    dataloader->AddBackgroundTree(bkg.m_training_tree, 1.0);
    dataloader->PrepareTrainingAndTestTree("", "");

    auto output = TFile::Open("/tmp/temp.root", "RECREATE");
    auto factory = new TMVA::Factory(
        "calo", output, "!V:!DrawProgressBar:AnalysisType=Classification"
    );
    factory->BookMethod(
        dataloader, TMVA::Types::kBDT, "BDT", "!V:!H:NTrees=100:MaxDepth=3"
    );
    factory->TrainAllMethods();

    // validation
    TMVA::Experimental::RReader model(
        bdt_dirname + std::string("/weights/calo_BDT.weights.xml")
    );

    // auto variables = model.GetVariableNames();
    // auto prediction = model.Compute({0.5, 0.5, 0.5});
    cuts::CutResult result{0, 0};
    std::vector<float> variables{0, 0, 0};

    Float_t energy;
    Float_t long_mean;
    Float_t lat_mean;

    sig.m_validation_tree->SetBranchAddress("energy", &energy);
    sig.m_validation_tree->SetBranchAddress("long_mean", &long_mean);
    sig.m_validation_tree->SetBranchAddress("lat_mean", &lat_mean);

    for (size_t i = 0; i < sig.m_validation_size; i++) {
        sig.m_validation_tree->GetEntry(i);

        auto classification = model.Compute({energy, long_mean, lat_mean});

        // std::cout << energy << ", " << long_mean << ", " << lat_mean << " | "
        //           << classification[0] << ", " << classification[1] << ", "
        //           << classification[2] << '\n';

        if (classification[0] > 0) {
            result.sig_eff += 1;
        }
    }
    result.sig_eff /= sig.m_validation_size;

    bkg.m_validation_tree->SetBranchAddress("energy", &energy);
    bkg.m_validation_tree->SetBranchAddress("long_mean", &long_mean);
    bkg.m_validation_tree->SetBranchAddress("lat_mean", &lat_mean);

    for (size_t i = 0; i < bkg.m_validation_size; i++) {
        bkg.m_validation_tree->GetEntry(i);

        auto classification = model.Compute({energy, long_mean, lat_mean});

        // std::cout << energy << ", " << long_mean << ", " << lat_mean << " | "
        //           << classification[0] << ", " << classification[1] << ", "
        //           << classification[2] << '\n';

        if (classification[0] > 0) {
            result.bkg_eff += 1;
        }
    }
    result.bkg_eff /= bkg.m_validation_size;

    return result;
}