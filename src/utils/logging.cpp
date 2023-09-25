#define PERCENTS_PER_SEGMENT (100.0 / NSEGMENT)

#include "logging.h"

#include <ctime>
#include <iostream>
#include <string>

#include "TFile.h"
#include "TTree.h"
#include "files.h"

logging::ProgressBar::ProgressBar(size_t max_events) {
    m_current_event = 0;
    m_current_segment = 0;
    m_events_per_segment = max_events / NSEGMENT;

    m_buffer.reserve(NSEGMENT);
    for (size_t i = 0; i < NSEGMENT; i++) {
        m_buffer.push_back(' ');
    }
}

// this isn't thread safe in the slightest, and sometimes causes progress bar
// bugs. Doesn't affect the data though, so not important enough to fix
void logging::ProgressBar::update() {
    m_current_event++;

    if (m_current_event % m_events_per_segment == 0) {
        m_buffer[m_current_segment] = '=';

        std::cout << "[" << m_buffer << "] ";

        m_current_segment++;

        std::cout << int(m_current_segment * PERCENTS_PER_SEGMENT) << " %\r";
        std::cout.flush();
    }
}

void logging::ProgressBar::finish() {
    std::cout << '[';
    for (size_t i = 0; i < NSEGMENT; i++) {
        std::cout << '=';
    }
    std::cout << "] 100" << std::endl;

    delete this;
}

logging::SimHitSnapper::SimHitSnapper(
    std::string dataset_name, size_t max_events
) {
    m_dataset_name = dataset_name;

    m_events_per_snap_thread_pid = max_events / NSNAPS / NTHREADS;

    for (uint8_t pid = 0; pid < NPIDS; pid++) {
        for (uint8_t i = 0; i < NTHREADS; i++) {
            m_current_event[pid][i] = 0;
            m_current_idx[pid][i] = SNAPS_PER_THREAD * i;
        }

        for (size_t a = 0; a < NSNAPS; a++) {
            m_arena[pid][a].total_energy = 0;

            for (size_t i = 0; i < DEPTH_CELLS; i++) {
                for (size_t j = 0; j < ETA_CELLS; j++) {
                    for (size_t k = 0; k < PHI_CELLS; k++) {
                        m_arena[pid][a].hits.energy[i][j][k] = 0;
                    }
                }
            }
        }
    }
}

void logging::SimHitSnapper::snap(
    uint8_t thread_num, uint8_t pid, float energy, sim::SimHits hits
) {
    m_current_event[pid][thread_num]++;

    uint8_t current_idx = m_current_idx[pid][thread_num];

    if ((m_current_event[pid][thread_num] % m_events_per_snap_thread_pid == 0
        ) &&
        (current_idx < (SNAPS_PER_THREAD * (thread_num + 1)))) {
        m_arena[pid][current_idx].total_energy = energy;
        m_arena[pid][current_idx].hits = hits;
        m_arena[pid][current_idx].pid = pid;

        m_current_idx[pid][thread_num]++;
    }
}

void logging::SimHitSnapper::save() {
    TFile *outfile = files::open_next_root_file(m_dataset_name, "snap");

    float hits[DEPTH_CELLS][ETA_CELLS][PHI_CELLS];
    float energy;
    UInt_t particle_id;

    auto tree = new TTree("snapshots", "Toy model snapshots");
    tree->Branch("energy", &energy, "energy/F");
    char type_info[30];
    std::sprintf(
        type_info, "sim_hits[%d][%d][%d]/F", DEPTH_CELLS, ETA_CELLS, PHI_CELLS
    );
    tree->Branch("sim_hits", hits, type_info);
    tree->Branch("pid", &particle_id, "pid/i");

    for (uint8_t pid = 0; pid < NPIDS; pid++) {
        particle_id = pid;
        for (size_t a = 0; a < NSNAPS; a++) {
            energy = m_arena[pid][a].total_energy;

            for (size_t i = 0; i < DEPTH_CELLS; i++) {
                for (size_t j = 0; j < ETA_CELLS; j++) {
                    for (size_t k = 0; k < PHI_CELLS; k++) {
                        hits[i][j][k] = m_arena[pid][a].hits.energy[i][j][k];
                    }
                }
            }

            tree->Fill();
        }
    }
    tree->Write();

    outfile->Close();
}