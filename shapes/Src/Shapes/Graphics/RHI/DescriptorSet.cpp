#include "Precompiled.h"
#include "DescriptorSet.h"

namespace Shapes
{
    namespace Graphics
    {
        DescriptorSet* (*DescriptorSet::CreateFunc)(const DescriptorDesc&) = nullptr;

        DescriptorSet* DescriptorSet::Create(const DescriptorDesc& desc)
        {
            SHAPES_ASSERT(CreateFunc, "No DescriptorSet Create Function");

            return CreateFunc(desc);
        }
    }
}
