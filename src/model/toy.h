#pragma once

// complete model runner, designed to be multithreaded for performance

#include <string>
#include <thread>

#include "gen.h"
#include "reco.h"
#include "sim.h"

namespace toy {

// holds the data of all the threads at the same time. Every thread has a
// separate segment of the arena, so filling is thread safe
class ThreadArena {
  public:
    reco::ShowerArena m_arena[NPIDS];
    std::thread m_threads[NTHREADS];

    ThreadArena();

    void
    fill(uint8_t thread_num, uint8_t pid, float energy, const sim::SimHits &);

    void save(std::string file_path);

  private:
    size_t m_current_idx[NPIDS][NTHREADS];
};

struct ToyRunner {
    float (*gen_func)();
    sim::SimHits (*sim_func)(float);
};

void run(const char *dataset_name, toy::ToyRunner runners[NPIDS]);

void run(const char *dataset_name, ...);

ThreadArena *run_egamma();

ThreadArena *run_hadron();

} // namespace toy