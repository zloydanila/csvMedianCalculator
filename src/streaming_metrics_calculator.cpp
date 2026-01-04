#include "streaming_metrics_calculator.hpp"

#include <stdexcept>
#include <string>

streaming_metrics_calculator::streaming_metrics_calculator(std::vector<std::string> metrics)
    : metrics_(std::move(metrics)) {}

void streaming_metrics_calculator::add(double x) {
    median_.add(x);
    acc_.add(x);
}

std::size_t streaming_metrics_calculator::size() const {
    return acc_.size();
}

std::vector<std::optional<double>> streaming_metrics_calculator::values() const {
    std::vector<std::optional<double>> out;
    out.reserve(metrics_.size());

    for (const auto& m : metrics_) {
        if (m == "median") {
            out.push_back(median_.median());
            continue;
        }

        // Percentiles (P²) become stable/available on 5th sample (as in your tests).
        if ((m == "p50" || m == "p90" || m == "p95" || m == "p99") && size() < 5) {
            out.push_back(std::nullopt);
            continue;
        }

        if (m == "mean") {
            out.push_back(acc_.mean());
        } else if (m == "stddev") {
            out.push_back(acc_.stddev());
        } else if (m == "p50") {
            out.push_back(acc_.p50());
        } else if (m == "p90") {
            out.push_back(acc_.p90());
        } else if (m == "p95") {
            out.push_back(acc_.p95());
        } else if (m == "p99") {
            out.push_back(acc_.p99());
        } else {
            // Unknown metric -> empty column (more robust than crash)
            out.push_back(std::nullopt);
        }
    }

    return out;
}
