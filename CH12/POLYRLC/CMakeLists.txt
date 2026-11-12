cmake_minimum_required(VERSION 3.28)
project(polyrlc LANGUAGES CXX)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)


find_package(Qat                 REQUIRED)
find_package(Circuit             REQUIRED)

set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_BUILD_RPATH "${Qat_DIR}/../..")


add_executable(circuitView ${CMAKE_CURRENT_SOURCE_DIR}/src/circuitView.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/CircuitViewer.cpp )
target_link_libraries(circuitView PRIVATE Qat::QatPlotWidgets Qat::QatPlotting
  Qat::QatDataModeling Qat::QatDataAnalysis Qat::QatGenericFunctions
  Qt6::Widgets Qt6::Core CIRCUIT::Circuit)

#NOT YET
#add_executable(circuitSim ${CMAKE_CURRENT_SOURCE_DIR}/src/circuitSim.cpp )
#target_link_libraries(circuitSim PRIVATE Qat::QatPlotWidgets Qat::QatPlotting
#  Qat::QatDataModeling Qat::QatDataAnalysis Qat::QatGenericFunctions
#  Qt6::Widgets Qt6::Core CIRCUIT::Circuit)


