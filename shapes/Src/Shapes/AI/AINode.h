#pragma once

namespace Shapes
{
    class AINode
    {
    public:
        AINode()          = default;
        virtual ~AINode() = default;

        void Update(float dt) {};
    };
}
