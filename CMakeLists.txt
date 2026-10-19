cmake_minimum_required(VERSION 4.3)
project(cpprust_forge2d VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON) # Force C++ 17 or higher
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Use an installed GLFW if one is found, otherwise download and
# build it automatically
find_package(glfw3 3.4 QUIET)

if (NOT glfw3_FOUND)
    include(FetchContent)

    set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
            glfw
            GIT_REPOSITORY https://github.com/glfw/glfw.git
            GIT_TAG        3.4
            GIT_SHALLOW    TRUE
    )
    FetchContent_MakeAvailable(glfw)
endif()

find_package(OpenGL REQUIRED)

add_executable(${PROJECT_NAME}
    engine/forge2d/src/main.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/engine/forge2d/include"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw       # Target name from find_package(glfw3)
    OpenGL::GL # Standard modern OpenGL target
)

if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        /W4
        /permissive-
        /utf-8
    )

    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY
        VS_STARTUP_PROJECT ${PROJECT_NAME}
    )
else()
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()
