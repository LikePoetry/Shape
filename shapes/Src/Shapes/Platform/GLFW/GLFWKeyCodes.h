#pragma once

#include "Core/OS/KeyCodes.h"

#include <GLFW/glfw3.h>

namespace Shapes
{
    namespace GLFWKeyCodes
    {
        static Shapes::InputCode::Key GLFWToShapesKeyboardKey(uint32_t glfwKey)
        {
            static std::map<uint32_t, Shapes::InputCode::Key> keyMap = {
                { GLFW_KEY_A, Shapes::InputCode::Key::A },
                { GLFW_KEY_B, Shapes::InputCode::Key::B },
                { GLFW_KEY_C, Shapes::InputCode::Key::C },
                { GLFW_KEY_D, Shapes::InputCode::Key::D },
                { GLFW_KEY_E, Shapes::InputCode::Key::E },
                { GLFW_KEY_F, Shapes::InputCode::Key::F },
                { GLFW_KEY_G, Shapes::InputCode::Key::G },
                { GLFW_KEY_H, Shapes::InputCode::Key::H },
                { GLFW_KEY_I, Shapes::InputCode::Key::I },
                { GLFW_KEY_J, Shapes::InputCode::Key::J },
                { GLFW_KEY_K, Shapes::InputCode::Key::K },
                { GLFW_KEY_L, Shapes::InputCode::Key::L },
                { GLFW_KEY_M, Shapes::InputCode::Key::M },
                { GLFW_KEY_N, Shapes::InputCode::Key::N },
                { GLFW_KEY_O, Shapes::InputCode::Key::O },
                { GLFW_KEY_P, Shapes::InputCode::Key::P },
                { GLFW_KEY_Q, Shapes::InputCode::Key::Q },
                { GLFW_KEY_R, Shapes::InputCode::Key::R },
                { GLFW_KEY_S, Shapes::InputCode::Key::S },
                { GLFW_KEY_T, Shapes::InputCode::Key::T },
                { GLFW_KEY_U, Shapes::InputCode::Key::U },
                { GLFW_KEY_V, Shapes::InputCode::Key::V },
                { GLFW_KEY_W, Shapes::InputCode::Key::W },
                { GLFW_KEY_X, Shapes::InputCode::Key::X },
                { GLFW_KEY_Y, Shapes::InputCode::Key::Y },
                { GLFW_KEY_Z, Shapes::InputCode::Key::Z },

                { GLFW_KEY_0, Shapes::InputCode::Key::D0 },
                { GLFW_KEY_1, Shapes::InputCode::Key::D1 },
                { GLFW_KEY_2, Shapes::InputCode::Key::D2 },
                { GLFW_KEY_3, Shapes::InputCode::Key::D3 },
                { GLFW_KEY_4, Shapes::InputCode::Key::D4 },
                { GLFW_KEY_5, Shapes::InputCode::Key::D5 },
                { GLFW_KEY_6, Shapes::InputCode::Key::D6 },
                { GLFW_KEY_7, Shapes::InputCode::Key::D7 },
                { GLFW_KEY_8, Shapes::InputCode::Key::D8 },
                { GLFW_KEY_9, Shapes::InputCode::Key::D9 },

                { GLFW_KEY_F1, Shapes::InputCode::Key::F1 },
                { GLFW_KEY_F2, Shapes::InputCode::Key::F2 },
                { GLFW_KEY_F3, Shapes::InputCode::Key::F3 },
                { GLFW_KEY_F4, Shapes::InputCode::Key::F4 },
                { GLFW_KEY_F5, Shapes::InputCode::Key::F5 },
                { GLFW_KEY_F6, Shapes::InputCode::Key::F6 },
                { GLFW_KEY_F7, Shapes::InputCode::Key::F7 },
                { GLFW_KEY_F8, Shapes::InputCode::Key::F8 },
                { GLFW_KEY_F9, Shapes::InputCode::Key::F9 },
                { GLFW_KEY_F10, Shapes::InputCode::Key::F10 },
                { GLFW_KEY_F11, Shapes::InputCode::Key::F11 },
                { GLFW_KEY_F12, Shapes::InputCode::Key::F12 },

                { GLFW_KEY_MINUS, Shapes::InputCode::Key::Minus },
                { GLFW_KEY_DELETE, Shapes::InputCode::Key::Delete },
                { GLFW_KEY_SPACE, Shapes::InputCode::Key::Space },
                { GLFW_KEY_LEFT, Shapes::InputCode::Key::Left },
                { GLFW_KEY_RIGHT, Shapes::InputCode::Key::Right },
                { GLFW_KEY_UP, Shapes::InputCode::Key::Up },
                { GLFW_KEY_DOWN, Shapes::InputCode::Key::Down },
                { GLFW_KEY_LEFT_SHIFT, Shapes::InputCode::Key::LeftShift },
                { GLFW_KEY_RIGHT_SHIFT, Shapes::InputCode::Key::RightShift },
                { GLFW_KEY_ESCAPE, Shapes::InputCode::Key::Escape },
                { GLFW_KEY_KP_ADD, Shapes::InputCode::Key::A },
                { GLFW_KEY_COMMA, Shapes::InputCode::Key::Comma },
                { GLFW_KEY_BACKSPACE, Shapes::InputCode::Key::Backspace },
                { GLFW_KEY_ENTER, Shapes::InputCode::Key::Enter },
                { GLFW_KEY_LEFT_SUPER, Shapes::InputCode::Key::LeftSuper },
                { GLFW_KEY_RIGHT_SUPER, Shapes::InputCode::Key::RightSuper },
                { GLFW_KEY_LEFT_ALT, Shapes::InputCode::Key::LeftAlt },
                { GLFW_KEY_RIGHT_ALT, Shapes::InputCode::Key::RightAlt },
                { GLFW_KEY_LEFT_CONTROL, Shapes::InputCode::Key::LeftControl },
                { GLFW_KEY_RIGHT_CONTROL, Shapes::InputCode::Key::RightControl },
                { GLFW_KEY_TAB, Shapes::InputCode::Key::Tab }
            };

            return keyMap[glfwKey];
        }

        static Shapes::InputCode::MouseKey GLFWToShapesMouseKey(uint32_t glfwKey)
        {

            static std::map<uint32_t, Shapes::InputCode::MouseKey> keyMap = {
                { GLFW_MOUSE_BUTTON_LEFT, Shapes::InputCode::MouseKey::ButtonLeft },
                { GLFW_MOUSE_BUTTON_RIGHT, Shapes::InputCode::MouseKey::ButtonRight },
                { GLFW_MOUSE_BUTTON_MIDDLE, Shapes::InputCode::MouseKey::ButtonMiddle }
            };

            return keyMap[glfwKey];
        }
    }
}
