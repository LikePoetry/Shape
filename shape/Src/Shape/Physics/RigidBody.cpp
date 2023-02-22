#include "hzpch.h"
#include "RigidBody.h"

namespace Shape
{

    RigidBody::RigidBody()
        : m_Static(false)
        , m_Elasticity(0.9f)
        , m_Friction(0.5f)
        , m_AtRest(false)
        , m_UUID(UUID())
    {
    }

    RigidBody::~RigidBody()
    {
    }
}
