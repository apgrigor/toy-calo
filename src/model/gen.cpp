#include "gen.h"

#include <functional>
#include <iostream>

#include "TRandom3.h"

TRandom *gen_rand_gen = new TRandom3();

float gen::flat() {
    return gen_rand_gen->Rndm() * (E_UPPER - E_LOWER) + E_LOWER;
}

// float gen::hadron_energy() {
//     return egamma_rand_gen->Rndm() * (E_UPPER - E_LOWER) + E_LOWER;
// }

#define N_SPEC_ENERGIES 5

float spec_energies[N_SPEC_ENERGIES]{5, 10, 20, 50, 100};

float gen::flat_spec() {
    size_t idx = static_cast<size_t>(gen_rand_gen->Rndm() * N_SPEC_ENERGIES);

    return spec_energies[idx];
}

std::function<float()> gen::make_exact(float energy) {
    return std::function<float()>([energy]() { return energy; });
}
