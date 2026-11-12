# Tests use the standard library only; no extra downloaded test framework.
add_executable(forge2d-image-tests ImageTests.cpp)
target_link_libraries(forge2d-image-tests PRIVATE forge2d)
target_compile_definitions(forge2d-image-tests PRIVATE
    FORGE2D_IMAGE_FIXTURE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/fixtures/images"
    FORGE2D_ENGINE_ASSET_DIR="${PROJECT_SOURCE_DIR}/engine/assets"
    FORGE2D_IMAGE_TEST_TEMP_DIR="${CMAKE_CURRENT_BINARY_DIR}"
)
add_test(NAME forge2d-images COMMAND forge2d-image-tests)

add_executable(forge2d-tests Renderer2DTests.cpp)

target_include_directories(
    forge2d-tests PRIVATE
    "${PROJECT_SOURCE_DIR}/engine/src"
)

target_link_libraries (forge2d-tests PRIVATE forge2d)

add_test(NAME forge2d-cpu COMMAND forge2d-tests)

add_executable(forge2d-renderer-gpu-tests Renderer2DGpuTests.cpp)
target_include_directories(
    forge2d-renderer-gpu-tests PRIVATE

    "${PROJECT_SOURCE_DIR}/engine/src"
    "${PROJECT_SOURCE_DIR}/engine/platform"
)

target_link_libraries(
    forge2d-renderer-gpu-tests PRIVATE

    forge2d
    glfw
    glad::glad
)

target_compile_definitions(
    forge2d-renderer-gpu-tests PRIVATE

    FORGE2D_TEST_SHADER_DIR="${PROJECT_SOURCE_DIR}/engine/shaders"
    $<$<NOT:$<CONFIG:Debug>>:FORGE2D_TEST_EMBEDDED_SHADERS>
    FORGE2D_IMAGE_FIXTURE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/fixtures/images"
)

add_test(NAME forge2d-gpu COMMAND forge2d-renderer-gpu-tests)
# A machine without an OpenGL 4.6 context can still run the CPU tests.
set_tests_properties(forge2d-gpu PROPERTIES SKIP_RETURN_CODE 77)
