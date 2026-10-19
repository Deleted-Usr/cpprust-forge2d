#include <GLFW/glfw3.h>
#include <iostream>

#if defined(_WIN32)
    #include <windows.h>
#elif defined(__APPLE__)
    #include <CoreFoundation/CoreFoundation.h>
    // Note: True native UI on macOS usually requires Objective-C (NSAlert).
#endif

/// A dedicated error callback for GLFW's initialisation.
/// @param error_code The error code thrown
/// @param description The error message accompanying the code
void error_callback(int error_code, const char* description)
{
    std::string title = "GLFW Error (" + std::to_string(error_code) + ")";

    #if defined(_WIN32)
        MessageBoxA(NULL, description, title.c_str(), MB_OK | MB_ICONERROR);
    #else
        std::cerr << "[" << title << "] " << description << std::endl;
    #endif
}

int main()
{
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
    {
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(800, 600, "Forge2D", nullptr, nullptr);
    if (!window)
    {
        #if defined(_WIN32)
            MessageBoxA(NULL, "Failed to create GLFW window", "Runtime Error", MB_OK | MB_ICONERROR);
        #endif

        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    while (!glfwWindowShouldClose(window))
    {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
