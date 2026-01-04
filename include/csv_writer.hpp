#pragma once

#include <cstdint>
#include <filesystem>
#include <map>

void write_median_csv(
    const std::filesystem::path& out_file,
    const std::map<std::int64_t, double>& medians);
