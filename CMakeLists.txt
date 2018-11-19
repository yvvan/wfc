cmake_minimum_required (VERSION 3.5)
project (wfc-sebastian)


set(CMAKE_CXX_FLAGS "-std=c++14 -Wall ${CMAKE_CXX_FLAGS}")

include_directories(libs libs/emilib)

add_executable(wfc-sebastian main.cpp)
