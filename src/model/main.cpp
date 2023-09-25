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

    std::cout << "running toy model for dataset \"" << option << "\"..."
              << std::endl;

    if (option.compare(std::string("flat")) == 0) {
        toy::ToyRunner flat_egamma{gen::flat, sim::egamma_hits};
        toy::ToyRunner flat_hadron{gen::flat, sim::hadron_hits};

        toy::run("flat", flat_egamma, flat_hadron);
    } else if (option.compare(std::string("flat_spec")) == 0) {
        toy::ToyRunner flat_spec_egamma{gen::flat_spec, sim::egamma_hits};
        toy::ToyRunner flat_spec_hadron{gen::flat_spec, sim::hadron_hits};

        toy::run("flat_spec", flat_spec_egamma, flat_spec_hadron);
    }

    std::cout << "\n\ntotal time taken: " << watch.RealTime() << 's'
              << std::endl;

    return 0;
}
