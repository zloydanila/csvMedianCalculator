#pragma once

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <vector>

class streaming_metrics_csv_writer {
public:
    streaming_metrics_csv_writer(const std::filesystem::path& out_file, std::vector<std::string> metrics);

    void write_header();

    void write_row(std::int64_t receivets, const std::vector<std::optional<double>>& values);

private:
    std::ofstream out_;
    std::vector<std::string> metrics_;
};
