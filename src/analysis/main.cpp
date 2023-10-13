#include <iostream>

#include "../utils/files.h"
#include "ROOT/RDataFrame.hxx"
#include "TFile.h"
#include "TROOT.h"
#include "bdts.h"
#include "cuts.h"
#include "discr.h"

int main(int argc, char **argv) {
    ROOT::EnableImplicitMT();

    auto option = std::string("cuts");
    if (argc > 1) {
        option = std::string(argv[1]);
    }

    auto infile = files::open("data/flat/data_05.root");

    ROOT::RDataFrame df("toy-calo", infile);

    cuts::CutResult res;

    if (option.compare(std::string("cuts")) == 0) {
        std::cout << "using method " << option << std::endl;
        cuts::Cutter cutter(
            (cuts::SimpleCut<float>::empty()),
            cuts::SimpleCut<float>::upper("long_mean", 2.86935),
            cuts::SimpleCut<float>::empty(),
            cuts::SimpleCut<UInt_t>::empty(),
            cuts::SimpleCut<UInt_t>::empty(),
            cuts::SimpleCut<UInt_t>::empty()
        );

        res = cutter.eval_cut(df);
    } else if (option.compare(std::string("bdts")) == 0) {
        std::cout << "using method " << option << std::endl;
        res = bdts::train_and_validate("data/model", df);
    } else if (option.compare(std::string("discr")) == 0) {
        std::cout << "comparing discriminatory capabilities" << std::endl;

        size_t reso_num_files = 12;
        std::vector<std::string> reso_input_file_paths(reso_num_files);

        for (size_t i = 0; i < 10; i++) {
            reso_input_file_paths[i] = std::string("data/discr/flat/data_0") +
                                       std::to_string(i) + std::string(".root");
        }
        for (size_t i = 10; i < reso_num_files; i++) {
            reso_input_file_paths[i] = std::string("data/discr/flat/data_") +
                                       std::to_string(i) + std::string(".root");
        }

        const std::vector<discr::DiscrResult> discr_by_resolution =
            discr::discriminate(
                {-5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6},
                reso_input_file_paths, 1.01, 4
            );

        std::cout << "discrimination by resolution:\n";
        for (const auto dbr : discr_by_resolution) {
            std::cout << dbr.sig_eff << "," << dbr.bkg_eff << "," << dbr.value
                      << "," << dbr.cut << '\n';
        }

        discr::save_discr(
            discr_by_resolution, files::relative_filename("data/discr/dbr.csv")
        );

        size_t energy_num_files = 12;
        std::vector<std::string> energy_input_file_paths(energy_num_files);


        for (size_t i = 0; i < 10; i++) {
            energy_input_file_paths[i] = std::string("data/discr/exact/data_0") +
                                       std::to_string(i) + std::string(".root");
        }
        for (size_t i = 10; i < energy_num_files; i++) {
            energy_input_file_paths[i] = std::string("data/discr/exact/data_") +
                                       std::to_string(i) + std::string(".root");
        }

        const std::vector<discr::DiscrResult> discr_by_energy =
            discr::discriminate(
                {3, 4, 5, 6, 7, 8, 10, 20, 35, 50, 75, 100},
                energy_input_file_paths, 0.35, 3.5
            );

        std::cout << "discrimination by energy:\n";
        for (const auto dbe : discr_by_energy) {
            std::cout << dbe.sig_eff << "," << dbe.bkg_eff << "," <<
            dbe.value
                      << "," << dbe.cut << '\n';
        }

        discr::save_discr(discr_by_energy,
        files::relative_filename("data/discr/dbe.csv"));

    } else {
        std::cout << "method " << option << " doesn't exist!" << std::endl;
        return 1;
    }

    std::cout << "signal efficiency: " << res.sig_eff << '\n';
    std::cout << "background efficiency: " << res.bkg_eff << std::endl;
}
