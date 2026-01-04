#pragma once

#include "app_config.hpp"

#include <filesystem>

app_config parse_config_file(const std::filesystem::path& path);
app_config parse_config_from_args(int argc, char** argv);
