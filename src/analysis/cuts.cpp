#include "cuts.h"

#include <optional>
#include <string>

#include "../model/reco.h"
#include "ROOT/RDataFrame.hxx"

template <typename T> cuts::SimpleCut<T>::SimpleCut() {
    m_name = "";
    m_lower = std::nullopt;
    m_upper = std::nullopt;
}

template <typename T>
cuts::SimpleCut<T>::SimpleCut(
    std::string name, std::optional<T> lower_lim, std::optional<T> upper_lim
) {
    m_name = name;
    m_lower = lower_lim;
    m_upper = upper_lim;
}

template <typename T> cuts::SimpleCut<T> cuts::SimpleCut<T>::empty() {
    return SimpleCut<T>();
}

template <typename T>
cuts::SimpleCut<T> cuts::SimpleCut<T>::lower(std::string name, T lower_lim) {
    return SimpleCut<T>(name, std::optional<T>(lower_lim), std::nullopt);
}

template <typename T>
cuts::SimpleCut<T> cuts::SimpleCut<T>::upper(std::string name, T upper_lim) {
    return SimpleCut<T>(name, std::nullopt, std::optional<T>(upper_lim));
}

template <typename T>
cuts::SimpleCut<T>
cuts::SimpleCut<T>::two_way(std::string name, T lower_lim, T upper_lim) {
    return SimpleCut<T>(
        name, std::optional<T>(lower_lim), std::optional<T>(upper_lim)
    );
}

template <typename T>
inline std::function<bool(T)> cuts::SimpleCut<T>::get_cut() {
    const bool lower_exists = m_lower.has_value();
    const bool upper_exists = m_upper.has_value();

    if (lower_exists) {
        const T lower_value = m_lower.value();
        if (upper_exists) {
            const T upper_value = m_upper.value();

            return [lower_value, upper_value](T variable) {
                return (variable > lower_value) && (variable < upper_value);
            };
        } else {
            return [lower_value](T variable) { return variable > lower_value; };
        }
    } else if (upper_exists) {
        const T upper_value = m_upper.value();

        return [upper_value](T variable) { return variable < upper_value; };
    } else {
        return [](T variable) { return true; };
    }
}

// C++ needs these declarations to know what templates to build
template class cuts::SimpleCut<float>;
template class cuts::SimpleCut<UInt_t>;

cuts::Cutter::Cutter(
    SimpleCut<float> energy_cut, SimpleCut<float> long_mean_cut,
    SimpleCut<float> lat_mean_cut, SimpleCut<UInt_t> depth_m1_cut,
    SimpleCut<UInt_t> depth_m2_cut, SimpleCut<UInt_t> em_frac_cut
) {
    m_energy_cut = energy_cut.get_cut();
    m_long_mean_cut = long_mean_cut.get_cut();
    m_lat_mean_cut = lat_mean_cut.get_cut();
    m_depth_m1_cut = depth_m1_cut.get_cut();
    m_depth_m2_cut = depth_m2_cut.get_cut();
    m_em_frac_cut = em_frac_cut.get_cut();
}

inline bool cuts::Cutter::cut_fn(
    float energy, float long_mean, float lat_mean, UInt_t depth_m1,
    UInt_t depth_m2, UInt_t em_frac
) {
    return m_energy_cut(energy) && m_long_mean_cut(long_mean) &&
           m_lat_mean_cut(lat_mean) && m_depth_m1_cut(depth_m1) &&
           m_depth_m2_cut(depth_m2) && m_em_frac_cut(em_frac);
}

auto cuts::Cutter::cut(ROOT::RDataFrame df) {
    return df.Filter(
        [this](
            float energy, float long_mean, float lat_mean, UInt_t depth_m1,
            UInt_t depth_m2, UInt_t em_frac
        ) {
            return this->cut_fn(
                energy, long_mean, lat_mean, depth_m1, depth_m2, em_frac
            );
        },
        {"energy", "long_mean", "lat_mean", "depth_m1", "depth_m2", "em_frac"}
    );
}

inline float float_div(size_t num, size_t den) {
    return static_cast<float>(num) / static_cast<float>(den);
}

cuts::CutResult cuts::Cutter::eval_cut(ROOT::RDataFrame df) {
    size_t total_signal =
        df.Filter([](UInt_t pid) { return pid == 0; }, {"pid"})
            .Count()
            .GetValue();

    size_t total_background =
        df.Filter([](UInt_t pid) { return pid == 1; }, {"pid"})
            .Count()
            .GetValue();

    auto cut_df = this->cut(df);

    size_t cut_signal =
        cut_df.Filter([](UInt_t pid) { return pid == 0; }, {"pid"})
            .Count()
            .GetValue();

    size_t cut_background = cut_df.Count().GetValue() - cut_signal;

    return cuts::CutResult{
        .sig_eff = float_div(cut_signal, total_signal),
        .bkg_eff = float_div(cut_background, total_background)};
}
