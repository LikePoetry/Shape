#pragma once
#include "Definitions.h"

namespace Shape
{
	namespace Graphics
	{
		struct DescriptorSetInfo
		{
			std::vector<Descriptor> descriptors;
		};

		class DescriptorSet
		{
		public:
			virtual ~DescriptorSet() = default;
			static DescriptorSet* Create(const DescriptorDesc& desc);
		};
	}
}