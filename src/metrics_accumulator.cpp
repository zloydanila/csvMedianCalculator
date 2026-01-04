#include "metrics_accumulator.hpp"

#include <boost/accumulators/statistics/p_square_quantile.hpp>
#include <boost/accumulators/statistics/variance.hpp>

#include <cmath>
#include <stdexcept>

void metrics_accumulator::add(double x) {
    acc_(x);
    ++n_;
}

void metrics_accumulator::clear() {
    acc_ = acc_t{};
    n_ = 0;
}

std::size_t metrics_accumulator::size() const {
    return n_;
}

double metrics_accumulator::mean() const {
    if (n_ == 0) {
        throw std::runtime_error("mean of empty sequence");
    }
    return boost::accumulators::mean(acc_);
}

double metrics_accumulator::stddev() const {
    if (n_ == 0) {
        throw std::runtime_error("stddev of empty sequence");
    }
    const double var = boost::accumulators::variance(acc_);
    return std::sqrt(var);
}

double metrics_accumulator::p50() const {
    if (n_ == 0) {
        throw std::runtime_error("p50 of empty sequence");
    }
    return boost::accumulators::p_square_quantile(acc_, boost::accumulators::quantile_probability = 0.50);
}

double metrics_accumulator::p90() const {
    if (n_ == 0) {
        throw std::runtime_error("p90 of empty sequence");
    }
    return boost::accumulators::p_square_quantile(acc_, boost::accumulators::quantile_probability = 0.90);
}

double metrics_accumulator::p95() const {
    if (n_ == 0) {
        throw std::runtime_error("p95 of empty sequence");
    }
    return boost::accumulators::p_square_quantile(acc_, boost::accumulators::quantile_probability = 0.95);
}

double metrics_accumulator::p99() const {
    if (n_ == 0) {
        throw std::runtime_error("p99 of empty sequence");
    }
    return boost::accumulators::p_square_quantile(acc_, boost::accumulators::quantile_probability = 0.99);
}
