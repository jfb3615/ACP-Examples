cmake_minimum_required(VERSION 3.10)
project(eigenEx2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)

# Find Eigen3 (assumes it is installed on the system)
find_package(Eigen3 REQUIRED)

add_executable(eigenEx2 eigenEx2.cpp)

target_link_libraries(eigenEx2 Eigen3::Eigen)
