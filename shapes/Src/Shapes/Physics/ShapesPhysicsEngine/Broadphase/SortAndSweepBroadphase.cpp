
#include "Precompiled.h"
#include "SortAndSweepBroadphase.h"

namespace Shapes
{

    SortAndSweepBroadphase::SortAndSweepBroadphase(const glm::vec3& axis)
        : Broadphase()
        , m_AxisIndex(0)
    {
        SetAxis(axis);
    }

    SortAndSweepBroadphase::~SortAndSweepBroadphase()
    {
    }

    void SortAndSweepBroadphase::SetAxis(const glm::vec3& axis)
    {
        SHAPES_PROFILE_FUNCTION();
        // Determine axis
        m_Axis = axis;
        glm::normalize(m_Axis);

        if(abs(m_Axis.x) > 0.9f)
            m_AxisIndex = 0;
        else if(abs(m_Axis.y) > 0.9f)
            m_AxisIndex = 1;
        else if(abs(m_Axis.z) > 0.9f)
            m_AxisIndex = 2;
    }

    void SortAndSweepBroadphase::FindPotentialCollisionPairs(RigidBody3D** objects, uint32_t objectCount,
                                                             std::vector<CollisionPair>& collisionPairs)
    {
        SHAPES_PROFILE_FUNCTION();
        // Sort entities along axis
        std::sort(objects, objects + objectCount, [this](RigidBody3D* a, RigidBody3D* b) -> bool
                  { return a->GetWorldSpaceAABB().Min()[this->m_AxisIndex] < b->GetWorldSpaceAABB().Min()[this->m_AxisIndex]; });

        for(uint32_t i = 0; i < objectCount; i++)
        {
            auto& obj = *objects[i];

            float thisBoxRight = obj.GetWorldSpaceAABB().Max()[m_AxisIndex];

            for(uint32_t iit = i + 1; iit < objectCount; iit++)
            {
                auto& obj2 = *objects[iit];
                // Skip pairs of two at rest/static objects
                if(obj.GetIsAtRest() && obj2.GetIsAtRest())
                    continue;

                // Skip pairs of two at static objects
                if(obj.GetIsStatic() && obj2.GetIsStatic())
                    continue;

                // Skip pairs of one static and one at rest
                if(obj.GetIsAtRest() && obj2.GetIsStatic())
                    continue;

                if(obj.GetIsStatic() && obj2.GetIsAtRest())
                    continue;

                float testBoxLeft = obj2.GetWorldSpaceAABB().Min()[m_AxisIndex];

                // Test for overlap between the axis values of the bounding boxes
                if(testBoxLeft < thisBoxRight)
                {
                    CollisionPair cp;
                    cp.pObjectA = &obj;
                    cp.pObjectB = &obj2;

                    collisionPairs.push_back(cp);
                }
            }
        }
    }

    void SortAndSweepBroadphase::DebugDraw()
    {
    }
}
