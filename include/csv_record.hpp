#pragma once

#include <cstdint>
#include <optional>

enum class market_side {
    bid,
    ask
};

enum class csv_format {
    trade,
    level
};

struct csv_record {
    std::int64_t receive_ts = 0;
    std::int64_t exchange_ts = 0;
    double price = 0.0;
    double quantity = 0.0;
    std::optional<market_side> side;
    std::optional<bool> rebuild;
};
