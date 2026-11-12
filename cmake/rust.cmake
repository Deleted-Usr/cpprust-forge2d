include_guard(GLOBAL)
include(FetchContent)

# Match the project's 64-bit Windows MinGW configuration.
# A -DRust_CARGO_TARGET=... argument can override this default.
if(WIN32 AND MINGW AND CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(
        Rust_CARGO_TARGET
        "x86_64-pc-windows-gnu"
        CACHE STRING "Rust compilation target"
    )
endif()

# Pin Corrosion rather than following its development branch.
FetchContent_Declare(
    Corrosion
    GIT_REPOSITORY https://github.com/corrosion-rs/corrosion.git
    GIT_TAG v0.6.1
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(Corrosion)

# Import the bridge static library.
# Cargo builds forge-assets and other Rust dependencies automatically.
corrosion_import_crate(
    MANIFEST_PATH "${PROJECT_SOURCE_DIR}/Cargo.toml"
    CRATES forge-bridge
    CRATE_TYPES staticlib
    LOCKED
)

# Generate and compile the C++ side of the CXX bridge.
# FILES paths are relative to forge-bridge/src/.
corrosion_add_cxxbridge(
    forge2d_rust
    CRATE forge_bridge

    FILES
        assets.rs
        audio.rs
)

# Give engine code one stable CMake target to link against.
add_library(forge2d::rust ALIAS forge2d_rust)
