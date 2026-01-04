#pragma once

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>

#include <cstddef>

class metrics_accumulator {
public:
    void add(double x);

    void clear();

    std::size_t size() const;

    double mean() const;

    double stddev() const;

    double p50() const;
    double p90() const;
    double p95() const;
    double p99() const;

private:
    using acc_t = boost::accumulators::accumulator_set<
        double,
        boost::accumulators::features<
            boost::accumulators::tag::mean,
            boost::accumulators::tag::variance,
            boost::accumulators::tag::p_square_quantile
        >
    >;

    acc_t acc_;
    std::size_t n_ = 0;
};
