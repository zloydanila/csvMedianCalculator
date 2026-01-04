#include "streaming_csv_writer.hpp"

#include <iomanip>
#include <stdexcept>
#include <string>

streaming_csv_writer::streaming_csv_writer(const std::filesystem::path& out_file)
    : out_(out_file.string()) {
    if (!out_.is_open()) {
        throw std::runtime_error("failed to open output file: " + out_file.string());
    }
    out_ << std::setprecision(10);
}

void streaming_csv_writer::write_header() {
    out_ << "receivets,pricemedian\n";
}

void streaming_csv_writer::write_row(std::int64_t receivets, double pricemedian) {
    out_ << receivets << "," << pricemedian << "\n";
}
