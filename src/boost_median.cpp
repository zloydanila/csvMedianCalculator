#include "boost_median.hpp"

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/count.hpp>

#include <queue>
#include <stdexcept>
#include <vector>

using namespace boost::accumulators;

struct boost_streaming_median::heaps {
    // max-heap for lower half
    std::priority_queue<double> lo;
    // min-heap for upper half
    std::priority_queue<double, std::vector<double>, std::greater<>> hi;
};

boost_streaming_median::boost_streaming_median()
    : heaps_(new heaps{}) {}

void boost_streaming_median::add(double x) {
    // exact incremental median with two heaps
    if (heaps_->lo.empty() || x <= heaps_->lo.top()) {
        heaps_->lo.push(x);
    } else {
        heaps_->hi.push(x);
    }

    if (heaps_->lo.size() > heaps_->hi.size() + 1) {
        heaps_->hi.push(heaps_->lo.top());
        heaps_->lo.pop();
    } else if (heaps_->hi.size() > heaps_->lo.size()) {
        heaps_->lo.push(heaps_->hi.top());
        heaps_->hi.pop();
    }

    ++count_;

    if (heaps_->lo.size() == heaps_->hi.size()) {
        median_ = (heaps_->lo.top() + heaps_->hi.top()) / 2.0;
    } else {
        median_ = heaps_->lo.top();
    }
}

double boost_streaming_median::median() const {
    if (count_ == 0) {
        throw std::runtime_error("median() called with empty sample");
    }
    return median_;
}

std::size_t boost_streaming_median::count() const {
    return count_;
}
