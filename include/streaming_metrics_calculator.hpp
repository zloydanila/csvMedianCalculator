#pragma once

#include "metrics_accumulator.hpp"
#include "streaming_median.hpp"

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

class streaming_metrics_calculator {
public:
    explicit streaming_metrics_calculator(std::vector<std::string> metrics);

    void add(double x);

    std::size_t size() const;

    std::vector<std::optional<double>> values() const;

private:
    std::vector<std::string> metrics_;
    streaming_median median_;
    metrics_accumulator acc_; // for percentiles/mean/stddev etc.
};
