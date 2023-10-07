#include <iostream>

#include "../utils/files.h"
#include "ROOT/RDataFrame.hxx"
#include "TFile.h"
#include "TROOT.h"
#include "bdts.h"
#include "cuts.h"

int main(int argc, char **argv) {
    ROOT::EnableImplicitMT();

    auto option = std::string("cuts");
    if (argc > 1) {
        option = std::string(argv[1]);
    }

    auto infile = files::open("data/flat/data_03.root");

    ROOT::RDataFrame df("toy-calo", infile);

    cuts::CutResult res;

    if (option.compare(std::string("cuts")) == 0) {
        std::cout << "using method " << option << std::endl;
        cuts::Cutter cutter(
            (cuts::SimpleCut<float>::empty()),
            cuts::SimpleCut<float>::upper("long_mean", 3),
            cuts::SimpleCut<float>::upper("lat_mean", 0.4),
            cuts::SimpleCut<UInt_t>::upper("depth_m1", 4),
            cuts::SimpleCut<UInt_t>::upper("depth_m2", 4),
            cuts::SimpleCut<UInt_t>::empty()
        );

        res = cutter.eval_cut(df);
    } else if (option.compare(std::string("bdts")) == 0) {
        std::cout << "using method " << option << std::endl;
        res = bdts::train_and_validate("data/model", df);
    } else if (option.compare(std::string("discr"))) {
        std::cout << "comparing discriminatory capabilities" << std::endl;
        res = bdts::train_and_validate("data/model", df);
    } else {
        std::cout << "method " << option << " doesn't exist!" << std::endl;
        return 1;
    }

    std::cout << "signal efficiency: " << res.sig_eff << '\n';
    std::cout << "background efficiency: " << res.bkg_eff << std::endl;
}
