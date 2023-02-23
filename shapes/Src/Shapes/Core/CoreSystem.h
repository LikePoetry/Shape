#pragma once

#include "Core/Core.h"

namespace Shapes
{
    namespace Internal
    {
        // Low-level System operations
        class SHAPES_EXPORT CoreSystem
        {
        public:
            static bool Init(int argc = 0, char** argv = nullptr);
            static void Shutdown();
        };

    }

};
