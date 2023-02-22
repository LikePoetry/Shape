#pragma once

namespace Shape
{
    class AINode
    {
    public:
        AINode()          = default;
        virtual ~AINode() = default;

        void Update(float dt) {};
    };
}
