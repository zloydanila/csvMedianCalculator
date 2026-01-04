#pragma once

#include "csv_reader.hpp"

#include <filesystem>
#include <string>
#include <vector>

struct discovered_file {
    std::filesystem::path path;
    csv_format format = csv_format::trade;
};

std::vector<discovered_file> discover_csv_files(
    const std::filesystem::path& input_dir,
    const std::vector<std::string>& masks);
