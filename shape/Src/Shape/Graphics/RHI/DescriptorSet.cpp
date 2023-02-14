#include "hzpch.h"
#include "DescriptorSet.h"

namespace Shape
{
	namespace Graphics
	{
        DescriptorSet* (*DescriptorSet::CreateFunc)(const DescriptorDesc&) = nullptr;

        DescriptorSet* DescriptorSet::Create(const DescriptorDesc& desc)
        {
            SHAPE_ASSERT(CreateFunc, "No DescriptorSet Create Function");

            return CreateFunc(desc);
        }
	}
}