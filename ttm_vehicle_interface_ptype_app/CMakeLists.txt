cmake_minimum_required(VERSION 3.0.0)

project(TTM_Vehicle_Interface VERSION 1.0.0)

add_definitions(-std=c++17)

add_subdirectory(modules/logging)

add_executable(client
src/main.cc 
src/ttm_client_tcp.cc)

target_include_directories(client PRIVATE include)

target_link_libraries(client PRIVATE logging)
