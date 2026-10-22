#pragma once

struct GLFWwindow;

namespace forge
{
    class OpenGLContext
    {
        public:
            explicit OpenGLContext(GLFWwindow* window);

            void initialise();
            void swapBuffers() const;

        private:
            GLFWwindow* window_;
    };
}
