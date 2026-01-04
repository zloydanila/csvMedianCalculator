#include "csv_reader.hpp"

#include <charconv>
#include <cctype>
#include <stdexcept>

csv_reader::csv_reader(const std::filesystem::path& path, csv_format fmt, bool strict_csv)
    : path_(path), fmt_(fmt), strict_(strict_csv), in_(path.string(), std::ios::binary) {
    if (!in_.is_open()) {
        throw std::runtime_error("failed to open csv file: " + path.string());
    }
}

std::optional<csv_record> csv_reader::next() {
    if (!header_read_) {
        read_header_or_throw();
        header_read_ = true;
    }

    std::string line;
    while (std::getline(in_, line)) {
        ++line_no_;

        line = trim(std::move(line));
        if (line.empty()) {
            if (strict_) {
                fail("empty line");
            }
            continue;
        }

        const auto row = split_line(line);

        if (strict_ && row.size() != header_.size()) {
            fail("wrong column count: expected " + std::to_string(header_.size()) + ", got " + std::to_string(row.size()));
        }

        csv_record rec;

        try {
            // TЗ names: receive_ts, exchange_ts [file:107]
            // Backward compat: receivets, exchangets
            const auto receive_s = get_any(row, {"receive_ts", "receivets"});
            const auto exchange_s = get_any(row, {"exchange_ts", "exchangets"});
            const auto price_s = get_any(row, {"price"});
            const auto quantity_s = get_any(row, {"quantity"});
            const auto side_s = get_any(row, {"side"});
            const auto rebuild_s = get_any(row, {"rebuild"});

            if (!receive_s || receive_s->empty()) fail("missing required field: receive_ts");
            if (!price_s || price_s->empty()) fail("missing required field: price");

            rec.receive_ts = parse_i64(*receive_s, strict_);
            rec.price = parse_double(*price_s, strict_);

            if (exchange_s && !exchange_s->empty()) rec.exchange_ts = parse_i64(*exchange_s, strict_);
            if (quantity_s && !quantity_s->empty()) rec.quantity = parse_double(*quantity_s, strict_);
            if (side_s && !side_s->empty()) rec.side = parse_side(*side_s, strict_);

            if (rebuild_s && !rebuild_s->empty()) rec.rebuild = parse_bool01(*rebuild_s, strict_);

            if (strict_) {
                if (fmt_ == csv_format::trade) {
                    // TЗ trade requires exchange_ts, quantity, side [file:107]
                    if (!exchange_s) fail("missing required field: exchange_ts");
                    if (!quantity_s) fail("missing required field: quantity");
                    if (!side_s) fail("missing required field: side");
                    // rebuild not in trade spec
                    if (col_.contains("rebuild")) fail("unexpected column 'rebuild' for trade csv");
                } else {
                    // TЗ level requires exchange_ts, quantity, side, rebuild [file:107]
                    if (!exchange_s) fail("missing required field: exchange_ts");
                    if (!quantity_s) fail("missing required field: quantity");
                    if (!side_s) fail("missing required field: side");
                    if (!rebuild_s) fail("missing required field: rebuild");
                }
            }

        } catch (const std::exception& e) {
            fail(e.what());
        }

        return rec;
    }

    return std::nullopt;
}

void csv_reader::read_header_or_throw() {
    std::string line;
    if (!std::getline(in_, line)) {
        throw std::runtime_error("empty csv file: " + path_.string());
    }
    ++line_no_;

    line = trim(std::move(line));
    if (line.empty()) {
        throw std::runtime_error("empty header line: " + path_.string());
    }

    // Detect delimiter: TЗ is ';' [file:107], but accept ',' for old tests/data.
    delim_ = (line.find(';') != std::string::npos) ? ';' : ',';

    header_ = split_line(line);
    col_.clear();

    for (std::size_t i = 0; i < header_.size(); ++i) {
        const auto key = trim(header_[i]);
        if (key.empty()) {
            throw std::runtime_error("empty header column name: " + path_.string());
        }
        col_[key] = i;
    }

    if (strict_) {
        validate_header_or_throw();
    }
}

void csv_reader::validate_header_or_throw() const {
    auto has_any = [&](std::initializer_list<const char*> names) {
        for (auto n : names) {
            if (col_.contains(n)) return true;
        }
        return false;
    };
    auto require_any = [&](std::initializer_list<const char*> names, const char* shown) {
        if (!has_any(names)) {
            throw std::runtime_error("missing required column '" + std::string(shown) + "' in: " + path_.string());
        }
    };

    require_any({"receive_ts", "receivets"}, "receive_ts");
    require_any({"exchange_ts", "exchangets"}, "exchange_ts");
    require_any({"price"}, "price");
    require_any({"quantity"}, "quantity");
    require_any({"side"}, "side");

    if (fmt_ == csv_format::level) {
        require_any({"rebuild"}, "rebuild");
    }
}

std::vector<std::string> csv_reader::split_line(const std::string& line) const {
    std::vector<std::string> out;
    std::string cur;
    cur.reserve(line.size());

    for (char c : line) {
        if (c == delim_) {
            out.push_back(trim(std::move(cur)));
            cur.clear();
        } else {
            cur.push_back(c);
        }
    }
    out.push_back(trim(std::move(cur)));
    return out;
}

std::string csv_reader::trim(std::string s) {
    auto is_space = [](unsigned char ch) { return std::isspace(ch) != 0; };
    while (!s.empty() && is_space(static_cast<unsigned char>(s.front()))) s.erase(s.begin());
    while (!s.empty() && is_space(static_cast<unsigned char>(s.back()))) s.pop_back();
    return s;
}

std::optional<std::string> csv_reader::get_field(const std::vector<std::string>& row, const std::string& name) const {
    const auto it = col_.find(name);
    if (it == col_.end()) return std::nullopt;
    const std::size_t idx = it->second;
    if (idx >= row.size()) return std::nullopt;
    return row[idx];
}

std::optional<std::string> csv_reader::get_any(
    const std::vector<std::string>& row,
    const std::initializer_list<const char*>& names) const {
    for (auto n : names) {
        if (auto v = get_field(row, n)) return v;
    }
    return std::nullopt;
}

[[noreturn]] void csv_reader::fail(const std::string& msg) const {
    throw std::runtime_error(path_.string() + ":" + std::to_string(line_no_) + ": " + msg);
}

std::optional<market_side> csv_reader::parse_side(const std::string& s, bool strict) {
    if (s == "bid") return market_side::bid;
    if (s == "ask") return market_side::ask;
    if (strict) throw std::runtime_error("invalid side: '" + s + "'");
    return std::nullopt;
}

std::optional<bool> csv_reader::parse_bool01(const std::string& s, bool strict) {
    if (s == "0") return false;
    if (s == "1") return true;
    if (strict) throw std::runtime_error("invalid bool01 (expected 0/1): '" + s + "'");
    return std::nullopt;
}

std::int64_t csv_reader::parse_i64(const std::string& s, bool strict) {
    std::int64_t v = 0;
    const char* b = s.data();
    const char* e = s.data() + s.size();
    auto [ptr, ec] = std::from_chars(b, e, v);
    if (ec != std::errc() || ptr != e) {
        if (strict) throw std::runtime_error("invalid int64: '" + s + "'");
        return 0;
    }
    return v;
}

double csv_reader::parse_double(const std::string& s, bool strict) {
    try {
        std::size_t pos = 0;
        const double v = std::stod(s, &pos);
        if (pos != s.size()) {
            if (strict) throw std::runtime_error("invalid double: '" + s + "'");
            return 0.0;
        }
        return v;
    } catch (...) {
        if (strict) throw std::runtime_error("invalid double: '" + s + "'");
        return 0.0;
    }
}
