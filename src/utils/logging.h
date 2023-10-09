#pragma once

#define NSEGMENT 50
#define NOMINAL_NSNAPS 20
#define SNAPS_PER_THREAD (NOMINAL_NSNAPS / NTHREADS + 1)
#define NSNAPS (SNAPS_PER_THREAD * NTHREADS)

#include <cstddef>
#include <cstdint>
#include <ctime>
#include <string>
#include <vector>

#include "../model/gen.h"
#include "../model/sim.h"

namespace logging {

class ProgressBar {
  public:
    ProgressBar(size_t max_events);

    void update();

    void finish();

  private:
    size_t m_current_event;
    size_t m_current_segment;
    size_t m_events_per_segment;
    std::string m_buffer;
};

// for saving snapshots of the sim hits, as they are normally discarded after
// the shower variables have been computed (see reco.cpp)
struct SimHitSnap {
    sim::SimHits hits;
    float total_energy;
    uint8_t pid;
};

class SimHitSnapper {
  public:
    // note that the constructor expects the max events PER PARTICLE ID
    SimHitSnapper(
        std::string dataset_base_dir, std::string dataset_name,
        size_t max_events
    );

    void snap(uint8_t thread_num, uint8_t pid, float energy, sim::SimHits hits);

    void save();

  private:
    std::string m_dataset_base_dir;
    std::string m_dataset_name;
    size_t m_current_event[NPIDS][NTHREADS];
    size_t m_events_per_snap_thread_pid; // I'm sorry for this
    size_t m_current_idx[NPIDS][NTHREADS];
    SimHitSnap m_arena[NPIDS][NSNAPS];
};

} // namespace logging
