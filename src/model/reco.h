#pragma once

#include <cstddef>
#include <cstdint>

#include "sim.h"

// reconstruction: turning the sim hit grid into shower shape parameters

#define NOMINAL_NTOYS 1000000

// note that the NTHREADS symbol is NOT defined in the source but
// passed to the compiler by the build script, as it can change depending
// on the system it's built. Implementation in xmake.lua
#define TOYS_PER_THREAD (NOMINAL_NTOYS / NTHREADS + 1)

// the total number of toys might be a bit larger than NOMINAL_NTOYS
#define NTOYS (NTHREADS * TOYS_PER_THREAD)

namespace reco {

class ShowerArena {
  public:
    float m_energy[NTOYS];     // total energy of the incoming particle
    float m_long_mean[NTOYS];  // energy-weighted sum of depths
    float m_lat_mean[NTOYS];   // energy-weighted sum of radii
    uint8_t m_depth_m1[NTOYS]; // first mode
    uint8_t m_depth_m2[NTOYS]; // second mode
    uint8_t m_em_frac[NTOYS];  // % of energy in the EM part

    void reco_fill(size_t thread_num, const sim::SimHits &);
};

} // namespace reco