#pragma once

#include <cstdint>
#include <map>
#include <vector>

class median_calculator {
public:
    void add(std::int64_t receivets, double price);

    std::map<std::int64_t, double> compute_medians() const;

    void clear();

private:
    std::map<std::int64_t, std::vector<double>> by_ts_;
};
