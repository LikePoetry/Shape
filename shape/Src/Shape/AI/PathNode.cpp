#include "hzpch.h"
#include "PathNode.h"
#include "PathEdge.h"

namespace Shape
{

    PathNode::PathNode(const glm::vec3& position)
        : RigidBody3D()
    {
        SetPosition(position);
    }

    PathNode::~PathNode()
    {
    }

    bool PathNode::IsOnList(const std::vector<PathNode*>& list)
    {
        return std::find(list.begin(), list.end(), this) != list.end();
    }

    float PathNode::HeuristicValue(const PathNode& other) const
    {
        return (GetWorldSpaceTransform()[3] - other.GetWorldSpaceTransform()[3]).length();
    }

}