#include "toy.h"

#include <cstdarg>
#include <iostream>
#include <string>
#include <thread>

#include "../utils/files.h"
#include "../utils/logging.h"
#include "TFile.h"
#include "TTree.h"
#include "gen.h"
#include "reco.h"
#include "sim.h"

// every thread has its own starting index in the arena, they never overlap
toy::ThreadArena::ThreadArena() {
    for (uint8_t pid = 0; pid < NPIDS; pid++) {
        for (uint8_t i = 0; i < NTHREADS; i++) {
            m_current_idx[pid][i] = TOYS_PER_THREAD * i;
        }
    }
}

size_t count = 0;

inline void toy::ThreadArena::fill(
    uint8_t thread_num, uint8_t pid, float energy, const sim::SimHits &sim_hits
) {
    m_arena[pid].reco_fill(m_current_idx[pid][thread_num], sim_hits);

    m_current_idx[pid][thread_num]++;
}

inline void toy::ThreadArena::save(std::string dataset_name) {
    TFile *outfile = files::open_next_data_file(dataset_name);

    Float_t energy;
    Float_t long_mean;
    Float_t lat_mean;
    UInt_t depth_m1;
    UInt_t depth_m2;
    UInt_t em_frac;
    UInt_t particle_id;

    auto tree = new TTree("toy-calo", "Toy model results");
    tree->Branch("energy", &energy, "energy/F");
    tree->Branch("long_mean", &long_mean, "long_mean/F");
    tree->Branch("lat_mean", &lat_mean, "lat_mean/F");
    tree->Branch("depth_m1", &depth_m1, "depth_m1/i");
    tree->Branch("depth_m2", &depth_m2, "depth_m2/i");
    tree->Branch("em_frac", &em_frac, "em_frac/i");
    tree->Branch("pid", &particle_id, "pid/i");

    for (uint8_t pid = 0; pid < NPIDS; pid++) {
        particle_id = pid;
        for (size_t ntoy = 0; ntoy < NTOYS; ntoy++) {
            energy = m_arena[pid].m_energy[ntoy];
            long_mean = m_arena[pid].m_long_mean[ntoy];
            lat_mean = m_arena[pid].m_lat_mean[ntoy];
            depth_m1 = m_arena[pid].m_depth_m1[ntoy];
            depth_m2 = m_arena[pid].m_depth_m2[ntoy];
            em_frac = m_arena[pid].m_em_frac[ntoy];

            tree->Fill();
        }
    }

    tree->Write();

    outfile->Close();
}

// clang-format off
void toy::run(const char *dataset_name, toy::ToyRunner runners[NPIDS]) {
    auto toy_pbar = new logging::ProgressBar(NTOYS * NPIDS);

    auto arena = new toy::ThreadArena();

    #ifdef SNAP
        auto snapper = new logging::SimHitSnapper(dataset_name, NTOYS);
    #endif

    for (uint8_t thread_num = 0; thread_num < NTHREADS; thread_num++) {
        arena->m_threads[thread_num] = std::thread(
            [ // variables to be passed into the closure
                #ifdef SNAP // sorry for ugly syntax, blame the preprocessor
                    &snapper,
                #endif
                &runners, &arena, thread_num, toy_pbar
            ]() {
                for (uint8_t pid = 0; pid < NPIDS; pid++) {
                    for (size_t ntoys = 0; ntoys < TOYS_PER_THREAD; ntoys++) {
                        float energy = runners[pid].gen_func();
                        auto sim_hits = runners[pid].sim_func(energy, runners[pid].energy_resolution);

                        #ifdef SNAP
                            snapper->snap(thread_num, pid, energy, sim_hits);
                        #endif

                        arena->fill(thread_num, pid, energy, sim_hits);

                        toy_pbar->update();
                    }
                }
            }
        );
    }

    for (uint8_t thread_num = 0; thread_num < NTHREADS; thread_num++) {
        arena->m_threads[thread_num].join();
    }

    toy_pbar->finish();

    #ifdef SNAP
        snapper->save();
    #endif

    arena->save(dataset_name);
}
// clang-format on

void toy::run(const char *dataset_name, ...) {
    va_list args;
    va_start(args, dataset_name);

    toy::ToyRunner runner_array[NPIDS];

    for (size_t pid = 0; pid < NPIDS; pid++) {
        runner_array[pid] = va_arg(args, toy::ToyRunner);
    }

    run(dataset_name, runner_array);
}
