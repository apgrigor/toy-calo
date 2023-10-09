#define TARGET_SIGNAL_EFF 0.995
#define TARGET_SIGNAL_TOLERANCE 0.00005

#include <vector>

#include "cuts.h"

namespace discr {

struct DiscrResult {
    float sig_eff;
    float bkg_eff;
    float value;
    float cut;
};

std::vector<DiscrResult> discriminate(
    std::vector<float> values, std::vector<std::string> input_file_paths,
    float initial_lower_cut, float initial_upper_cut
);

} // namespace discr
