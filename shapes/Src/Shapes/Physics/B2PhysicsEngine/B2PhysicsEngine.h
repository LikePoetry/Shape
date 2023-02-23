#pragma once

#include "Utilities/TSingleton.h"
#include "Scene/ISystem.h"

class b2World;
class b2Body;
struct b2BodyDef;
struct b2FixtureDef;
class b2ContactListener;

namespace Shapes
{
    class TimeStep;
    class B2DebugDraw;

    class SHAPES_EXPORT B2PhysicsEngine : public ISystem
    {
        friend class TSingleton<B2PhysicsEngine>;

    public:
        B2PhysicsEngine();
        ~B2PhysicsEngine();
        void SetDefaults();

        void OnUpdate(const TimeStep& timeStep, Scene* scene) override;
        void OnInit() override {};
        void OnImGui() override;

        b2World* GetB2World() const
        {
            return m_B2DWorld.get();
        }
        b2Body* CreateB2Body(b2BodyDef* bodyDef) const;

        static void CreateFixture(b2Body* body, const b2FixtureDef* fixtureDef);

        void SetPaused(bool paused)
        {
            m_Paused = paused;
        }
        bool IsPaused() const
        {
            return m_Paused;
        }

        void OnDebugDraw() override;

        uint32_t GetDebugDrawFlags();
        void SetDebugDrawFlags(uint32_t flags);
        void SetGravity(const glm::vec2& gravity);

        void SetContactListener(b2ContactListener* listener);
        void SyncTransforms(Scene* scene);

    private:
        UniquePtr<b2World> m_B2DWorld;
        UniquePtr<B2DebugDraw> m_DebugDraw;

        float m_UpdateTimestep;
        bool m_Paused = true;

        int32_t m_VelocityIterations = 6;
        int32_t m_PositionIterations = 2;

        b2ContactListener* m_Listener;
    };
}
