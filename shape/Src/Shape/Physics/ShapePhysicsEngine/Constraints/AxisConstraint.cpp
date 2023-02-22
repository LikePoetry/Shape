#include "hzpch.h"
#include "AxisConstraint.h"
#include "Maths/Maths.h"
#include "Physics/ShapePhysicsEngine/RigidBody3D.h"
#include "Graphics/Renderers/DebugRenderer.h"

namespace Shape
{

    AxisConstraint::AxisConstraint(RigidBody3D* obj1, Axes axes)
        : m_pObj1(obj1)
        , m_Axes(axes)
    {
    }

    void AxisConstraint::ApplyImpulse()
    {
        SHAPE_PROFILE_FUNCTION();

        auto velocity = m_pObj1->GetAngularVelocity();

        switch(m_Axes)
        {
        case Axes::X:
            velocity.x = 0.0f;
            break;
        case Axes::Y:
            velocity.y = 0.0f;
            break;
        case Axes::Z:
            velocity.z = 0.0f;
            break;
        case Axes::XZ:
            velocity.x = 0.0f;
            velocity.z = 0.0f;
            break;
        case Axes::XY:
            velocity.x = 0.0f;
            velocity.y = 0.0f;
            break;
        case Axes::YZ:
            velocity.y = 0.0f;
            velocity.z = 0.0f;
            break;
        case Axes::XYZ:
            velocity.x = 0.0f;
            velocity.y = 0.0f;
            velocity.z = 0.0f;
            break;
        default:
            break;
        }

        m_pObj1->SetAngularVelocity(velocity);
    }

    void AxisConstraint::DebugDraw() const
    {
    }
}
