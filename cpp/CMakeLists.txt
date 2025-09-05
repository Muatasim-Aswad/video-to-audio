cmake_minimum_required(VERSION 3.16)
project(VideoToAudioConverter VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Compiler flags for optimization
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -s")
endif()

# Find required packages
find_package(PkgConfig REQUIRED)

# Check if we have filesystem support
include(CheckIncludeFileCXX)
check_include_file_cxx("filesystem" HAS_FILESYSTEM)
if(NOT HAS_FILESYSTEM)
    check_include_file_cxx("experimental/filesystem" HAS_EXPERIMENTAL_FILESYSTEM)
    if(HAS_EXPERIMENTAL_FILESYSTEM)
        add_definitions(-DUSE_EXPERIMENTAL_FILESYSTEM)
        set(FILESYSTEM_LIB stdc++fs)
    endif()
endif()

# Source files
set(SOURCES
    src/main.cpp
    src/VideoFile.cpp
    src/Config.cpp
    src/VideoConverter.cpp
    src/ConsoleUtils.cpp
)

# Create executable
add_executable(convert ${SOURCES})

# Link libraries
if(FILESYSTEM_LIB)
    target_link_libraries(convert ${FILESYSTEM_LIB})
endif()

# Include directories
target_include_directories(convert PRIVATE include)

# Compiler-specific options
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(convert PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Installation
install(TARGETS convert RUNTIME DESTINATION bin)
