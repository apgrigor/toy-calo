#pragma once

#define NSAMPLE 200

#include "sim.h"

#include <cstdint>
#include <iostream>

#include "Math/QuantFuncMathMore.h"
#include "Math/SpecFuncMathCore.h"
#include "TMath.h"
#include "TRandom3.h"

TRandom *sim_rand_gen = new TRandom3(1000009);

inline float detector_resolution(float energy, float energy_resolution) {
    return TMath::Sqrt(energy_resolution / energy + 0.000035);
}

// gaussian smearing to simulate the precision of the detector
inline void smear_cells(
    float energy_threshold, sim::SimHits &hits, float energy_resolution
) {
    for (uint8_t i = 0; i < DEPTH_CELLS; i++) {
        for (uint8_t j = 0; j < ETA_CELLS; j++) {
            for (uint8_t k = 0; k < PHI_CELLS; k++) {
                float energy = hits.energy[i][j][k];

                if (energy < energy_threshold) {
                    continue;
                }

                hits.energy[i][j][k] = TMath::Abs(
                    energy +
                    sim_rand_gen->Gaus(
                        0,
                        energy * detector_resolution(energy, energy_resolution)
                    )
                );
            }
        }
    }
}

inline float get_egamma_mode(float log_energy) {
    return 0.65 * log_energy - 0.45;
}

inline float get_egamma_alpha(float log_energy) {
    return 1.0 * log_energy + 0.5;
}

// hadron gamma distribution is deeper and wider than egamma
inline float get_hadron_mode(float log_energy) {
    return 1.1 * log_energy + 0.3;
}

inline float get_hadron_alpha(float log_energy) {
    return 3.5 * log_energy + 7.5;
}

// longitudinal and lateral distributions (the latter assumed same in eta and
// phi) are sampled, each point representing part of the deposited energy
// Sampler class must satisfy the following interface:
// class Sampler {
//   public:
//     Sampler(float);
//     void sample_point(float (&point)[3]);
// };
// but this is not expressed in code as C++ didn't have compile-time generics
// before concepts in C++20, and I have to use C++17 for ROOT of course
template <class Sampler>
inline sim::SimHits generate_hits(float total_energy, float energy_resolution) {
    sim::SimHits result;
    for (size_t i = 0; i < DEPTH_CELLS; i++) {
        for (size_t j = 0; j < ETA_CELLS; j++) {
            for (size_t k = 0; k < PHI_CELLS; k++) {
                result.energy[i][j][k] = 0;
            }
        }
    }

    Sampler sampler(total_energy);

    uint8_t point[3];
    const float sample_energy = total_energy / NSAMPLE;

    for (size_t i = 0; i < NSAMPLE; i++) {
        sampler.sample_point(point);

        result.energy[point[0]][point[1]][point[2]] += sample_energy;
    }

    smear_cells(sample_energy, result, energy_resolution);

    return result;
}

class EgammaSampler {
  public:
    EgammaSampler(float energy);

    void sample_point(uint8_t (&point)[3]);

  private:
    float m_long_profile_cdf[DEPTH_CELLS];
    float m_r[DEPTH_CELLS];
    float m_lat_norm[DEPTH_CELLS];
};

inline EgammaSampler::EgammaSampler(float energy) {
    const float log_energy = TMath::Log(energy);

    // parameters depend logarithmically on the energy, coefficients are picked
    // at will so the sim hits have more or less the right qualitative features
    const float mode = get_egamma_mode(log_energy); // the mode i.e. shower max
    const float alpha = get_egamma_alpha(log_energy); // high alpha = low var
    const float beta = (alpha - 1) / mode;

    const float long_profile_norm =
        ROOT::Math::inc_gamma(alpha, beta * DEPTH_CELLS);

    m_long_profile_cdf[DEPTH_CELLS - 1] = 1;

    for (uint8_t t = 0; t < DEPTH_CELLS - 1; t++) {
        m_long_profile_cdf[t] =
            ROOT::Math::inc_gamma(alpha, beta * (t + 1)) / long_profile_norm;
    }

    for (uint8_t t = 0; t < DEPTH_CELLS; t++) {
        m_r[t] = 0.5 + 0.05 * log_energy * t;

        const float mr2 = TMath::Sq(m_r[t]);

        m_lat_norm[t] = 1 - mr2 / (RADIUS_CELLS * RADIUS_CELLS + mr2);
    }
}

// point is a 3D vector of (depth, eta, phi)
inline void EgammaSampler::sample_point(uint8_t (&point)[3]) {
    float r1 = sim_rand_gen->Rndm();

    for (uint8_t t = 0; t < DEPTH_CELLS; t++) {
        if (r1 < m_long_profile_cdf[t]) {
            point[0] = t;
            break;
        }
    }

    float eta;
    float phi;

    const float r2 = sim_rand_gen->Rndm() * m_lat_norm[point[0]];

    const float radius = m_r[point[0]] * TMath::Sqrt(r2 / (1 - r2));

    // somewhat physical mixing angle between eta and phi
    const float angle = sim_rand_gen->Rndm() * 2 * TMath::Pi();

    // the + 0.5 rounds to the nearest integer instead of truncating
    eta = static_cast<uint8_t>(radius * TMath::Cos(angle) + ETA_MID + 0.5);
    phi = static_cast<uint8_t>(radius * TMath::Sin(angle) + PHI_MID + 0.5);

    point[1] = eta;
    point[2] = phi;
}

sim::SimHits sim::egamma_hits(float total_energy, float energy_resolution) {
    return generate_hits<EgammaSampler>(total_energy, energy_resolution);
}

class HadronSampler {
  public:
    HadronSampler(float energy);

    void sample_point(uint8_t (&point)[3]);

  private:
    float m_long_profile_cdf[DEPTH_CELLS];
    float m_r[DEPTH_CELLS];
    float m_lat_norm[DEPTH_CELLS];
};

inline float double_inc_gamma(
    float c, float a1, float b1, float a2, float b2, uint8_t depth
) {
    return ROOT::Math::inc_gamma(a1, b1 * depth) * c +
           ROOT::Math::inc_gamma(a2, b2 * depth) * (1 - c);
}

#define C_EG 0.33

inline HadronSampler::HadronSampler(float energy) {
    const float log_energy = TMath::Log(energy);

    // parameters depend logarithmically on the energy, coefficients are picked
    // at will so the sim hits have more or less the right qualitative features
    const float mode_1 = get_egamma_mode(log_energy); // shallow shower maximum
    const float alpha_1 = get_egamma_alpha(log_energy);
    const float beta_1 = (alpha_1 - 1) / mode_1;

    const float mode_2 = get_hadron_mode(log_energy); // deep shower maximum
    const float alpha_2 = get_hadron_alpha(log_energy);
    const float beta_2 = (alpha_2 - 1) / mode_2;

    const float long_profile_norm =
        double_inc_gamma(C_EG, alpha_1, beta_1, alpha_2, beta_2, DEPTH_CELLS);

    m_long_profile_cdf[DEPTH_CELLS - 1] = 1;

    for (uint8_t t = 0; t < DEPTH_CELLS - 1; t++) {
        m_long_profile_cdf[t] =
            double_inc_gamma(C_EG, alpha_1, beta_1, alpha_2, beta_2, t + 1) /
            long_profile_norm;
    }

    for (uint8_t t = 0; t < DEPTH_CELLS; t++) {
        m_r[t] = TMath::Sq(0.5 + 0.1 * log_energy * t);

        const float mr2 = TMath::Sq(m_r[t]);

        m_lat_norm[t] = 1 - mr2 / (RADIUS_CELLS * RADIUS_CELLS + mr2);
    }
}

// point is a 3D vector of (depth, eta, phi)
inline void HadronSampler::sample_point(uint8_t (&point)[3]) {
    const float r1 = sim_rand_gen->Rndm();

    for (uint8_t t = 0; t < DEPTH_CELLS; t++) {
        if (r1 < m_long_profile_cdf[t]) {
            point[0] = t;
            break;
        }
    }

    float eta;
    float phi;

    const float r2 = sim_rand_gen->Rndm() * m_lat_norm[point[0]];

    const float radius = m_r[point[0]] * TMath::Sqrt(r2 / (1 - r2));

    // somewhat physical mixing angle between eta and phi
    const float angle = sim_rand_gen->Rndm() * 2 * TMath::Pi();

    // the + 0.5 rounds to the nearest integer instead of truncating
    eta = static_cast<uint8_t>(radius * TMath::Cos(angle) + ETA_MID + 0.5);
    phi = static_cast<uint8_t>(radius * TMath::Sin(angle) + PHI_MID + 0.5);

    point[1] = eta;
    point[2] = phi;
}

sim::SimHits sim::hadron_hits(float total_energy, float energy_resolution) {
    return generate_hits<HadronSampler>(total_energy, energy_resolution);
}
