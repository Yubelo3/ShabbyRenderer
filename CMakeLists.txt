cmake_minimum_required(VERSION 3.9)

find_package(Eigen3 REQUIRED)
include_directories(EIGEN3_INCLUDE_DIR)

project(ShabbyRenderer)

set( CMAKE_CXX_FLAGS "-std=c++17" )

set(CMAKE_BUILD_TYPE Release)

add_executable(ShabbyRenderer src/main.cpp)