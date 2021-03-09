cmake_minimum_required(VERSION 3.0.0)

project(logging VERSION 1.0.0)

add_definitions(-std=c++17)

add_library(logging
src/log.cc
src/standard_output_sink.cc)

target_include_directories(logging PUBLIC include)

find_package(g3log CONFIG REQUIRED)

target_link_libraries(logging PRIVATE g3log)
