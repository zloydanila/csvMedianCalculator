#include "file_discovery.hpp"

#include <algorithm>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

static std::optional<csv_format> format_from_masks(
    const std::string& filename,
    const std::vector<std::string>& masks) {
    // Формат строго по masks (первое совпадение).
    for (const auto& m : masks) {
        if (m.empty()) {
            continue;
        }
        if (filename.find(m) == std::string::npos) {
            continue;
        }

        if (m == "level") {
            return csv_format::level;
        }
        if (m == "trade") {
            return csv_format::trade;
        }

        // кастомная маска -> trade (минимальные обязательные поля)
        return csv_format::trade;
    }

    return std::nullopt;
}

std::vector<discovered_file> discover_csv_files(
    const std::filesystem::path& input_dir,
    const std::vector<std::string>& masks) {
    if (!std::filesystem::exists(input_dir)) {
        throw std::runtime_error("input dir does not exist: " + input_dir.string());
    }
    if (!std::filesystem::is_directory(input_dir)) {
        throw std::runtime_error("input path is not a directory: " + input_dir.string());
    }

    std::vector<discovered_file> out;

    for (const auto& it : std::filesystem::directory_iterator(input_dir)) {
        if (!it.is_regular_file()) {
            continue;
        }

        const auto p = it.path();
        if (p.extension() != ".csv") {
            continue;
        }

        const auto name = p.filename().string();
        const auto fmt = format_from_masks(name, masks);
        if (!fmt.has_value()) {
            continue;
        }

        out.push_back(discovered_file{ .path = p, .format = *fmt });
    }

    std::sort(out.begin(), out.end(), [](const discovered_file& a, const discovered_file& b) {
        return a.path < b.path;
    });

    return out;
}
