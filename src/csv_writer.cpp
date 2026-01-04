#include "csv_writer.hpp"

#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <string>

void write_median_csv(
    const std::filesystem::path& out_file,
    const std::map<std::int64_t, double>& medians) {
    // writes result csv with receivets,pricemedian.
    std::ofstream out(out_file.string());
    if (!out.is_open()) {
        throw std::runtime_error("failed to open output file: " + out_file.string());
    }

    out << "receivets,pricemedian\n";
    out << std::setprecision(10);

    for (const auto& [ts, med] : medians) {
        out << ts << "," << med << "\n";
    }
}
