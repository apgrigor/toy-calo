#pragma once

#define E_LOWER 2
#define E_UPPER 100

// number of different particle types
#define NPIDS 2

#include <functional>

namespace gen {

float flat(void); // uniform from E_LOWER to E_UPPER

// float exp(); // exponential from E_LOWER to E_UPPER

float flat_spec(void
); // uniform but can only take specific values: 5,10,20,50,100

std::function<float()> make_exact(float energy);

} // namespace gen
