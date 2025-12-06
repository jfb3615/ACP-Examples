cmake_minimum_required(VERSION 3.16)
Project(molmodel VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_AUTOMOC ON)

file (GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
list (REMOVE_ITEM SOURCES  "${CMAKE_CURRENT_SOURCE_DIR}/src/runnoble.cpp")
list (REMOVE_ITEM SOURCES  "${CMAKE_CURRENT_SOURCE_DIR}/src/runmcmc.cpp")


execute_process (COMMAND which qat-project  OUTPUT_VARIABLE QPREFIX)

if (QPREFIX STREQUAL "")
  message(FATAL_ERROR "You appear to not have Qat installed. Exiting")
endif()

string(REPLACE "bin/qat-project" "lib" QLIB0 ${QPREFIX})
string(STRIP ${QLIB0} QLIB)
message (STATUS " The Qat libraries are found in QLIB=${QLIB}")

find_package(Eigen3 REQUIRED)
find_package(SoQt REQUIRED)
find_package(Qt6 COMPONENTS Core Widgets Gui PrintSupport Svg OpenGL REQUIRED)


add_library   (MM ${SOURCES})


add_executable(runmcmc "${CMAKE_CURRENT_SOURCE_DIR}/src/runmcmc.cpp" )
add_executable(runnoble "${CMAKE_CURRENT_SOURCE_DIR}/src/runnoble.cpp" )

target_link_libraries(MM PRIVATE Qt6::Core Qt6::Widgets Qt6::Gui Qt6::PrintSupport Qt6::Svg Eigen3::Eigen -L${QLIB} -lQatPlotWidgets -lQatPlotting -lQatDataAnalysis -lQatGenericFunctions )


target_link_libraries(runmcmc PRIVATE -L. -lMM SoQt::SoQt Qt6::Core Qt6::Widgets Qt6::Gui Qt6::PrintSupport Qt6::Svg Qt6::OpenGL Eigen3::Eigen -L${QLIB} -lQatPlotWidgets -lQatPlotting -lQatDataAnalysis -lQatGenericFunctions )

target_link_libraries(runnoble PRIVATE -L. -lMM Qt6::Core Qt6::Widgets Qt6::Gui Qt6::PrintSupport Qt6::Svg Eigen3::Eigen -L${QLIB} -lQatPlotWidgets -lQatPlotting -lQatDataAnalysis -lQatGenericFunctions )

set_target_properties(runmcmc PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/local/bin"
)

set_target_properties(runnoble PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/local/bin"
)

