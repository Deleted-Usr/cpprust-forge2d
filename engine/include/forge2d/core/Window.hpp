// Window.hpp
#pragma once

#include <memory>
#include <string>
#include <forge2d/graphics/FrameInfo.hpp>

struct GLFWwindow;

namespace forge
{
    class OpenGLContext;

    struct WindowConfig
    {
        std::string title = "Forge2D";

        int width  = 1280;
        int height = 720;

        bool resizable = true;
        bool vsync     = true;
    };

    class Window
    {
        public:
            explicit Window(const WindowConfig& config);

            ~Window();

            Window(const Window&) = delete;
            Window& operator=(const Window&) = delete;

            [[nodiscard]]
            bool shouldClose() const;

            void pollEvents();
            void swapBuffers();

            [[nodiscard]]
            int width() const;

            [[nodiscard]]
            int height() const;

            /// Queries current logical and physical sizes, including after resize.
            [[nodiscard]] FrameInfo frameInfo() const;

            // Engine/platform code may need this.
            // Gameplay generally should not.
            [[nodiscard]]
            GLFWwindow* nativeHandle() const;

        private:
            GLFWwindow* handle_ = nullptr;

            std::unique_ptr<OpenGLContext> context_;

    };
}
