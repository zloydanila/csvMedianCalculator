#include "streaming_median.hpp"

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <vector>

void streaming_median::add(double x) {
    values_.push_back(x);
}

double streaming_median::median() const {
    if (values_.empty()) {
        throw std::runtime_error("median of empty sequence");
    }

    std::vector<double> tmp = values_;
    const std::size_t n = tmp.size();
    const std::size_t mid = n / 2;

    std::nth_element(tmp.begin(), tmp.begin() + static_cast<std::ptrdiff_t>(mid), tmp.end());
    const double hi = tmp[mid];

    if (n % 2 == 1) {
        return hi;
    }

    std::nth_element(tmp.begin(), tmp.begin() + static_cast<std::ptrdiff_t>(mid - 1), tmp.begin() + static_cast<std::ptrdiff_t>(mid));
    const double lo = tmp[mid - 1];

    return (lo + hi) / 2.0;
}

void streaming_median::clear() {
    values_.clear();
}

std::size_t streaming_median::size() const {
    return values_.size();
}
