#include "OpenGLContext.hpp"

#include <stdexcept>

#include <glad/gl.h>

#define GLFW_INCLUDE_NONE
#include <GL/gl.h>
#include <GLFW/glfw3.h>

namespace forge
{
    OpenGLContext::OpenGLContext(GLFWwindow* window) : window_(window) { }

    void OpenGLContext::initialise() const
    {
        glfwMakeContextCurrent(window_);

        if (gladLoadGL(glfwGetProcAddress) == 0)
        {
            throw std::runtime_error("Failed to initialise GLAD");
        }

        int width, height;

        glfwGetFramebufferSize(window_, &width, &height);
        glViewport(0, 0, width, height);

        glfwSetFramebufferSizeCallback(
            window_,
            [](
                GLFWwindow*,
                int width,
                int height
            ) {
                glViewport(0, 0, width, height);
            }
        );
    }

    void OpenGLContext::swapBuffers() const
    {
        glfwSwapBuffers(window_);
    }
}
