cmake_minimum_required (VERSION 3.2.2)
project (wfc)

include_directories(libs include)

add_library(libs libs/libs.cpp)

add_library(wfc-lib
		src/app.cpp
		src/algorithm.cpp
		src/configuru.cpp
		src/loguru.cpp
		src/tile_model.cpp
		src/algorithm_data.cpp
		src/overlapping_model.cpp
		src/pattern_properties_comparison.cpp
		src/overlapping_pattern_extraction.cpp)

add_executable(wfc src/main.cpp)

target_link_libraries(wfc libs wfc-lib)

add_subdirectory(test)

