#include "TestSupport.hpp"

#include <glfw/input/GLFWInput.hpp>
#include <forge2d/input/InputManager.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

int main()
{
    // Real GLFW registration, without a display or OpenGL context.
    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_NULL);
    if (!glfwInit()) return 77;
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    GLFWwindow* window = glfwCreateWindow(100, 100, "Input test", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate(); return 77;
    }

    int result = 0;
    try
    {
        forge::InputManager snapshot;
        {
            forge::internal::glfw::GLFWInput collector(window);

            auto key         = glfwSetKeyCallback         (window, nullptr);
            auto mouse = glfwSetMouseButtonCallback (window, nullptr);
            auto cursor  = glfwSetCursorPosCallback   (window, nullptr);
            auto scroll     = glfwSetScrollCallback      (window, nullptr);
            auto text        = glfwSetCharCallback        (window, nullptr);
            auto focus = glfwSetWindowFocusCallback (window, nullptr);

            check(key && mouse && cursor && scroll && text && focus, "callbacks registered");

            glfwSetKeyCallback         (window, key);
            glfwSetMouseButtonCallback (window, mouse);
            glfwSetCursorPosCallback   (window, cursor);
            glfwSetScrollCallback      (window, scroll);
            glfwSetCharCallback        (window, text);
            glfwSetWindowFocusCallback (window, focus);

            snapshot.endFrame(collector.takeEvents());

            focus(window, GLFW_TRUE);
            cursor(window, 10, 20);
            cursor(window, 14, 26);

            key(window, GLFW_KEY_D, 0, GLFW_PRESS,   0);
            key(window, GLFW_KEY_D, 0, GLFW_REPEAT,  0);
            key(window, GLFW_KEY_D, 0, GLFW_RELEASE, 0);

            key(window, GLFW_KEY_UNKNOWN, 0, GLFW_PRESS, 0);

            mouse(window, GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS,   0);
            mouse(window, GLFW_MOUSE_BUTTON_LEFT, GLFW_RELEASE, 0);
            mouse(window, GLFW_KEY_LAST,          GLFW_PRESS,   0);

            scroll(window, 0.5, 1.25);

            text(window, 0x1f600);

            check(!snapshot.isKeyPressed(forge::Key::D), "snapshot stays stable during callbacks");

            auto events = collector.takeEvents();

            key(window, GLFW_KEY_A, 0, GLFW_PRESS, 0);

            snapshot.endFrame(events);

            check(snapshot.isKeyPressed(forge::Key::D) && snapshot.isKeyReleased(forge::Key::D) && !snapshot.isKeyDown(forge::Key::D), "short key tap");
            check(snapshot.isMouseButtonPressed(forge::MouseButton::Left) && snapshot.isMouseButtonReleased(forge::MouseButton::Left), "short mouse tap");
            check(!snapshot.isKeyDown(forge::Key::A), "next-frame event isolation");

            check(snapshot.mouseDelta().x  == 4     && snapshot.mouseDelta().y  == 6,             "cursor accumulation");
            check(snapshot.scrollDelta().y == 1.25f && snapshot.textInput()     == U"\U0001F600", "scroll and Unicode");

            snapshot.endFrame(collector.takeEvents());

            check(snapshot.isKeyPressed(forge::Key::A), "event outside polling retained");
            key(window, GLFW_KEY_A, 0, GLFW_REPEAT, 0);

            snapshot.endFrame(collector.takeEvents());

            check(snapshot.isKeyDown(forge::Key::A) && !snapshot.isKeyPressed(forge::Key::A), "repeat ignored and queue drained once");
            focus(window, GLFW_FALSE);

            snapshot.endFrame(collector.takeEvents());

            check(snapshot.isKeyReleased(forge::Key::A) && !snapshot.isKeyDown(forge::Key::A), "focus releases held input");
        }

        check(glfwGetWindowUserPointer(window) == nullptr, "user pointer detached");

        check(glfwSetKeyCallback         (window, nullptr) == nullptr, "key callback detached");

        check(glfwSetMouseButtonCallback (window, nullptr) == nullptr, "mouse callback detached");
        check(glfwSetCursorPosCallback   (window, nullptr) == nullptr, "cursor callback detached");
        check(glfwSetScrollCallback      (window, nullptr) == nullptr, "scroll callback detached");

        check(glfwSetCharCallback        (window, nullptr) == nullptr, "text callback detached");
        check(glfwSetWindowFocusCallback (window, nullptr) == nullptr, "focus callback detached");

        std::cout << "GLFW callback and queue tests passed\n";
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << '\n';
        result = 1;
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return result;
}
