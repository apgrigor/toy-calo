
#include "discr.h"

#include <thread>

#include "../utils/files.h"
#include "ROOT/RDataFrame.hxx"
#include "TFile.h"
#include "TMath.h"
#include "cuts.h"

// will tamper with the long_mean cut until it finds one that yields acceptable
// signal efficiency
inline discr::DiscrResult discr_at_constant_sig_eff(
    ROOT::RDataFrame df, float target_sig_eff, float target_sig_tolerance,
    float initial_lower_cut, float initial_upper_cut
) {
    cuts::Cutter cutter(
        (cuts::SimpleCut<float>::empty()),
        cuts::SimpleCut<float>::upper("long_mean", 3),
        cuts::SimpleCut<float>::empty(), cuts::SimpleCut<UInt_t>::empty(),
        cuts::SimpleCut<UInt_t>::empty(), cuts::SimpleCut<UInt_t>::empty()
    );

    auto get_cut_result = [&df, &cutter](float long_mean_cut) {
        cutter.m_long_mean_cut =
            cuts::SimpleCut<float>::upper("long_mean", long_mean_cut).get_cut();

        cuts::CutResult result = cutter.eval_cut(df);
        return result;
    };
    float lower_cut = initial_lower_cut;
    float upper_cut = initial_upper_cut;
    float mid_cut = 0;

    cuts::CutResult lower_cut_res = get_cut_result(lower_cut);
    cuts::CutResult upper_cut_res = get_cut_result(upper_cut);
    cuts::CutResult mid_cut_res;

    float lower_dif = target_sig_eff - lower_cut_res.sig_eff;
    float upper_dif = target_sig_eff - upper_cut_res.sig_eff;
    float mid_dif = 1;

    size_t increments = 0;

    // use bisection method to find an acceptable result
    while (TMath::Abs(mid_dif) > target_sig_tolerance) {
        mid_cut = 0.5 * (upper_cut + lower_cut);

        mid_cut_res = get_cut_result(mid_cut);
        mid_dif = target_sig_eff - mid_cut_res.sig_eff;

        if (lower_dif * mid_dif < 0) {
            upper_cut = mid_cut;
            upper_dif = mid_dif;
        } else if (mid_dif * upper_dif < 0) {
            lower_cut = mid_cut;
            lower_dif = mid_dif;
        } else {
            std::cout << "failed in " << increments << " steps, " << lower_dif
                      << ", "
                      << ", " << mid_dif << ", " << upper_dif << std::endl;
            return discr::DiscrResult{-1, -1, -1, -1};
        }
        increments++;
    }
    std::cout << "finished in " << increments << " steps." << std::endl;
    return discr::DiscrResult{
        .sig_eff = mid_cut_res.sig_eff,
        .bkg_eff = mid_cut_res.bkg_eff,
        .value = -1,
        .cut = mid_cut};
}

std::vector<discr::DiscrResult> discr::discriminate(
    std::vector<float> values, std::vector<std::string> input_file_paths,
    float initial_lower_cut, float initial_upper_cut
) {
    size_t len = input_file_paths.size();

    std::vector<discr::DiscrResult> result(len);
    std::vector<float> bkg_efficiencies(len);
    std::vector<std::thread> threads(len);

    for (size_t i = 0; i < len; i++) {
        std::string input_file_path = input_file_paths[i];

        threads[i] = std::thread([i, &values, &result, input_file_path,
                                  initial_lower_cut, initial_upper_cut]() {
            TFile *input_file = files::open(input_file_path);
            ROOT::RDataFrame df("toy-calo", input_file);

            result[i] = discr_at_constant_sig_eff(
                df, TARGET_SIGNAL_EFF, TARGET_SIGNAL_TOLERANCE,
                initial_lower_cut, initial_upper_cut
            );

            result[i].value = values[i];
        });
    }

    for (size_t i = 0; i < len; i++) {
        threads[i].join();
    }

    return result;
}
