#include <forge2d/core/Window.hpp>

#include "../../platform/opengl/OpenGLContext.hpp"

#include <stdexcept>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace
{
    std::size_t glfwWindowCount = 0;
}

namespace forge
{
    Window::Window(const WindowConfig& config) : width_(config.width), height_(config.height)
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
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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

        ++glfwWindowCount;

        context_ = std::make_unique<OpenGLContext>(handle_);
        context_->initialise();

        glfwSwapInterval(config.vsync ? 1 : 0);
    }

    Window::~Window()
    {
        // Destroy OpenGL-related state before destroying
        // the native window/context.
        context_.reset();

        if (handle_)
        {
            glfwDestroyWindow(handle_);
            handle_ = nullptr;
        }

        if (glfwWindowCount >  0) --glfwWindowCount;
        if (glfwWindowCount == 0) glfwTerminate();
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

    int Window::width() const
    {
        return width_;
    }

    int Window::height() const
    {
        return height_;
    }

    GLFWwindow* Window::nativeHandle() const
    {
        return handle_;
    }
}
