#include <iostream>
#include <thread>

#include "../utils/logging.h"
#include "TROOT.h"
#include "TStopwatch.h"
#include "gen.h"
#include "sim.h"
#include "toy.h"

int main(int argc, char **argv) {
    ROOT::EnableImplicitMT();

    auto watch = TStopwatch();

    auto option = std::string("flat");
    if (argc > 1) {
        option = std::string(argv[1]);
    }
    float energy_resolution = 0.3;
    if (argc > 2) {
        energy_resolution = std::stof(argv[2]);
    }

    std::cout << "running toy model for dataset \"" << option << "\"..."
              << std::endl;

    if (option.compare(std::string("flat")) == 0) {
        toy::ToyRunner flat_egamma{
            energy_resolution, gen::flat, sim::egamma_hits};
        toy::ToyRunner flat_hadron{
            energy_resolution, gen::flat, sim::hadron_hits};

        toy::run("flat", flat_egamma, flat_hadron);
    } else if (option.compare(std::string("flat_spec")) == 0) {
        toy::ToyRunner flat_spec_egamma{
            energy_resolution, gen::flat_spec, sim::egamma_hits};
        toy::ToyRunner flat_spec_hadron{
            energy_resolution, gen::flat_spec, sim::hadron_hits};

        toy::run("flat_spec", flat_spec_egamma, flat_spec_hadron);
    } else if (option.compare(std::string("discr")) == 0) {
        toy::ToyRunner flat_spec_egamma{
            energy_resolution, gen::flat_spec, sim::egamma_hits};
        toy::ToyRunner flat_spec_hadron{
            energy_resolution, gen::flat_spec, sim::hadron_hits};

        toy::run("discr", flat_spec_egamma, flat_spec_hadron);
    }

    std::cout << "\n\ntotal time taken: " << watch.RealTime() << 's'
              << std::endl;

    return 0;
}
