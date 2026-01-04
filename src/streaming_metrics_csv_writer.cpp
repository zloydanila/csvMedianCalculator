#include "streaming_metrics_csv_writer.hpp"

#include <iomanip>
#include <stdexcept>
#include <string>

streaming_metrics_csv_writer::streaming_metrics_csv_writer(const std::filesystem::path& out_file, std::vector<std::string> metrics)
    : out_(out_file.string()), metrics_(std::move(metrics)) {
    if (!out_.is_open()) {
        throw std::runtime_error("failed to open output file: " + out_file.string());
    }
    out_ << std::setprecision(10);
}

void streaming_metrics_csv_writer::write_header() {
    out_ << "receivets";
    for (const auto& m : metrics_) {
        out_ << "," << m;
    }
    out_ << "\n";
}

void streaming_metrics_csv_writer::write_row(std::int64_t receivets, const std::vector<std::optional<double>>& values) {
    out_ << receivets;
    for (const auto& v : values) {
        out_ << ",";
        if (v.has_value()) {
            out_ << *v;
        }
    }
    out_ << "\n";
}
