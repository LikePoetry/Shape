#include "hzpch.h"
#include "TextureMatrixComponent.h"

namespace Shape
{
    TextureMatrixComponent::TextureMatrixComponent(const glm::mat4& matrix)
        : m_TextureMatrix(matrix)
    {
    }

    void TextureMatrixComponent::OnImGui()
    {
    }
}
