cmake_minimum_required(VERSION 3.16)
project(molmodel VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Release)

file (GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
list (REMOVE_ITEM SOURCES  "${CMAKE_CURRENT_SOURCE_DIR}/src/noblegas.cpp")
list (REMOVE_ITEM SOURCES  "${CMAKE_CURRENT_SOURCE_DIR}/src/molmodel.cpp")


set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_BUILD_RPATH "${QatPlotWidgets_DIR}/../..")


find_package(Eigen3 REQUIRED)
find_package(SoQt REQUIRED)
find_package(Qat REQUIRED)
find_package(Qt6 COMPONENTS Core Widgets Gui PrintSupport Svg OpenGL REQUIRED)


add_library   (MM ${SOURCES})


add_executable(molmodel "${CMAKE_CURRENT_SOURCE_DIR}/src/molmodel.cpp" )
add_executable(noblegas "${CMAKE_CURRENT_SOURCE_DIR}/src/noblegas.cpp" )

target_link_libraries(MM PRIVATE Qt6::Core Qt6::Widgets Qt6::Gui Qt6::PrintSupport Qt6::Svg Eigen3::Eigen QatPlotWidgets QatPlotting
QatDataAnalysis QatGenericFunctions )


target_link_libraries(molmodel PRIVATE -L. -lMM SoQt::SoQt Qt6::Core
Qt6::Widgets Qt6::Gui Qt6::PrintSupport Qt6::Svg Qt6::OpenGL
Eigen3::Eigen QatPlotWidgets QatPlotting QatDataAnalysis
QatGenericFunctions )

target_link_libraries(noblegas PRIVATE -L. -lMM
Qt6::Core Qt6::Widgets Qt6::Gui Qt6::PrintSupport Qt6::Svg
Eigen3::Eigen  QatPlotWidgets QatPlotting QatDataAnalysis
QatGenericFunctions )


