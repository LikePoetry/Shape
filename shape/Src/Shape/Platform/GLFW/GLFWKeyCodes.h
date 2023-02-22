#pragma once

#include "Core/OS/KeyCodes.h"

#include <GLFW/glfw3.h>

#include <map>

namespace Shape
{
	namespace GLFWKeyCodes
	{
		static Shape::InputCode::Key GLFWToShapeKeyboardKey(uint32_t glfwKey)
		{
			static std::map<uint32_t, Shape::InputCode::Key> keyMap = {
				{ GLFW_KEY_A, Shape::InputCode::Key::A },
				{ GLFW_KEY_B, Shape::InputCode::Key::B },
				{ GLFW_KEY_C, Shape::InputCode::Key::C },
				{ GLFW_KEY_D, Shape::InputCode::Key::D },
				{ GLFW_KEY_E, Shape::InputCode::Key::E },
				{ GLFW_KEY_F, Shape::InputCode::Key::F },
				{ GLFW_KEY_G, Shape::InputCode::Key::G },
				{ GLFW_KEY_H, Shape::InputCode::Key::H },
				{ GLFW_KEY_I, Shape::InputCode::Key::I },
				{ GLFW_KEY_J, Shape::InputCode::Key::J },
				{ GLFW_KEY_K, Shape::InputCode::Key::K },
				{ GLFW_KEY_L, Shape::InputCode::Key::L },
				{ GLFW_KEY_M, Shape::InputCode::Key::M },
				{ GLFW_KEY_N, Shape::InputCode::Key::N },
				{ GLFW_KEY_O, Shape::InputCode::Key::O },
				{ GLFW_KEY_P, Shape::InputCode::Key::P },
				{ GLFW_KEY_Q, Shape::InputCode::Key::Q },
				{ GLFW_KEY_R, Shape::InputCode::Key::R },
				{ GLFW_KEY_S, Shape::InputCode::Key::S },
				{ GLFW_KEY_T, Shape::InputCode::Key::T },
				{ GLFW_KEY_U, Shape::InputCode::Key::U },
				{ GLFW_KEY_V, Shape::InputCode::Key::V },
				{ GLFW_KEY_W, Shape::InputCode::Key::W },
				{ GLFW_KEY_X, Shape::InputCode::Key::X },
				{ GLFW_KEY_Y, Shape::InputCode::Key::Y },
				{ GLFW_KEY_Z, Shape::InputCode::Key::Z },

				{ GLFW_KEY_0, Shape::InputCode::Key::D0 },
				{ GLFW_KEY_1, Shape::InputCode::Key::D1 },
				{ GLFW_KEY_2, Shape::InputCode::Key::D2 },
				{ GLFW_KEY_3, Shape::InputCode::Key::D3 },
				{ GLFW_KEY_4, Shape::InputCode::Key::D4 },
				{ GLFW_KEY_5, Shape::InputCode::Key::D5 },
				{ GLFW_KEY_6, Shape::InputCode::Key::D6 },
				{ GLFW_KEY_7, Shape::InputCode::Key::D7 },
				{ GLFW_KEY_8, Shape::InputCode::Key::D8 },
				{ GLFW_KEY_9, Shape::InputCode::Key::D9 },

				{ GLFW_KEY_F1, Shape::InputCode::Key::F1 },
				{ GLFW_KEY_F2, Shape::InputCode::Key::F2 },
				{ GLFW_KEY_F3, Shape::InputCode::Key::F3 },
				{ GLFW_KEY_F4, Shape::InputCode::Key::F4 },
				{ GLFW_KEY_F5, Shape::InputCode::Key::F5 },
				{ GLFW_KEY_F6, Shape::InputCode::Key::F6 },
				{ GLFW_KEY_F7, Shape::InputCode::Key::F7 },
				{ GLFW_KEY_F8, Shape::InputCode::Key::F8 },
				{ GLFW_KEY_F9, Shape::InputCode::Key::F9 },
				{ GLFW_KEY_F10, Shape::InputCode::Key::F10 },
				{ GLFW_KEY_F11, Shape::InputCode::Key::F11 },
				{ GLFW_KEY_F12, Shape::InputCode::Key::F12 },

				{ GLFW_KEY_MINUS, Shape::InputCode::Key::Minus },
				{ GLFW_KEY_DELETE, Shape::InputCode::Key::Delete },
				{ GLFW_KEY_SPACE, Shape::InputCode::Key::Space },
				{ GLFW_KEY_LEFT, Shape::InputCode::Key::Left },
				{ GLFW_KEY_RIGHT, Shape::InputCode::Key::Right },
				{ GLFW_KEY_UP, Shape::InputCode::Key::Up },
				{ GLFW_KEY_DOWN, Shape::InputCode::Key::Down },
				{ GLFW_KEY_LEFT_SHIFT, Shape::InputCode::Key::LeftShift },
				{ GLFW_KEY_RIGHT_SHIFT, Shape::InputCode::Key::RightShift },
				{ GLFW_KEY_ESCAPE, Shape::InputCode::Key::Escape },
				{ GLFW_KEY_KP_ADD, Shape::InputCode::Key::A },
				{ GLFW_KEY_COMMA, Shape::InputCode::Key::Comma },
				{ GLFW_KEY_BACKSPACE, Shape::InputCode::Key::Backspace },
				{ GLFW_KEY_ENTER, Shape::InputCode::Key::Enter },
				{ GLFW_KEY_LEFT_SUPER, Shape::InputCode::Key::LeftSuper },
				{ GLFW_KEY_RIGHT_SUPER, Shape::InputCode::Key::RightSuper },
				{ GLFW_KEY_LEFT_ALT, Shape::InputCode::Key::LeftAlt },
				{ GLFW_KEY_RIGHT_ALT, Shape::InputCode::Key::RightAlt },
				{ GLFW_KEY_LEFT_CONTROL, Shape::InputCode::Key::LeftControl },
				{ GLFW_KEY_RIGHT_CONTROL, Shape::InputCode::Key::RightControl },
				{ GLFW_KEY_TAB, Shape::InputCode::Key::Tab }
			};

			return keyMap[glfwKey];
		}

		static Shape::InputCode::MouseKey GLFWToShapeMouseKey(uint32_t glfwKey)
		{

			static std::map<uint32_t, Shape::InputCode::MouseKey> keyMap = {
				{ GLFW_MOUSE_BUTTON_LEFT, Shape::InputCode::MouseKey::ButtonLeft },
				{ GLFW_MOUSE_BUTTON_RIGHT, Shape::InputCode::MouseKey::ButtonRight },
				{ GLFW_MOUSE_BUTTON_MIDDLE, Shape::InputCode::MouseKey::ButtonMiddle }
			};

			return keyMap[glfwKey];
		}
	}
}
