#pragma once

#include <optional>
#include <string>

#include "ROOT/RDataFrame.hxx"

namespace cuts {

template <typename T> class SimpleCut {
  public:
    std::string m_name;
    std::optional<T> m_lower;
    std::optional<T> m_upper;

    SimpleCut();

    SimpleCut(
        std::string name, std::optional<T> lower_lim, std::optional<T> upper_lim
    );

    static SimpleCut<T> empty();
    static SimpleCut<T> lower(std::string name, T lower_lim);
    static SimpleCut<T> upper(std::string name, T upper_lim);
    static SimpleCut<T> two_way(std::string name, T lower_lim, T upper_lim);

    std::function<bool(T)> get_cut();
};

struct CutResult {
    float sig_eff; // how much of the signal got left
    float bkg_eff; // how much of the background got left
};

class Cutter {
  public:
    Cutter(SimpleCut<float>, SimpleCut<float>, SimpleCut<float>, SimpleCut<UInt_t>, SimpleCut<UInt_t>, SimpleCut<UInt_t>);

    // sorry for auto: This returns the result of an RDataFrame.Filter, the
    // type of which is pretty obtuse, but it functions like an RDF
    auto cut(ROOT::RDataFrame);

    CutResult eval_cut(ROOT::RDataFrame);

  private:
    std::function<bool(float)> m_energy_cut;
    std::function<bool(float)> m_long_mean_cut;
    std::function<bool(float)> m_lat_mean_cut;
    std::function<bool(UInt_t)> m_depth_m1_cut;
    std::function<bool(UInt_t)> m_depth_m2_cut;
    std::function<bool(UInt_t)> m_em_frac_cut;

    bool cut_fn(float, float, float, UInt_t, UInt_t, UInt_t);
};

} // namespace manual