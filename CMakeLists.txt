cmake_minimum_required(VERSION 3.23)
project(csv_median_calculator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
include(CTest)

FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG v1.14.1
)
FetchContent_MakeAvailable(spdlog)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.5.4
)
FetchContent_MakeAvailable(Catch2)

FetchContent_Declare(
  tomlplusplus
  GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
  GIT_TAG v3.4.0
)
FetchContent_MakeAvailable(tomlplusplus)

find_package(Boost REQUIRED COMPONENTS program_options)

add_library(csvmediancalculator_lib
  src/config_parser.cpp
  src/csv_reader.cpp
  src/csv_writer.cpp
  src/file_discovery.cpp
  src/boost_median.cpp
)

target_include_directories(csvmediancalculator_lib
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(csvmediancalculator_lib PUBLIC cxx_std_23)

# IMPORTANT: PUBLIC so consumers get include dirs (spdlog headers) too.
target_link_libraries(csvmediancalculator_lib
  PUBLIC
    spdlog::spdlog
    tomlplusplus::tomlplusplus
    Boost::program_options
)

add_executable(csv_median_calculator src/main.cpp)
target_link_libraries(csv_median_calculator PRIVATE csvmediancalculator_lib)

add_executable(csv_median_calculator_tests tests/main_test.cpp)
target_link_libraries(csv_median_calculator_tests PRIVATE csvmediancalculator_lib Catch2::Catch2WithMain)

include(Catch)
catch_discover_tests(csv_median_calculator_tests)
