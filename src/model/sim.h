#pragma once

#define DEPTH_CELLS 9
#define ETA_CELLS 9
#define PHI_CELLS 9
#define ETA_MID 4
#define PHI_MID 4
#define DEPTH_EM 3

// the number of possible sqrt((eta-ETA_MID)^2 + (phi-PHI_MID)^2) values
#define RADIUS_CELLS 4

namespace sim {

// a depth x eta x phi grid, where [0..DEPTH_EM] will be the EM calorimeter
// and the rest will be the hadronic calorimeter
struct SimHits {
    float energy[DEPTH_CELLS][ETA_CELLS][PHI_CELLS];
};

SimHits egamma_hits(float total_energy);

SimHits hadron_hits(float total_energy);

} // namespace sim