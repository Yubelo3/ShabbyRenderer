cmake_minimum_required(VERSION 3.9)

find_package(Eigen3 REQUIRED)

project(ShabbyRenderer)

set(CMAKE_CXX_FLAGS "-std=c++17 -fopenmp")
set(OpenMP_CXX_FLAGS)
set(CMAKE_BUILD_TYPE Debug)

add_executable(ShabbyRenderer src/main.cpp)