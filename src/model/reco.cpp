#pragma once

#include "reco.h"

#include <algorithm>

#include "TMath.h"
#include "sim.h"

inline void update_modes(
    float energy, uint8_t depth, float (&modes)[2], uint8_t (&mode_depths)[2]
) {
    if (energy > modes[0]) {
        modes[1] = modes[0];
        modes[0] = energy;
        mode_depths[1] = mode_depths[0];
        mode_depths[0] = depth;
    } else if (energy > modes[1]) {
        modes[1] = energy;
        mode_depths[1] = depth;
    }
}

inline uint8_t u8_distance(uint8_t u1, uint8_t u2) {
    if (u1 < u2) {
        return u2 - u1;
    }
    return u1 - u2;
}

// by "box" I mean the equivalent of a cylindrical surface, but rectangles
// every point in eta, phi will correspond to one such surface
inline uint8_t get_box_idx(uint8_t eta_cell, uint8_t phi_cell) {
    return std::max(
        u8_distance(eta_cell, ETA_MID), u8_distance(phi_cell, PHI_MID)
    );
}

void reco::ShowerArena::reco_fill(size_t idx, const sim::SimHits &hits) {
    float total_energy = 0;
    float em_energy = 0;

    float modes[2]{0, 0};
    uint8_t mode_depths[2]{0, 0};

    float depth_mean = 0;
    float eta_mean = 0;
    float phi_mean = 0;

    for (uint8_t i = 0; i < DEPTH_CELLS; i++) {
        for (uint8_t j = 0; j < ETA_CELLS; j++) {
            for (uint8_t k = 0; k < PHI_CELLS; k++) {
                float energy = hits.energy[i][j][k];

                update_modes(energy, i, modes, mode_depths);

                depth_mean += energy * i;
                eta_mean += energy * (j - ETA_MID);
                phi_mean += energy * (k - PHI_MID);

                total_energy += energy;
            }
        }

        if (i == DEPTH_EM) {
            em_energy = total_energy;
        }
    }

    m_energy[idx] = total_energy;

    m_depth_m1[idx] = mode_depths[0];
    m_depth_m2[idx] = mode_depths[1];

    m_long_mean[idx] = depth_mean / total_energy;
    m_lat_mean[idx] = TMath::Sqrt(
        TMath::Sq(eta_mean / total_energy) + TMath::Sq(phi_mean / total_energy)
    );

    // percentage represented as u8 to save space
    m_em_frac[idx] = static_cast<uint8_t>(em_energy / total_energy * 100);
}

