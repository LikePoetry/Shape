#pragma once

#include "Graphics/Mesh.h"
#include "Graphics/RHI/Shader.h"

namespace Shapes
{

    namespace Graphics
    {
        class Material;
        class Pipeline;

        struct SHAPES_EXPORT RenderCommand
        {
            Mesh* mesh         = nullptr;
            Material* material = nullptr;
            Pipeline* pipeline = nullptr;
            glm::mat4 transform;
            glm::mat4 textureMatrix;
            bool animated = false;
        };
    }
}
