cmake_minimum_required(VERSION 3.16)

project(skeleton LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SoQt CONFIG REQUIRED)

file(GLOB SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(skeleton ${SOURCES})
target_link_libraries(skeleton PRIVATE SoQt::SoQt)

