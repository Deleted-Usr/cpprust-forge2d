#include <forge2d/core/Window.hpp>

#include <stdexcept>

#include <glfw/input/GLFWInput.hpp>
#include <opengl/OpenGLContext.hpp>

#include <forge2d/debug/Log.hpp>
#include <forge2d/input/InputManager.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace
{
    std::size_t glfwWindowCount = 0;
}

namespace forge
{
    Window::Window(const WindowConfig& config)
    {
        if (glfwWindowCount == 0)
        {
            if (!glfwInit())
            {
                throw std::runtime_error("Failed to initialise GLFW");
            }
        }

        // Asking GFLW for a modern OpenGL Core context.
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        glfwWindowHint(GLFW_RESIZABLE, config.resizable);

        #ifdef FORGE2D_DEBUG
            glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
        #endif

        handle_ = glfwCreateWindow(
            config.width,
            config.height,
            config.title.c_str(),
            nullptr,
            nullptr
        );

        if (!handle_)
        {
            if (glfwWindowCount == 0)
            {
                glfwTerminate();
            }

            throw std::runtime_error("Failed to create GLFW window");
        }

        glfwSetErrorCallback([](int code, const char* description) {
            FORGE_LOG_ERROR("[GLFW] {} (code {})", description, code);
        });

        ++glfwWindowCount;

        context_ = std::make_unique<OpenGLContext>(handle_);
        context_->initialise();

        glfwSwapInterval(config.vsync ? 1 : 0);
        input_ = std::make_unique<internal::glfw::GLFWInput>(handle_);
    }

    Window::~Window()
    {
        // Detach callbacks while the native window is still alive.
        input_.reset();
        // Destroy OpenGL-related state before destroying
        // the native window/context.
        context_.reset();

        if (handle_)
        {
            glfwDestroyWindow(handle_);
            handle_ = nullptr;
        }

        if (glfwWindowCount >  0) --glfwWindowCount;
        if (glfwWindowCount == 0)   glfwTerminate();
    }

    bool Window::shouldClose() const
    {
        return glfwWindowShouldClose(handle_);
    }

    void Window::pollEvents()
    {
        glfwPollEvents();
    }

    void Window::swapBuffers()
    {
        context_->swapBuffers();
    }

    void Window::updateInput(InputManager& input)
    {
        input.endFrame(input_->takeEvents());
    }

    int Window::width() const
    {
        int width, height;
        glfwGetWindowSize(handle_, &width, &height);

        return width;
    }

    int Window::height() const
    {
        int width, height;
        glfwGetWindowSize(handle_, &width, &height);

        return height;
    }

    FrameInfo Window::frameInfo() const
    {
        int width, height, framebufferWidth, framebufferHeight;

        glfwGetWindowSize(handle_, &width, &height);
        glfwGetFramebufferSize(handle_, &framebufferWidth, &framebufferHeight);

        return {
            { static_cast<float>(width), static_cast<float>(height) },
            framebufferWidth,
            framebufferHeight
        };
    }

    GLFWwindow* Window::nativeHandle() const
    {
        return handle_;
    }
}
