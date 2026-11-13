// Window.hpp
#pragma once

/// @file Window.hpp
/// @brief The native window and its OpenGL context.

#include <memory>
#include <string>

#include <forge2d/graphics/Colour.hpp>
#include <forge2d/graphics/FrameInfo.hpp>

struct GLFWwindow;

namespace forge
{
    class OpenGLContext;
    class InputManager;
    namespace internal::glfw { class GLFWInput; }

    /// @brief Settings used to create a `Window`.
    struct WindowConfig
    {
        /// Text shown in the title bar. Defaults to `"Forge2D"`.
        std::string title = "Forge2D";

        int width  = 1280; ///< Initial width in logical (screen-space) units.
        int height = 720;  ///< Initial height in logical (screen-space) units.

        /// The window's default background colour
        Colour clearColour = colours::Black;

        /// Whether the user can resize the window. Defaults to `true`.
        bool resizable = true;

        /// Whether presenting waits for the display's refresh, capping the frame rate and
        /// preventing tearing. Defaults to `true`.
        bool vsync     = true;
    };

    /// @brief A desktop window with an OpenGL 4.6 core context.
    ///
    /// Engine-facing: `Application` owns the window and drives it each frame. Games normally
    /// don't use it directly.
    ///
    /// Creating a window makes its GL context current on the calling thread and loads the GL
    /// functions. The window also collects its own input events, which `updateInput()` hands to
    /// an `InputManager`. In Debug builds the context is created with GL debug output enabled.
    ///
    /// @note Not thread-safe. Create, use and destroy it on the main thread.
    class Window
    {
        public:
            /// @brief Opens a window and creates its OpenGL context.
            ///
            /// @param config Title, size and behaviour settings.
            ///
            /// @throws std::runtime_error If GLFW can't be initialised or the window and context
            ///         can't be created (for example if OpenGL 4.6 isn't supported).
            /// @note Every build stops with a fatal error if the GL functions can't be loaded.
            explicit Window(const WindowConfig& config);

            /// @brief Detaches input callbacks, destroys the GL context and closes the window.
            ///
            /// @pre Every GPU resource (textures, the renderer) has already been released.
            ~Window();

            Window(const Window&) = delete;
            Window& operator=(const Window&) = delete;

            /// @brief Whether the user has asked to close the window (e.g. with its close button).
            [[nodiscard]]
            bool shouldClose() const;

            /// @brief Processes pending window and input events from the operating system.
            ///
            /// Input events are queued, not applied: call `updateInput()` afterwards to apply
            /// them to an `InputManager`.
            void pollEvents();

            /// @brief Moves this window's queued input events into @p input, starting a new
            ///        input frame.
            ///
            /// Call once per frame on the main thread, after `pollEvents()` and before the update
            /// that reads @p input. It clears the previous frame's presses, releases, deltas and
            /// text even when there are no new events. Events that arrive after this call stay
            /// queued for the next one.
            ///
            /// @param input The snapshot to advance.
            ///
            /// @throws std::bad_alloc If memory ran out while events were being queued. The
            ///         failure is permanent, because input events have been lost.
            void updateInput(InputManager& input);

            /// @brief Presents the frame that has just been drawn.
            ///
            /// With `WindowConfig::vsync` on, this waits for the display's next refresh.
            void swapBuffers();

            /// @brief Current width in logical (screen-space) units.
            [[nodiscard]]
            int width() const;

            /// @brief Current height in logical (screen-space) units.
            [[nodiscard]]
            int height() const;

            /// The window's default clear colour for each frame
            [[nodiscard]]
            Colour clearColour() const;

            /// @brief Current logical size and framebuffer size in pixels.
            ///
            /// Queried from the window each call, so it reflects resizes and DPI changes. See
            /// `FrameInfo` for how the two sizes differ.
            [[nodiscard]] FrameInfo frameInfo() const;

            /// @brief The underlying GLFW window, for engine and platform code.
            ///
            /// Gameplay code shouldn't need it.
            ///
            /// @warning Don't change the window's user pointer or input callbacks: the window's
            ///          input collector relies on them.
            [[nodiscard]]
            GLFWwindow* nativeHandle() const;

        private:
            GLFWwindow* handle_ = nullptr;

            std::unique_ptr<OpenGLContext> context_;
            std::unique_ptr<internal::glfw::GLFWInput> input_;

            Colour clearColour_;

    };
}
