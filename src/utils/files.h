#pragma once

#include "TFile.h"

namespace files {

// gets filename relative to the project directory
std::string relative_filename(std::string);

// opens file for reading relative to the project directory
TFile *open(std::string path);

// opens file for writing relative to the project directory
TFile *recreate(std::string path);

TFile *open_next_data_file(std::string base_dir, std::string dataset_name);

TFile *open_next_root_file(
    std::string base_dir, std::string dataset_name, std::string naming_scheme
);

} // namespace files
