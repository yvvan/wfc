cmake_minimum_required (VERSION 3.5)
project (wfc-sebastian)


set(CMAKE_CXX_FLAGS "-std=c++14 -Wall -O2 -g -DNDEBUG ${CMAKE_CXX_FLAGS}")


include_directories(libs libs/emilib)

add_library(libs libs.cpp)

add_executable(wfc-sebastian main.cpp)

target_link_libraries(wfc-sebastian libs pthread ${CMAKE_DL_LIBS})
