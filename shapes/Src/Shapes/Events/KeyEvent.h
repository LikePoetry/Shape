#pragma once

#include "Event.h"
#include "Core/OS/KeyCodes.h"

#include <sstream>

namespace Shapes
{

    class SHAPES_EXPORT KeyEvent : public Event
    {
    public:
        inline Shapes::InputCode::Key GetKeyCode() const { return m_KeyCode; }

        EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

    protected:
        KeyEvent(Shapes::InputCode::Key keycode)
            : m_KeyCode(keycode)
        {
        }

        Shapes::InputCode::Key m_KeyCode;
    };

    class SHAPES_EXPORT KeyPressedEvent : public KeyEvent
    {
    public:
        KeyPressedEvent(Shapes::InputCode::Key keycode, int repeatCount)
            : KeyEvent(keycode)
            , m_RepeatCount(repeatCount)
        {
        }

        inline int GetRepeatCount() const { return m_RepeatCount; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "KeyPressedEvent: " << uint32_t(m_KeyCode) << " (" << m_RepeatCount << " repeats)";
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyPressed)
    private:
        int m_RepeatCount;
    };

    class SHAPES_EXPORT KeyReleasedEvent : public KeyEvent
    {
    public:
        KeyReleasedEvent(Shapes::InputCode::Key keycode)
            : KeyEvent(keycode)
        {
        }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "KeyReleasedEvent: " << uint32_t(m_KeyCode);
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyReleased)
    };

    class SHAPES_EXPORT KeyTypedEvent : public KeyEvent
    {
    public:
        KeyTypedEvent(Shapes::InputCode::Key keycode, char character)
            : KeyEvent(keycode)
            , Character(character)
        {
        }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "KeyPressedEvent: " << uint32_t(m_KeyCode);
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyTyped)

        char Character;

    private:
    };
}
