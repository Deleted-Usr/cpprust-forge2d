# Forge2D

A lightweight 2D game engine written in C++20, with engine subsystems in Rust.

Forge2D handles the window, the game loop, batched 2D rendering, a camera and cached asset
loading, so game code only has to describe *what* to update and draw. The C++ engine talks to
its Rust crates through [`cxx`](https://cxx.rs), and the build is driven by CMake with
[Corrosion](https://github.com/corrosion-rs/corrosion) building the Rust side automatically.

> **Status:** early development (`0.1.0`). The public API under `engine/include/forge2d/` will
> change between minor versions until `1.0`. See [CHANGELOG.md](CHANGELOG.md).

## Features

- **Game loop:** subclass `forge::Game`, hand it to `forge::Application`, and implement
  `update(deltaTime)` and `render(renderer)`.
- **Batched 2D renderer (OpenGL 4.6):**
  - Filled or outlined rectangles, triangles and circles
  - Lines with thickness
  - Sprites with colour tinting
  - Screen-space coordinates by default (top-left origin, +Y down)
- **Camera2D:**
  - Follow a target, with smoothing and look-ahead
  - Zoom, rotation and screen shake
  - `resetCamera()` to go back to screen space for HUD and UI drawing
- **Assets:**
  - A cached `AssetManager` with typed asset IDs or string paths
  - Parallel preloading with `preloadTextures()`
  - A visible magenta "missing texture" instead of a crash when a file can't be loaded
  - PNG and JPEG decoding in Rust (`forge-assets`, built on the `image` crate)
- **Textures:** copyable, reference-counted `Texture2D` handles, with filter, wrap and mipmap
  options.
- **Debug tools:** levelled logging, `FORGE_ASSERT`/`FORGE_CHECK`/`FORGE_ENSURE` macros and
  OpenGL debug output in Debug builds.
- **Shaders:** loaded from disk in Debug builds so you can edit them without recompiling, and
  embedded into the binary in all other build types.

## Requirements

| Tool         | Version                                             |
|--------------|-----------------------------------------------------|
| CMake        | 3.25 or newer                                       |
| C++ compiler | C++20 support (GCC, Clang, or MinGW-w64 on Windows) |
| Rust         | 1.98.0 (installed by `rustup` from `rust-toolchain.toml`) |
| GPU / driver | OpenGL 4.6                                          |

GLFW, GLM and Corrosion are downloaded automatically by CMake. GLAD is vendored in `external/`.

### Platform support

| Platform | Status                                                                 |
|----------|------------------------------------------------------------------------|
| Windows  | Primary platform, built with MinGW-w64 (`x86_64-pc-windows-gnu`)       |
| Linux    | Expected to work, not yet tested regularly                             |
| macOS    | Not supported: macOS stops at OpenGL 4.1, and Forge2D requires 4.6     |

## Building

```bash
git clone https://github.com/Deleted-Usr/cpprust-forge2d.git
cd cpprust-forge2d
```

Configure and build (Debug):

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
```

On Windows with MinGW, choose the generator explicitly:

```bash
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug
```

### Build options

| Option                   | Default | Description                     |
|--------------------------|---------|---------------------------------|
| `FORGE2D_BUILD_PLATFORMER` | `ON`  | Build the platformer game       |
| `FORGE2D_BUILD_EXAMPLES`   | `ON`  | Build the sandbox example       |
| `FORGE2D_BUILD_TESTS`      | `OFF` | Build the test executables      |

## Running

Executables are written to `build/bin/`. Debug builds read shaders from a `shaders/` folder
next to the executable (copied there by the build), and assets are loaded relative to the
working directory, so run them from that folder:

```bash
cd build/bin
./forge2d-sandbox
```

## A minimal game

```c++
#include <exception>
#include <memory>

#include <forge2d/core/Application.hpp>
#include <forge2d/debug/Log.hpp>

class MyGame final : public forge::Game
{
    public:
        void onStart(forge::AssetManager& assets) override
        {
            mySprite_ = assets.loadTexture("cool-image.png");
        }

        void update(float deltaTime, const forge::InputManager& input) override
        {
            if (input.isKeyDown(forge::Key::D))
                x_ += 100.0f * deltaTime; // 100 pixels per second
        }

        void render(forge::Renderer2D& renderer) override
        {
            renderer.drawRect(
                { .position = { x_, 100.0f }, .scale = { 64.0f, 64.0f } },
                forge::colours::Cyan
            );

            renderer.drawSprite(
                mySprite_,
                { .position = { 200.0f, 200.0f }, .scale = { 64.0f, 64.0f } }
            );
        }

    private:
        float x_ = 0.0f;
        forge::Texture2D mySprite_;
};

int main()
{
    try
    {
        forge::ApplicationConfig config;
        config.window.title  = "My Game";
        config.window.width  = 800;
        config.window.height = 600;
        config.assetRoot     = "assets";

        forge::Application app(config, std::make_unique<MyGame>());
        return app.run();
    }
    catch (const std::exception& e)
    {
        FORGE_LOG_FATAL("Fatal error: {}", e.what());
        return 1;
    }
}
```

See `examples/sandbox/main.cpp` for a slightly larger example, including the camera.

## Project layout

```
cpprust-forge2d/
├── engine/
│   ├── include/forge2d/   # Public API: the only headers games should include
│   ├── src/               # Engine implementation (private)
│   ├── platform/opengl/   # OpenGL backend (private)
│   ├── shaders/           # GLSL batch shaders (+ legacy/)
│   └── assets/            # Engine-provided images
├── rust/
│   ├── forge-bridge/      # cxx bridge: the C++ <-> Rust boundary
│   ├── forge-assets/      # Image decoding
│   └── forge-audio/       # Audio engine (kira), not yet exposed to C++
├── games/platformer/      # A game built on Forge2D
├── examples/sandbox/      # Small feature playground
├── tests/                 # CPU, GPU and image tests (CTest)
├── docs/                  # Renderer, camera and scripting design notes
├── cmake/                 # Dependency, Rust and shader helpers
└── external/glad/         # Vendored OpenGL loader
```

**Public API rule:** everything under `engine/include/forge2d/` is public, except anything in a
`forge::internal` namespace. `engine/src/`, `engine/platform/` and the Rust crates are
implementation details and can change at any time.

## Tests

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DFORGE2D_BUILD_TESTS=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

| Test           | What it covers                                            |
|----------------|-----------------------------------------------------------|
| `forge2d-cpu`    | Renderer batching logic, no GPU needed                  |
| `forge2d-gpu`    | Rendered pixel output; **skipped** without an OpenGL 4.6 context |
| `forge2d-images` | PNG/JPEG decoding through the Rust image decoder        |

The tests use only the standard library, with no external test framework.

## Documentation

Design notes live in `docs/`:

- [`Rendering.md`](docs/Rendering.md): how the renderer is structured
- [`OpenGLRenderer.md`](docs/markdown/OpenGLRenderer.md): the OpenGL backend and batching
- [`OpenGLvsVulkan.md`](docs/markdown/OpenGLvsVulkan.md): graphics API comparison
- [`Camera2DGameCodeSyntax.md`](docs/markdown/Camera2DGameCodeSyntax.md): camera usage from game code
- [`EngineResourcesManager.md`](docs/markdown/EngineResourcesManager.md): engine resource management
- [`scripting/`](docs/markdown/scripting): planned Rust and Kotlin scripting frontends

## Roadmap

- [x] Keyboard/mouse input snapshots (`InputManager`)
- [ ] Audio engine implementation
- [ ] Fixed-timestep game loop option
- [ ] Text rendering
- [ ] Built-in UI using the renderer and input system
- [ ] File I/O system
- [ ] Scripting frontends (Rust, Kotlin)

## License

No license has been chosen yet. Until one is added, all rights are reserved.
