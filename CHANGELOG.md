# Changelog

All notable changes to Forge2D are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project follows [Semantic Versioning](https://semver.org/).

## Unreleased

### Added

#### Core
- `Screen`: a new game-facing header used to retrieve the screen's logical width and height. Only static, cannot 
be instantiated.

## 0.1.0 - 2026-11-14 - Initial Release

The first version of Forge2D: a window and game loop, a batched OpenGL 2D renderer with a camera,
a cached asset pipeline with Rust image decoding, and debug tooling.

Forge2D is dual licensed under MIT OR Apache-2.0.

### Added

#### Core
- `Application` and `ApplicationConfig`, which own the window, asset manager, input manager, renderer and run the game loop:
events, `update(deltaTime)`, rendering and buffer swapping. Elapsed time is measured with `std::chrono::steady_clock`.
- `Game` base class with `onStart(AssetManager&)`, `update(float, const InputManager&)`, `render(Renderer2D&)` and
`onStop()` hooks.
- `Window` and `WindowConfig` (title, size, resizability, vsync), backed by GLFW and an OpenGL 4.6 core context.
`frameInfo()` reports logical and physical sizes, including after a resize.
- `WindowConfig::clearColour` sets the colour each frame is cleared to (black by default).

#### Rendering
- `Renderer2D`: a batched 2D renderer that groups draws to keep draw calls low.
  - `drawRect`, `drawTriangle` and `drawCircle`, each filled or outlined.
  - `drawLine` with a configurable thickness.
  - `drawSprite` with colour tinting.
  - Screen-space coordinates by default (top-left origin, +Y down).
- `Camera2D` with target following, smoothing, look-ahead that flips with the target's facing, zoom, rotation and
screen shake. `Renderer2D::setCamera()` draws in world space, and `resetCamera()` returns to screen space for HUD and UI.
- The view-projection matrix is supplied through a uniform buffer.
- `Colour` with `fromRGBA8()` and named constants in `forge::colours` (`White`, `Red`, `Cyan`, ...).
- `Texture2D`: a copyable, reference-counted handle to a GPU texture, with `TextureOptions` for filtering (`NEAREST`
by default for pixel art), wrapping and mipmaps.

#### Shaders
- Batch shaders (`batch.vert.glsl`, `batch.frag.glsl`). Debug builds load them from disk so they can be edited without
recompiling. All other build types embed them in the binary through a CMake-generated header.
- The original shape and sprite shaders are kept under `engine/shaders/legacy/`.

#### Assets
- `AssetManager`, which owns and caches every loaded texture.
  - A typed `loadTexture(assets::TextureAsset)` overload for compile-checked asset IDs, and a string overload for
  generated paths.
  - `loadTextureRange()` for loading indexed families such as `tile_0.png` ... `tile_N.png`.
  - `preloadTextures()` decodes files on worker threads and then uploads them on the GL thread.
- Missing or unreadable files show a magenta "missing texture" with a black border instead of crashing the game.
- `Image` loading for PNG and JPEG, decoded in Rust.

#### Math & Time
- `Vec2`, `Vec4`, `Mat4` and `Transform2D` (position, scale, rotation), with GLM used internally as a header-only library.
- `Time` and `Clock` utilities.

#### Input
- `InputManager`: a frame-based input manager supported by GLFW
  - `isKeyDown`, `isMouseButtonDown` for keys and mouse buttons held down over multiple frames
  - `isKeyPressed`, `isMouseButtonPressed` for keys and mouse buttons pressed this frame
  - `isKeyReleased`, `isMouseButtonReleased` for keys and mouse buttons released this frame
  - `textInput` returns the characters typed this frame as a UTF-8 encoded `std::string_view`

#### Debug
- Levelled logging (`TRACE` to `FATAL`) with source locations and pluggable sinks.
- `FORGE_ASSERT` (Debug only), `FORGE_CHECK`, `FORGE_ENSURE` and `FORGE_UNREACHABLE`, with a debugger break on each
supported compiler.
- OpenGL debug output in Debug builds.

#### Rust
- A Cargo workspace (edition 2024, Rust 1.98.0 pinned in `rust-toolchain.toml`) built through Corrosion.
- `forge-bridge`: the `cxx` bridge that exposes Rust subsystems to C++ as one `forge2d::rust` CMake target.
- `forge-assets`: PNG and JPEG decoding using the `image` crate.
- `forge-audio`: an audio engine built on `kira`, which can load and play sounds. It isn't exposed to C++ yet.

#### Examples
- **Sandbox**: a small playground that draws shapes and a sprite and sets up a following camera.
- **Platformer**: the start of a platformer game, with an asset registry for its textures.
- **Shoot 'em up**: the start of a shoot 'em up game.

#### Tests
- `forge2d-cpu`: renderer batching tests that need no GPU.
- `forge2d-gpu`: renderer pixel tests that read back rendered output. They're skipped, not failed, on machines without
an OpenGL 4.6 context.
- `forge2d-images`: decoding tests against PNG and JPEG fixtures.
- All tests use the standard library only and run through CTest with `-DFORGE2D_BUILD_TESTS=ON`.

#### Build
- CMake 3.25+ project with `FORGE2D_BUILD_PLATFORMER`, `FORGE2D_BUILD_SHMUP`, `FORGE2D_BUILD_EXAMPLES` and
`FORGE2D_BUILD_TESTS` options. The games and examples are only built by default when Forge2D is the top-level project.
- Usable from other projects with `FetchContent` or `add_subdirectory`, through the `forge2d::forge2d` target.
- `forge2d_copy_shaders(<target>)` copies the batch shaders next to a game's executable for Debug builds.
- GLFW 3.5.1, GLM 1.0.3 and Corrosion 0.6.1 fetched with `FetchContent`, with GLAD vendored under `external/`.
- Executables are written to `build/bin/` and libraries to `build/lib/`.
- `-Wall -Wextra -Wpedantic` (GCC/Clang) and `/W4` (MSVC) warnings.
- Windows MinGW builds target `x86_64-pc-windows-gnu` for Rust automatically.

#### Documentation
- Design notes on the renderer, the OpenGL backend and batching, OpenGL vs Vulkan, camera usage from game code,
engine resource management, and the planned Rust and Kotlin scripting frontends.


