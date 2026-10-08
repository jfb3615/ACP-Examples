cmake_minimum_required(VERSION 3.16)

project(Splash LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_BUILD_TYPE Release)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(splash splash.cpp render.cpp)
target_link_libraries(splash PRIVATE Qt6::Widgets)
