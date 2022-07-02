cmake_minimum_required(VERSION 3.21)

project(build-your-own-lisp)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

include(CTest)
include(FetchContent)
set(FETCHCONTENT_QUIET 0)

# Download / Include MPC library
FetchContent_Declare(
  mpc_fetch
  GIT_REPOSITORY https://github.com/orangeduck/mpc.git
  GIT_TAG        65f20a1a0b3249a475efa8ecb7b5ecd2c1c071c4)
FetchContent_GetProperties(mpc_fetch)
if(NOT mpc_fetch_POPULATED)
  FetchContent_Populate(mpc_fetch)
  add_library(
    mpc STATIC
    ${mpc_fetch_SOURCE_DIR}/mpc.c
    ${mpc_fetch_SOURCE_DIR}/mpc.h)
  target_include_directories(mpc PUBLIC ${mpc_fetch_SOURCE_DIR})
endif()

# Enable GNU readline functionality provided by the editline library
find_package(editline REQUIRED)

# Output directory structure configuration.
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")
set(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")
set(TEST_OUTPUT_PATH "${EXECUTABLE_OUTPUT_PATH}/test")

# Add symbolic link to resources folder in build
add_custom_target(resources)
FILE(TO_NATIVE_PATH "${PROJECT_SOURCE_DIR}/resources" source)
FILE(TO_NATIVE_PATH "${PROJECT_BINARY_DIR}/resources" destination)
IF (WIN32)
  add_custom_command(
    TARGET resources POST_BUILD
    COMMAND mklink /D ${destination} ${source}
    DEPENDS ${destination}
    COMMENT "symbolic link resources folder from ${source} => ${destination}"
    )
ELSE()
  add_custom_command(
    TARGET resources POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${source} ${destination}
    DEPENDS ${destination}
    COMMENT "symbolic link resources folder from ${source} => ${destination}"
    )
ENDIF()

# Link libraries to all targets added later
if(UNIX)
  link_libraries(m)
endif(UNIX)

# Generates a compile_commands.json file containing the exact compiler calls
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
FILE(TO_NATIVE_PATH "${PROJECT_BINARY_DIR}/compile_commands.json" sourceJson)
FILE(TO_NATIVE_PATH "${PROJECT_SOURCE_DIR}/build/compile_commands.json" destinationJson)

if(EXISTS ${sourceJson})
  configure_file(${sourceJson} ${destinationJson} COPYONLY)
endif ()

# Project-wide include directory configuration.
include_directories("${PROJECT_SOURCE_DIR}/src")

# Project library source lives in src/.
add_subdirectory(src)

# CTest sets BUILD_TESTING option to ON by default.
# Test-related configuration goes here.
if (BUILD_TESTING)
  enable_testing()
  add_subdirectory(test)
endif()

