#pragma once

#include <cstdint>
#include <string>
#include "ROOT/RDataFrame.hxx"
#include "cuts.h"

namespace bdts {

// class Fitter {
//   public:
//     std::string m_model_dir;

//     Fitter(std::string);

//     void train_and_validate(ROOT::RDataFrame df);
// };

cuts::CutResult train_and_validate(std::string model_dir, ROOT::RDataFrame df);

} // namespace bdts