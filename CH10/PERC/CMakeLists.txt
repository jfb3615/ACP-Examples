cmake_minimum_required(VERSION 3.16)

project(perc LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SoQt CONFIG REQUIRED)

file(GLOB SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(perc ${SOURCES})
target_link_libraries(perc PRIVATE SoQt::SoQt -L/opt/homebrew/lib -lQatDataAnalysis)

