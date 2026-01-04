#include "config_parser.hpp"

#include <toml++/toml.hpp>

#include <boost/program_options.hpp>

#include <filesystem>
#include <stdexcept>
#include <string>
#include <vector>

namespace po = boost::program_options;

static std::filesystem::path default_config_path_near_exe(const std::filesystem::path& argv0) {
    std::error_code ec;
    auto abs = std::filesystem::absolute(argv0, ec);
    if (ec) {
        abs = argv0;
    }
    return abs.parent_path() / "config.toml";
}

static std::vector<std::string> read_string_array(const toml::node_view<const toml::node>& node) {
    std::vector<std::string> out;
    const auto* arr = node.as_array();
    if (!arr) {
        return out;
    }
    for (const auto& it : *arr) {
        auto v = it.value<std::string>();
        if (!v) {
            throw std::runtime_error("config: filename_mask must contain only strings");
        }
        out.push_back(*v);
    }
    return out;
}

app_config parse_config_file(const std::filesystem::path& path) {
    toml::table t;
    try {
        t = toml::parse_file(path.string());
    } catch (const toml::parse_error& e) {
        throw std::runtime_error(std::string("config parse error: ") + e.what());
    }

    const auto* main_tbl = t["main"].as_table();
    if (!main_tbl) {
        throw std::runtime_error("config: missing [main] table");
    }

    app_config cfg;

    if (auto v = (*main_tbl)["input"].value<std::string>()) {
        cfg.input_dir = *v;
    } else {
        throw std::runtime_error("config: missing or invalid main.input (string)");
    }

    if (auto v = (*main_tbl)["output"].value<std::string>()) {
        cfg.output_dir = *v;
    } else {
        cfg.output_dir = std::filesystem::current_path() / "output";
    }

    cfg.filename_mask = read_string_array((*main_tbl)["filename_mask"]);
    return cfg;
}

app_config parse_config_from_args(int argc, char** argv) {
    po::options_description desc("Options");
    desc.add_options()
        ("config", po::value<std::string>(), "Path to config.toml")
        ("cfg", po::value<std::string>(), "Path to config.toml");

    po::variables_map vm;

 
    auto parsed = po::command_line_parser(argc, argv)
        .options(desc)
        .style(po::command_line_style::unix_style | po::command_line_style::allow_long_disguise)
        .run();

    po::store(parsed, vm);
    po::notify(vm);

    std::filesystem::path cfg_path;
    if (vm.count("config")) {
        cfg_path = vm["config"].as<std::string>();
    } else if (vm.count("cfg")) {
        cfg_path = vm["cfg"].as<std::string>();
    } else {
        cfg_path = default_config_path_near_exe(argv[0]);
    }

    return parse_config_file(cfg_path);
}
