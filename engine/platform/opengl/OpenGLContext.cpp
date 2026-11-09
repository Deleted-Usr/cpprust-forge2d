#include "OpenGLContext.hpp"

#include <forge2d/debug/Assert.hpp>
#include "OpenGLDebug.hpp"

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

        FORGE_CHECK(gladLoadGL(glfwGetProcAddress) != 0, "Failed to initialise GLAD");

        internal::opengl::logDriverInfo();
        #ifdef FORGE2D_DEBUG
            internal::opengl::enableDebugOutput();
        #endif

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
