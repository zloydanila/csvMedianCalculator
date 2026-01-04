#pragma once

#include <filesystem>
#include <string>
#include <vector>

struct app_config {
    std::filesystem::path input_dir;
    std::filesystem::path output_dir;
    std::vector<std::string> filename_mask;
};
