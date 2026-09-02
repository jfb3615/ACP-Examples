cmake_minimum_required(VERSION 3.16)
project(simpleIntegrator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


find_package(QatGenericFunctions      REQUIRED)

set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_BUILD_RPATH "${QatPlotWidgets_DIR}/../..")

file(GLOB SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(simpleIntegrator ${SOURCES})
target_link_libraries(simpleIntegrator PRIVATE Qat::QatGenericFunctions )

