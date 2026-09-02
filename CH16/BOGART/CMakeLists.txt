cmake_minimum_required(VERSION 3.16)
set (target bogart)
project(${target} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)


find_package(QatPlotWidgets      REQUIRED)
find_package(SoQt                REQUIRED)

set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_BUILD_RPATH "${QatPlotWidgets_DIR}/../..")

file(GLOB SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(${target} ${SOURCES})
target_include_directories (${target} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(${target} PRIVATE QatPlotWidgets QatPlotting
				QatDataAnalysis QatGenericFunctions
				Qt6::Widgets Qt6::Core SoQt::SoQt)

