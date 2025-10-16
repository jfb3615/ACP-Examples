cmake_minimum_required(VERSION 3.10)
project(RATIONAL VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Collect all .cpp sources in this directory
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
if (NOT PROJECT_SOURCES)
  message(FATAL_ERROR "No .cpp sources found in ${CMAKE_CURRENT_SOURCE_DIR}")
endif()

add_executable(rational_client ${PROJECT_SOURCES})

target_include_directories(rational_client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if (MSVC)
  target_compile_options(rational_client PRIVATE /W4)
else()
  target_compile_options(rational_client PRIVATE -Wall -Wextra -Wpedantic)
endif()

set_target_properties(rational_client PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)