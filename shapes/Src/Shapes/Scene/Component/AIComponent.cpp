#include "Precompiled.h"
#include "AIComponent.h"

#include <imgui/imgui.h>

namespace Shapes
{
    AIComponent::AIComponent()
    {
        m_AINode = CreateSharedPtr<AINode>();
    }

    AIComponent::AIComponent(SharedPtr<AINode>& aiNode)
        : m_AINode(aiNode)
    {
    }

    void AIComponent::OnImGui()
    {
    }

}
