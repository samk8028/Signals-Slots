cmake_minimum_required(VERSION 3.16)

project(SignalsSlots LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Option to choose shared / static
option(SIGNALS_SLOTS_BUILD_SHARED
  "Build SignalsSlots as a shared library (ON) or static library (OFF)" OFF)

if(SIGNALS_SLOTS_BUILD_SHARED)
  set(SIGNALS_SLOTS_LIB_TYPE SHARED)
else()
  set(SIGNALS_SLOTS_LIB_TYPE STATIC)
endif()

# Sources / headers
file(GLOB_RECURSE SIGNALS_SLOTS_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")
file(GLOB_RECURSE SIGNALS_SLOTS_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp")

add_library(SignalsSlots ${SIGNALS_SLOTS_LIB_TYPE} ${SIGNALS_SLOTS_SOURCES} ${SIGNALS_SLOTS_HEADERS})

if(NOT SIGNALS_SLOTS_BUILD_SHARED)
  target_compile_definitions(SignalsSlots PRIVATE AS_STATIC_LIBRARY)
endif()

# Corrected target_include_directories: variable expansion is inside the generator expression
target_include_directories(SignalsSlots
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Windows DLL export macro:
if(WIN32 AND SIGNALS_SLOTS_BUILD_SHARED)
  target_compile_definitions(SignalsSlots PRIVATE SIGNALS_SLOTS_EXPORTS)
endif()

target_compile_definitions(SignalsSlots
  PUBLIC $<$<NOT:$<BOOL:${SIGNALS_SLOTS_BUILD_SHARED}>>:SIGNALS_SLOTS_STATIC_BUILD>
)

# Installation boilerplate (kept minimal here)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(TARGETS SignalsSlots
  EXPORT SignalsSlotsTargets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT SignalsSlotsTargets
  FILE SignalsSlotsTargets.cmake
  NAMESPACE SignalsSlots::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SignalsSlots
)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/SignalsSlotsConfigVersion.cmake"
  VERSION 1.0.0
  COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/SignalsSlotsConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/SignalsSlotsConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SignalsSlots
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
    @ONLY
)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
    DESTINATION .
)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/SignalsSlotsConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/SignalsSlotsConfigVersion.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SignalsSlots
)

# Export for build-tree find_package
export(EXPORT SignalsSlotsTargets
  FILE ${CMAKE_CURRENT_BINARY_DIR}/SignalsSlotsTargets.cmake
  NAMESPACE SignalsSlots::
)
