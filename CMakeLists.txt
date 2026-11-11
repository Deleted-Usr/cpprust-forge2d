cmake_minimum_required(VERSION 3.25)
project(
    cpprust_forge2d
    VERSION 0.1.0
    DESCRIPTION "A lightweight 2D game engine"
    LANGUAGES C CXX
)

# ------------------------------------------------------------
# GLOBAL PROJECT CONFIGURATION
# ------------------------------------------------------------

# Useful for IDEs, clangd, etc.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Don't silently fall back to compiler-specific GNU extensions.
set(CMAKE_CXX_EXTENSIONS OFF)

# Put executables in one predictable location.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
    "${CMAKE_BINARY_DIR}/bin"
)

# Static libraries, such as libforge2d.a.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY
    "${CMAKE_BINARY_DIR}/lib"
)

# Shared libraries, when you eventually have any.
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY
   "${CMAKE_BINARY_DIR}/lib"
)

# ------------------------------------------------------------
# OPTIONS
# ------------------------------------------------------------

option(
    FORGE2D_BUILD_PLATFORMER
    "Build the platformer"
    ON
)

option(
    FORGE2D_BUILD_EXAMPLES
    "Build Forge2D examples"
    ON
)

option(
    FORGE2D_BUILD_TESTS
    "Build Forge2D tests"
    OFF
)

# ------------------------------------------------------------
# DEPENDENCIES
# ------------------------------------------------------------

include(cmake/dependencies.cmake)
include(cmake/rust.cmake)

# ------------------------------------------------------------
# ENGINE
# ------------------------------------------------------------

add_subdirectory(engine)

# ------------------------------------------------------------
# GAMES
# ------------------------------------------------------------

if(FORGE2D_BUILD_PLATFORMER)
    add_subdirectory(games/platformer)
endif()

# ------------------------------------------------------------
# EXAMPLES
# ------------------------------------------------------------

if(FORGE2D_BUILD_EXAMPLES)
    add_subdirectory(examples/sandbox)
endif()

# ------------------------------------------------------------
# TESTS
# ------------------------------------------------------------

if(FORGE2D_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
