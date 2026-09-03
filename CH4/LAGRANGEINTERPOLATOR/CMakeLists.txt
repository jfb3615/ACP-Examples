cmake_minimum_required(VERSION 3.16)

project(lagrangeInterpolator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 COMPONENTS Core Widgets REQUIRED)


file(GLOB SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")


add_executable(lagrangeInterpolator ${SOURCES})
target_link_libraries(lagrangeInterpolator PRIVATE Qt6::Widgets Qt6::Core)

