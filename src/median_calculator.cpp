#include "median_calculator.hpp"

#include <algorithm>
#include <cstddef>
#include <map>
#include <vector>

void median_calculator::add(std::int64_t receivets, double price) {
    by_ts_[receivets].push_back(price);
}

std::map<std::int64_t, double> median_calculator::compute_medians() const {
    std::map<std::int64_t, double> out;

    for (const auto& [ts, prices] : by_ts_) {
        if (prices.empty()) {
            continue;
        }

        std::vector<double> tmp = prices;
        const std::size_t n = tmp.size();
        const std::size_t mid = n / 2;

        std::nth_element(tmp.begin(), tmp.begin() + static_cast<std::ptrdiff_t>(mid), tmp.end());
        const double hi = tmp[mid];

        if (n % 2 == 1) {
            out[ts] = hi;
        } else {
            std::nth_element(tmp.begin(), tmp.begin() + static_cast<std::ptrdiff_t>(mid - 1), tmp.begin() + static_cast<std::ptrdiff_t>(mid));
            const double lo = tmp[mid - 1];
            out[ts] = (lo + hi) / 2.0;
        }
    }

    return out;
}

void median_calculator::clear() {
    by_ts_.clear();
}
