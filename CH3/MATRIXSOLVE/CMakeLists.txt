cmake_minimum_required(VERSION 3.10)

project(matrixsolve)

set(CMAKE_CXX_STANDARD 20)


find_package(Eigen3 REQUIRED)

add_executable(matrixsolve src/matrixsolve.cpp)
target_link_libraries(matrixsolve PRIVATE Eigen3::Eigen)
