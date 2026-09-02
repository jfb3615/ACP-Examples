cmake_minimum_required(VERSION 3.16)
project(client-server-elaborate LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)


find_package(QatPlotWidgets      REQUIRED)
find_package(Eigen3              REQUIRED)

set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_BUILD_RPATH "${QatPlotWidgets_DIR}/../..")

file(GLOB CLIENTSOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/client/*.cpp")
file(GLOB SERVERSOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/server/*.cpp")

add_executable(server ${SERVERSOURCES})
target_link_libraries(server PRIVATE QatPlotWidgets QatPlotting QatDataAnalysis QatGenericFunctions Qt6::Widgets Qt6::Core Eigen3::Eigen)

add_executable(client ${CLIENTSOURCES})
target_link_libraries(client PRIVATE QatDataAnalysis QatGenericFunctions)

