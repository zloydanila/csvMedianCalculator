#pragma once

#include "csv_record.hpp"

#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

class csv_reader {
public:
    explicit csv_reader(const std::filesystem::path& path, csv_format fmt, bool strict_csv = true);

    std::optional<csv_record> next();

private:
    std::filesystem::path path_;
    csv_format fmt_;
    bool strict_ = true;

    std::vector<std::string> header_;
    std::unordered_map<std::string, std::size_t> col_;
    bool header_read_ = false;

    std::size_t line_no_ = 0;
    std::ifstream in_;

    char delim_ = ';'; // per TЗ [file:107]

    void read_header_or_throw();
    void validate_header_or_throw() const;

    static std::string trim(std::string s);
    std::vector<std::string> split_line(const std::string& line) const;

    std::optional<std::string> get_field(const std::vector<std::string>& row, const std::string& name) const;
    std::optional<std::string> get_any(const std::vector<std::string>& row, const std::initializer_list<const char*>& names) const;

    [[noreturn]] void fail(const std::string& msg) const;

    static std::optional<market_side> parse_side(const std::string& s, bool strict);
    static std::optional<bool> parse_bool01(const std::string& s, bool strict);

    static std::int64_t parse_i64(const std::string& s, bool strict);
    static double parse_double(const std::string& s, bool strict);
};
