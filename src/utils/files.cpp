#include "files.h"

#include <filesystem>
#include <iostream>
#include <string>

#include "TFile.h"

namespace fs = std::filesystem;

std::string files::relative_filename(std::string path) {
    // note that PROJECT_DIR is NOT defined anywhere in the code but passed to
    // the compiler by the build script! Look at xmake.lua
    fs::path final_path{PROJECT_DIR};

    final_path.append(path);

    return final_path.string();
}

// opens file relative to the project directory
TFile *files::open(std::string path) {
    std::string final_path = files::relative_filename(path);

    return TFile::Open(final_path.c_str(), "READ");
}

TFile *files::recreate(std::string path) {
    std::string final_path = files::relative_filename(path);

    return TFile::Open(final_path.c_str(), "RECREATE");
}

std::string next_filename(
    fs::path base, const std::string &name, const std::string &extension
) {
    int file_num = 0;

    for (const auto &entry : fs::directory_iterator(base)) {
        const fs::path path = entry.path();
        const std::string filename = path.filename().string();
        const std::string ext = path.extension().string();

        if ((filename.substr(0, name.length()).compare(name) == 0) &&
            (ext.compare(extension) == 0)) {
            file_num++;
        }
    }

    const size_t out_length = name.length() + extension.length() + 5;

    char *out_cstring;
    out_cstring = (char *)malloc(out_length * sizeof(char));

    std::snprintf( // I truly hate pre-C++20 string formatting
        out_cstring, out_length, "%s_%02d%s", name.c_str(), file_num,
        extension.c_str()
    );

    return std::string(out_cstring);
}

TFile *files::open_next_root_file(
    std::string dataset_name, std::string naming_scheme
) {
    fs::path final_path{PROJECT_DIR};

    final_path.append("data");
    final_path.append(dataset_name);
    final_path.append(
        next_filename(final_path, naming_scheme, std::string(".root"))
    );

    return TFile::Open(final_path.c_str(), "CREATE");
}

TFile *files::open_next_data_file(std::string dataset_name) {
    return open_next_root_file(dataset_name, "data");
}