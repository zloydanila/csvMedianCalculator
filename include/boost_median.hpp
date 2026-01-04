#pragma once

#include <cstddef>

class boost_streaming_median {
public:
    boost_streaming_median();

    void add(double x);
    double median() const;
    std::size_t count() const;

private:
    double median_ = 0.0;
    std::size_t count_ = 0;

    struct heaps;
    heaps* heaps_;
};
