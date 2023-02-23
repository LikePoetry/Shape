#pragma once
#include "Event.h"

#include <sstream>

namespace Shapes
{

    class SHAPES_EXPORT MouseMovedEvent : public Event
    {
    public:
        MouseMovedEvent(float x, float y)
            : m_MouseX(x)
            , m_MouseY(y)
        {
        }

        inline float GetX() const { return m_MouseX; }
        inline float GetY() const { return m_MouseY; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "MouseMovedEvent: " << m_MouseX << ", " << m_MouseY;
            return ss.str();
        }

        EVENT_CLASS_TYPE(MouseMoved)
        EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

    private:
        float m_MouseX, m_MouseY;
    };

    class SHAPES_EXPORT MouseScrolledEvent : public Event
    {
    public:
        MouseScrolledEvent(float xOffset, float yOffset)
            : m_XOffset(xOffset)
            , m_YOffset(yOffset)
        {
        }

        inline float GetXOffset() const { return m_XOffset; }
        inline float GetYOffset() const { return m_YOffset; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "MouseScrolledEvent: " << GetXOffset() << ", " << GetYOffset();
            return ss.str();
        }

        EVENT_CLASS_TYPE(MouseScrolled)
        EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

    private:
        float m_XOffset, m_YOffset;
    };

    class SHAPES_EXPORT MouseButtonEvent : public Event
    {
    public:
        inline Shapes::InputCode::MouseKey GetMouseButton() const
        {
            return m_Button;
        }

        EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

    protected:
        MouseButtonEvent(Shapes::InputCode::MouseKey button)
            : m_Button(button)
        {
        }

        Shapes::InputCode::MouseKey m_Button;
    };

    class SHAPES_EXPORT MouseButtonPressedEvent : public MouseButtonEvent
    {
    public:
        MouseButtonPressedEvent(Shapes::InputCode::MouseKey button)
            : MouseButtonEvent(button)
        {
        }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "MouseButtonPressedEvent: " << m_Button;
            return ss.str();
        }

        EVENT_CLASS_TYPE(MouseButtonPressed)
    };

    class SHAPES_EXPORT MouseButtonReleasedEvent : public MouseButtonEvent
    {
    public:
        MouseButtonReleasedEvent(Shapes::InputCode::MouseKey button)
            : MouseButtonEvent(button)
        {
        }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "MouseButtonReleasedEvent: " << m_Button;
            return ss.str();
        }

        EVENT_CLASS_TYPE(MouseButtonReleased)
    };

    class SHAPES_EXPORT MouseEnterEvent : public Event
    {
    public:
        MouseEnterEvent(bool enter)
            : m_Entered(enter)
        {
        }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "MouseEnterEvent: " << m_Entered;
            return ss.str();
        }

        EVENT_CLASS_TYPE(MouseEntered)
        EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

        inline bool GetEntered() const { return m_Entered; }

    protected:
        bool m_Entered;
    };

}
