cmake_minimum_required(VERSION 3.10)
project(vectEx)

set(CMAKE_CXX_STANDARD 20)

# Find Eigen3 (assumes it is installed on the system)
find_package(Eigen3 REQUIRED)

add_executable(vectEx vectEx.cpp)

target_link_libraries(vectEx Eigen3::Eigen)
