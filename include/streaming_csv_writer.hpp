#pragma once

#include <cstdint>
#include <filesystem>
#include <fstream>

class streaming_csv_writer {
public:
    explicit streaming_csv_writer(const std::filesystem::path& out_file);

    void write_header();

    void write_row(std::int64_t receivets, double pricemedian);

private:
    std::ofstream out_;
};
