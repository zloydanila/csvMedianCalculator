#include "boost_median.hpp"
#include "config_parser.hpp"
#include "csv_reader.hpp"
#include "file_discovery.hpp"

#include <spdlog/spdlog.h>

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <optional>
#include <vector>

struct event {
    std::int64_t receive_ts;
    double price;
};

int main(int argc, char** argv) {
    try {
        spdlog::info("Starting csv_median_calculator");

        const auto cfg = parse_config_from_args(argc, argv);

        std::filesystem::create_directories(cfg.output_dir);

        const auto files = discover_csv_files(cfg.input_dir, cfg.filename_mask);
        spdlog::info("Found {} csv file(s)", files.size());

        std::vector<event> events;
        events.reserve(1024);

        for (const auto& f : files) {
            csv_reader r(f.path, f.format, true);
            while (const auto rec = r.next()) {
                events.push_back(event{rec->receive_ts, rec->price});
            }
        }

        std::sort(events.begin(), events.end(), [](const event& a, const event& b) {
            return a.receive_ts < b.receive_ts;
        });

        const auto out_path = cfg.output_dir / "median_result.csv";
        std::ofstream out(out_path.string(), std::ios::binary);
        if (!out.is_open()) {
            throw std::runtime_error("failed to open output file: " + out_path.string());
        }

        out << "receive_ts;price_median\n";
        out << std::fixed << std::setprecision(8);

        boost_streaming_median med;
        std::optional<double> prev;

        std::size_t written = 0;
        for (const auto& e : events) {
            med.add(e.price);
            const double cur = med.median();

            if (!prev.has_value() || cur != *prev) {
                out << e.receive_ts << ";" << cur << "\n";
                prev = cur;
                ++written;
            }
        }

        spdlog::info("Processed {} event(s), wrote {} median change(s)", events.size(), written);
        spdlog::info("Saved: {}", out_path.string());

        return 0;
    } catch (const std::exception& e) {
        spdlog::error("Error: {}", e.what());
        return 1;
    }
}
