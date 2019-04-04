cmake_minimum_required (VERSION 3.2.2)
project (wfc-sebastian)

set(CMAKE_CXX_FLAGS "-std=c++17 -Wall -O2 -g -DNDEBUG -w ${CMAKE_CXX_FLAGS}")
#set(CMAKE_CXX_FLAGS "-Wno-sign-compare ${CMAKE_CXX_FLAGS}")

include_directories(libs libs/emilib include)

add_library(libs libs/libs.cpp)

add_library(wfc 
	src/app.cpp
	src/algorithm.cpp
	src/configuru.cpp
	src/tile_model.cpp
	src/algorithm_data.cpp
	src/overlapping_model.cpp
	src/pattern_properties_comparison.cpp
	src/overlapping_pattern_extraction.cpp)

add_executable(wfc-sebastian src/main.cpp)

target_link_libraries(wfc-sebastian libs wfc pthread ${CMAKE_DL_LIBS})

add_subdirectory(test)

