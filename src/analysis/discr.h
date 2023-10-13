#define TARGET_SIGNAL_EFF 0.992
#define TARGET_SIGNAL_TOLERANCE 0.00001

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

void save_discr(std::vector<DiscrResult>, std::string file_path);

} // namespace discr
