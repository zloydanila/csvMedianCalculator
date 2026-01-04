#include "csv_reader.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>

using Catch::Matchers::ContainsSubstring;

static std::string read_file(const std::filesystem::path& p) {
    std::ifstream in(p.string(), std::ios::binary);
    REQUIRE(in.is_open());
    return std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
}

static std::string normalize(std::string s) {
    std::string out;
    out.reserve(s.size());
    for (char c : s) {
        if (c != '\r') out.push_back(c);
    }
    while (!out.empty() && (out.back() == ' ' || out.back() == '\n' || out.back() == '\t')) out.pop_back();
    return out;
}

static std::filesystem::path build_dir() {
    namespace fs = std::filesystem;
    return std::getenv("CTEST_BINARY_DIRECTORY") ? fs::path(std::getenv("CTEST_BINARY_DIRECTORY")) : fs::current_path();
}

static int run_app(const std::filesystem::path& cfg_path) {
    namespace fs = std::filesystem;
    const fs::path exe = build_dir() / "csv_median_calculator";
    REQUIRE(fs::exists(exe));
    const std::string cmd = "\"" + exe.string() + "\" -config \"" + cfg_path.string() + "\"";
    return std::system(cmd.c_str());
}

TEST_CASE("integration: writes only median changes (TЗ, fixed-8)") {
    namespace fs = std::filesystem;

    const fs::path base = fs::temp_directory_path() / "csv_median_calculator_it";
    fs::remove_all(base);
    fs::create_directories(base / "input");
    fs::create_directories(base / "output");

    {
        std::ofstream f((base / "input" / "trade.csv").string(), std::ios::binary);
        REQUIRE(f.is_open());
        f << "receive_ts;exchange_ts;price;quantity;side\n";
        f << "1000;900;100.0;1.0;bid\n";
        f << "2000;1900;101.0;2.0;ask\n";
        f << "2000;1900;102.0;1.5;bid\n";
        f << "3000;2900;103.0;3.0;ask\n";
    }

    const fs::path cfg = base / "config.toml";
    {
        std::ofstream f(cfg.string(), std::ios::binary);
        REQUIRE(f.is_open());
        f << "[main]\n";
        f << "input = \"" << (base / "input").string() << "\"\n";
        f << "output = \"" << (base / "output").string() << "\"\n";
        f << "filename_mask = [\"trade\"]\n";
    }

    const int rc = run_app(cfg);
    REQUIRE(rc == 0);

    const fs::path out = base / "output" / "median_result.csv";
    REQUIRE(fs::exists(out));

    const std::string got = read_file(out);
    const std::string expected =
        "receive_ts;price_median\n"
        "1000;100.00000000\n"
        "2000;100.50000000\n"
        "2000;101.00000000\n"
        "3000;101.50000000\n";

    REQUIRE(normalize(got) == normalize(expected));
}

TEST_CASE("strict_csv rejects invalid side (TЗ error handling)") {
    namespace fs = std::filesystem;

    const fs::path base = fs::temp_directory_path() / "csv_median_calculator_bad_side";
    fs::remove_all(base);
    fs::create_directories(base / "input");
    fs::create_directories(base / "output");

    {
        std::ofstream f((base / "input" / "trade.csv").string(), std::ios::binary);
        REQUIRE(f.is_open());
        f << "receive_ts;exchange_ts;price;quantity;side\n";
        f << "1000;900;100.0;1.0;WTF\n";
    }

    const fs::path cfg = base / "config.toml";
    {
        std::ofstream f(cfg.string(), std::ios::binary);
        REQUIRE(f.is_open());
        f << "[main]\n";
        f << "input = \"" << (base / "input").string() << "\"\n";
        f << "output = \"" << (base / "output").string() << "\"\n";
        f << "filename_mask = [\"trade\"]\n";
    }

    const int rc = run_app(cfg);
    REQUIRE(rc != 0);
}

TEST_CASE("strict_csv rejects invalid rebuild (TЗ error handling)") {
    namespace fs = std::filesystem;

    const fs::path base = fs::temp_directory_path() / "csv_median_calculator_bad_rebuild";
    fs::remove_all(base);
    fs::create_directories(base / "input");
    fs::create_directories(base / "output");

    {
        std::ofstream f((base / "input" / "level.csv").string(), std::ios::binary);
        REQUIRE(f.is_open());
        f << "receive_ts;exchange_ts;price;quantity;side;rebuild\n";
        f << "1000;900;100.0;1.0;bid;2\n";
    }

    const fs::path cfg = base / "config.toml";
    {
        std::ofstream f(cfg.string(), std::ios::binary);
        REQUIRE(f.is_open());
        f << "[main]\n";
        f << "input = \"" << (base / "input").string() << "\"\n";
        f << "output = \"" << (base / "output").string() << "\"\n";
        f << "filename_mask = [\"level\"]\n";
    }

    const int rc = run_app(cfg);
    REQUIRE(rc != 0);
}

TEST_CASE("strict_csv error contains file and line (TЗ requirement)") {
    namespace fs = std::filesystem;

    const fs::path base = fs::temp_directory_path() / "csv_median_calculator_reader_err";
    fs::remove_all(base);
    fs::create_directories(base);

    const fs::path csv = base / "trade.csv";
    {
        std::ofstream f(csv.string(), std::ios::binary);
        REQUIRE(f.is_open());
        f << "receive_ts;exchange_ts;price;quantity;side\n";
        f << "1000;900;100.0;1.0;bid\n";
        f << "2000;1900;BAD;2.0;ask\n";
    }

    REQUIRE_THROWS_WITH(
        [&] {
            csv_reader r(csv, csv_format::trade, true);
            while (r.next()) {}
        }(),
        ContainsSubstring(csv.string() + ":3:")
    );
}
