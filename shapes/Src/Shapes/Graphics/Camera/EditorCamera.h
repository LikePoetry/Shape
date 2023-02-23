#pragma once
#include "Graphics/Camera/CameraController.h"

namespace Shapes
{
    class SHAPES_EXPORT EditorCameraController : public CameraController
    {
    public:
        EditorCameraController();
        ~EditorCameraController();

        virtual void HandleMouse(Maths::Transform& transform, float dt, float xpos, float ypos) override;
        virtual void HandleKeyboard(Maths::Transform& transform, float dt) override;

        void UpdateScroll(Maths::Transform& transform, float offset, float dt) override;

        void SetMode(bool is2D) { m_2DMode = is2D; }
        void StopMovement();
        void SetSpeed(float speed) { m_CameraSpeed = speed; }

    private:
        bool m_2DMode = false;
        glm::vec2 m_StoredCursorPos;
        float m_CameraSpeed = 20.0f;
    };
}
