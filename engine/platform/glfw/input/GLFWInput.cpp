#include <glfw/input/GLFWInput.hpp>
#include <glfw/input/KeyTranslation.hpp>
#include <glfw/input/MouseButtonTranslation.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace forge::internal::glfw
{
    GLFWInput::GLFWInput(GLFWwindow* window) : window_(window)
    {
        pending_.reserve(64);
        frameEvents_.reserve(64);

        glfwSetWindowUserPointer   (window_, this);

        glfwSetKeyCallback         (window_, &keyCallback);

        glfwSetMouseButtonCallback (window_, &mouseButtonCallback);
        glfwSetCursorPosCallback   (window_, &cursorPositionCallback);
        glfwSetScrollCallback      (window_, &scrollCallback);

        glfwSetCharCallback        (window_, &textCallback);

        glfwSetWindowFocusCallback (window_, &focusCallback);

        // Seed focus before position so an unfocused window cannot establish
        // a movement baseline that will later produce a jump on focus gain.
        enqueue(FocusEvent{glfwGetWindowAttrib(window_, GLFW_FOCUSED) == GLFW_TRUE});
        double x, y;
        glfwGetCursorPos(window_, &x, &y);
        enqueue(CursorEvent{x, y});
    }

    GLFWInput::~GLFWInput()
    {
        glfwSetKeyCallback (window_, nullptr);

        glfwSetMouseButtonCallback (window_, nullptr);
        glfwSetCursorPosCallback   (window_, nullptr);
        glfwSetScrollCallback      (window_, nullptr);

        glfwSetCharCallback (window_, nullptr);

        glfwSetWindowFocusCallback (window_, nullptr);
        glfwSetWindowUserPointer   (window_, nullptr);
    }

    void GLFWInput::enqueue(InputEvent event) noexcept
    {
        try { pending_.push_back(event); }
        catch (...)
        {
            if (!callbackError_) callbackError_ = std::current_exception();
        }
    }

    std::span<const InputEvent> GLFWInput::takeEvents()
    {
        if (callbackError_)
        {
            std::rethrow_exception(callbackError_);
        }

        // After the swap, pending_ owns the previously delivered batch. Clear
        // that old batch, not the new events now owned by frameEvents_.
        frameEvents_.swap(pending_);
        pending_.clear();
        return frameEvents_;
    }

    void GLFWInput::keyCallback(GLFWwindow* window, int key, int, int action, int)
    {
        if (action != GLFW_PRESS && action != GLFW_RELEASE) return;

        const auto translated = translateKey(key);

        if (translated == Key::Unknown) return;

        if (auto* input = static_cast<GLFWInput*>(glfwGetWindowUserPointer(window)))
        {
            input->enqueue(KeyEvent{translated, action == GLFW_PRESS});
        }
    }

    void GLFWInput::mouseButtonCallback(GLFWwindow* window, int button, int action, int)
    {
        if (action != GLFW_PRESS && action != GLFW_RELEASE) return;

        const auto translated = translateMouseButton(button);

        if (translated == MouseButton::Unknown) return;

        if (auto* input = static_cast<GLFWInput*>(glfwGetWindowUserPointer(window)))
        {
            input->enqueue(MouseButtonEvent{translated, action == GLFW_PRESS});
        }
    }

    void GLFWInput::cursorPositionCallback(GLFWwindow* window, double x, double y)
    {
        if (auto* input = static_cast<GLFWInput*>(glfwGetWindowUserPointer(window)))
        {
            input->enqueue(CursorEvent{x, y});
        }
    }

    void GLFWInput::scrollCallback(GLFWwindow* window, double x, double y)
    {
        if (auto* input = static_cast<GLFWInput*>(glfwGetWindowUserPointer(window)))
        {
            input->enqueue(ScrollEvent{x, y});
        }
    }

    void GLFWInput::textCallback(GLFWwindow* window, unsigned int codepoint)
    {
        if (auto* input = static_cast<GLFWInput*>(glfwGetWindowUserPointer(window)))
        {
            input->enqueue(TextEvent{static_cast<char32_t>(codepoint)});
        }
    }

    void GLFWInput::focusCallback(GLFWwindow* window, int focused)
    {
        if (auto* input = static_cast<GLFWInput*>(glfwGetWindowUserPointer(window)))
        {
            input->enqueue(FocusEvent{focused == GLFW_TRUE});
        }
    }
}
