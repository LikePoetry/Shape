#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "Core/UUID.h"

namespace Shape
{

    enum class SHAPE_EXPORT Shape
    {
        Square = 0,
        Circle = 1,
        Custom = 2
    };

    struct SHAPE_EXPORT RigidBodyParameters
    {
        RigidBodyParameters()
        {
            mass     = 1.0f;
            shape    = Shape::Square;
            position = glm::vec3(0.0f);
            scale    = glm::vec3(1.0f);
            isStatic = false;
        }

        float mass;
        glm::vec3 position;
        glm::vec3 scale;
        bool isStatic;
        Shape shape;
        std::vector<glm::vec2> custumShapePositions;
    };

    class SHAPE_EXPORT RigidBody
    {

    public:
        RigidBody();

        virtual ~RigidBody();

        bool GetIsStatic() const { return m_Static; }
        bool GetIsAtRest() const { return m_AtRest; }
        float GetElasticity() const { return m_Elasticity; }
        float GetFriction() const { return m_Friction; }
        bool IsAwake() const { return !m_AtRest; }
        void SetElasticity(const float elasticity) { m_Elasticity = elasticity; }
        void SetFriction(const float friction) { m_Friction = friction; }

        virtual void SetIsStatic(const bool isStatic) { m_Static = isStatic; }
        virtual void SetIsAtRest(const bool isAtRest) { m_AtRest = isAtRest; }
        virtual void WakeUp() { m_AtRest = false; }

        inline bool operator<(const RigidBody& p_r) const { return false; }
        inline bool operator==(const RigidBody& p_r) const { return false; }

        UUID GetUUID() const { return m_UUID; }

    protected:
        bool m_Static;
        float m_Elasticity;
        float m_Friction;
        bool m_AtRest;
        UUID m_UUID;
    };
}
