#pragma once
namespace Shape
{
	namespace Graphics
	{
		class CommandBuffer;
		class DescriptorSet;
		class Pipeline;
		class UniformBuffer;
		class GraphicsContext;
		class Texture;
		class Texture2D;
		class TextureCube;
		class TextureDepth;
		class TextureDepthArray;

		static constexpr uint8_t MAX_RENDER_TARGETS = 8;
		static constexpr uint8_t SHADOWMAP_MAX = 16;
		static constexpr uint8_t MAX_MIPS = 32;

		enum class CullMode
		{
			FRONT,
			BACK,
			FRONTANDBACK,
			NONE
		};

		enum class PolygonMode
		{
			FILL,
			LINE,
			POINT
		};

		enum class BlendMode
		{
			None = 0,
			OneZero,
			ZeroSrcColor,
			SrcAlphaOneMinusSrcAlpha,
		};

		enum class DrawType
		{
			POINT,
			TRIANGLE,
			LINES
		};

		enum class PhysicalDeviceType
		{
			DISCRETE = 0,
			INTEGRATED = 1,
			VIRTUAL = 2,
			CPU = 3,
			UNKNOWN = 4
		};

		enum class ShaderType : int
		{
			VERTEX,
			FRAGMENT,
			GEOMETRY,
			TESSELLATION_CONTROL,
			TESSELLATION_EVALUATION,
			COMPUTE,
			UNKNOWN
		};

		enum class TextureType
		{
			COLOUR,
			DEPTH,
			DEPTHARRAY,
			CUBE,
			OTHER
		};

		enum class TextureWrap
		{
			NONE,
			REPEAT,
			CLAMP,
			MIRRORED_REPEAT,
			CLAMP_TO_EDGE,
			CLAMP_TO_BORDER
		};

		enum class TextureFilter
		{
			NONE,
			LINEAR,
			NEAREST
		};

		enum class RHIFormat : uint32_t
		{
			NONE = 0,
			R8_Unorm,
			R8G8_Unorm,
			R8G8B8_Unorm,
			R8G8B8A8_Unorm,

			R8_UInt,

			R11G11B10_Float,
			R10G10B10A2_Unorm,

			R32_Int,
			R32G32_Int,
			R32G32B32_Int,
			R32G32B32A32_Int,

			R32_UInt,
			R32G32_UInt,
			R32G32B32_UInt,
			R32G32B32A32_UInt,

			R16_Float,
			R16G16_Float,
			R16G16B16_Float,
			R16G16B16A16_Float,

			R32_Float,
			R32G32_Float,
			R32G32B32_Float,
			R32G32B32A32_Float,

			D16_Unorm,
			D32_Float,
			D16_Unorm_S8_UInt,
			D24_Unorm_S8_UInt,
			D32_Float_S8_UInt,
			SCREEN
		};

		enum class DescriptorType
		{
			UNIFORM_BUFFER,
			UNIFORM_BUFFER_DYNAMIC,
			IMAGE_SAMPLER,
			IMAGE_STORAGE
		};

		enum TextureFlags : uint32_t
		{
			Texture_Sampled = BIT(0),
			Texture_Storage = BIT(1),
			Texture_RenderTarget = BIT(2),
			Texture_DepthStencil = BIT(3),
			Texture_DepthStencilReadOnly = BIT(4),
			Texture_CreateMips = BIT(5),
			Texture_MipViews = BIT(6)
		};

		struct TextureDesc
		{
			RHIFormat format;
			TextureFilter minFilter;
			TextureFilter magFilter;
			TextureWrap wrap;
			uint16_t msaaLevel = 1;
			uint16_t flags = TextureFlags::Texture_CreateMips;
			bool srgb = false;
			bool generateMipMaps = true;

			TextureDesc()
			{
				format = RHIFormat::R8G8B8A8_Unorm;
				minFilter = TextureFilter::NEAREST;
				magFilter = TextureFilter::NEAREST;
				wrap = TextureWrap::REPEAT;
				msaaLevel = 1;
			}

			TextureDesc(RHIFormat format, TextureFilter minFilter, TextureFilter magFilter, TextureWrap wrap)
				: format(format)
				, minFilter(minFilter)
				, magFilter(magFilter)
				, wrap(wrap)
			{
			}

			TextureDesc(TextureFilter minFilter, TextureFilter magFilter)
				: format(RHIFormat::R8G8B8A8_Unorm)
				, minFilter(minFilter)
				, magFilter(magFilter)
				, wrap(TextureWrap::CLAMP)
			{
			}

			TextureDesc(TextureFilter minFilter, TextureFilter magFilter, TextureWrap wrap)
				: format(RHIFormat::R8G8B8A8_Unorm)
				, minFilter(minFilter)
				, magFilter(magFilter)
				, wrap(wrap)
			{
			}

			TextureDesc(TextureWrap wrap)
				: format(RHIFormat::R8G8B8A8_Unorm)
				, minFilter(TextureFilter::LINEAR)
				, magFilter(TextureFilter::LINEAR)
				, wrap(wrap)
			{
			}

			TextureDesc(RHIFormat format)
				: format(format)
				, minFilter(TextureFilter::LINEAR)
				, magFilter(TextureFilter::LINEAR)
				, wrap(TextureWrap::CLAMP)
			{
			}
		};

		struct TextureLoadOptions
		{
			bool flipX;
			bool flipY;

			TextureLoadOptions()
			{
				flipX = false;
				flipY = false;
			}

			TextureLoadOptions(bool flipX, bool flipY)
				: flipX(flipX)
				, flipY(flipY)
			{
			}
		};

		enum class ShaderDataType
		{
			NONE,
			FLOAT32,
			VEC2,
			VEC3,
			VEC4,
			IVEC2,
			IVEC3,
			IVEC4,
			MAT3,
			MAT4,
			INT32,
			INT,
			UINT,
			BOOL,
			STRUCT,
			MAT4ARRAY
		};

		struct BufferMemberInfo
		{
			uint32_t size;
			uint32_t offset;
			ShaderDataType type;
			std::string name;
			std::string fullName;
		};

		struct DescriptorDesc
		{
			uint32_t layoutIndex;
			Shader* shader;
			uint32_t count = 1;
		};

		struct Descriptor
		{
			Texture** textures;
			Texture* texture;
			UniformBuffer* buffer;

			uint32_t offset;
			uint32_t size;
			uint32_t binding;
			uint32_t textureCount = 1;
			uint32_t mipLevel = 0;
			std::string name;

			TextureType textureType;
			DescriptorType type = DescriptorType::IMAGE_SAMPLER;
			ShaderType shaderType;

			std::vector<BufferMemberInfo> m_Members;
		};

		struct PushConstant
		{
			uint32_t size;
			ShaderType shaderStage;
			uint8_t* data;
			uint32_t offset = 0;
			std::string name;

			std::vector<BufferMemberInfo> m_Members;

			void SetValue(const std::string& name, void* value)
			{
				for (auto& member : m_Members)
				{
					if (member.name == name)
					{
						memcpy(&data[member.offset], value, member.size);
						break;
					}
				}
			}

			void SetData(void* value)
			{
				memcpy(data, value, size);
			}
		};
	}
}

