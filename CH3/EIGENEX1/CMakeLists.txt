cmake_minimum_required(VERSION 3.10)
project(eigenEx1)

set(CMAKE_CXX_STANDARD 20)

# Find Eigen3 (assumes it is installed on the system)
find_package(Eigen3 REQUIRED)

add_executable(eigenEx1 eigenEx1.cpp)

target_link_libraries(eigenEx1 Eigen3::Eigen)
