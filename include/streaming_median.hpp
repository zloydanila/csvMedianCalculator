#pragma once

#include <cstddef>
#include <vector>

class streaming_median {
public:
    void add(double x);

    double median() const;

    void clear();

    std::size_t size() const;

private:
    std::vector<double> values_;
};
