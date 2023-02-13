#pragma once
#include "DescriptorSet.h"
#include "Definitions.h"
#include "Shape/Core/Profiler.h"

namespace spirv_cross
{
	struct SPIRType;
}

namespace Shape
{
	namespace Graphics
	{
		class SHAPE_EXPORT Shader
		{
		public:
			static const Shader* s_CurrentlyBound;

		public:
			virtual void Bind() const = 0;
			virtual void Unbind() const = 0;

			virtual ~Shader() = default;

			virtual const std::vector<ShaderType> GetShaderType() const = 0;
			virtual const std::string& GetName() const = 0;
			virtual const std::string& GetFilePath() const = 0;

			virtual void* GetHandle() const = 0;
			virtual bool IsCompiled() const { return true; }


		};
	}
}

