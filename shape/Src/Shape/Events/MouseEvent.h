#pragma once
#include "Event.h"
#include <sstream>

namespace Shape
{
	class SHAPE_EXPORT MouseMovedEvent : public Event
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

    class SHAPE_EXPORT MouseScrolledEvent : public Event
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

    class SHAPE_EXPORT MouseButtonEvent : public Event
    {
    public:
        inline Shape::InputCode::MouseKey GetMouseButton() const
        {
            return m_Button;
        }

        EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

    protected:
        MouseButtonEvent(Shape::InputCode::MouseKey button)
            : m_Button(button)
        {
        }

        Shape::InputCode::MouseKey m_Button;
    };

    class SHAPE_EXPORT MouseButtonPressedEvent : public MouseButtonEvent
    {
    public:
        MouseButtonPressedEvent(Shape::InputCode::MouseKey button)
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

    class SHAPE_EXPORT MouseButtonReleasedEvent : public MouseButtonEvent
    {
    public:
        MouseButtonReleasedEvent(Shape::InputCode::MouseKey button)
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

    class SHAPE_EXPORT MouseEnterEvent : public Event
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