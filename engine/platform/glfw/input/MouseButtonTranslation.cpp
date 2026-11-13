#include <glfw/input/MouseButtonTranslation.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace forge
{
    MouseButton translateMouseButton(int button) noexcept
    {
        switch (button)
        {
            case GLFW_MOUSE_BUTTON_LEFT:   return MouseButton::Left;
            case GLFW_MOUSE_BUTTON_RIGHT:  return MouseButton::Right;
            case GLFW_MOUSE_BUTTON_MIDDLE: return MouseButton::Middle;
            case GLFW_MOUSE_BUTTON_4:      return MouseButton::Button4;
            case GLFW_MOUSE_BUTTON_5:      return MouseButton::Button5;
            case GLFW_MOUSE_BUTTON_6:      return MouseButton::Button6;
            case GLFW_MOUSE_BUTTON_7:      return MouseButton::Button7;
            case GLFW_MOUSE_BUTTON_8:      return MouseButton::Button8;
            default:                       return MouseButton::Unknown;
        }
    }
}
