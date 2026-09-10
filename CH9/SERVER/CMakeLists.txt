cmake_minimum_required(VERSION 3.16)
set (target simpleServer)
project(${target} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


find_package(Qat      REQUIRED)

set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_BUILD_RPATH "${QatDataAnalysis_DIR}/../..")

file(GLOB SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(${target} ${SOURCES})
target_link_libraries(${target} PRIVATE Qat::QatDataAnalysis Qat::QatGenericFunctions)

