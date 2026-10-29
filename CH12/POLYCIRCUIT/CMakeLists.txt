cmake_minimum_required(VERSION 3.16)

project(Circuit VERSION 1.0.0 LANGUAGES CXX)
include(GNUInstallDirs)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 20)
find_package(QatGenericFunctions REQUIRED)
find_package(Eigen3 REQUIRED)
include(CMakePackageConfigHelpers)

file( GLOB HEADERS include/Circuit/*.h include/Circuit/*.icc )
file( GLOB SOURCES src/*.cpp)

add_library(Circuit ${SOURCES})


add_library(Circuit::Circuit ALIAS Circuit)


target_include_directories(Circuit
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(Circuit PUBLIC Qat::QatGenericFunctions Eigen3::Eigen)


set_target_properties( Circuit PROPERTIES
   VERSION ${PROJECT_VERSION}
   SOVERSION ${PROJECT_VERSION_MAJOR} )


install(TARGETS Circuit
    EXPORT ${PROJECT_NAME}-export
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            COMPONENT          Runtime
            NAMELINK_COMPONENT Development   # Requires CMake 3.12
)

install( FILES ${HEADERS}
   DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Circuit
   COMPONENT Development )


configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/CircuitPackageConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
    NO_SET_AND_CHECK_MACRO
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(EXPORT ${PROJECT_NAME}-export
    FILE "${PROJECT_NAME}Targets.cmake"
    NAMESPACE "CIRCUIT::"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
)

