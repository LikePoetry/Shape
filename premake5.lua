function SetRecommendedSettings()
	filter ()
	editandcontinue "Off"
	warnings "Off"
end

IncludeDir = {}
IncludeDir["spdlog"] = "Shapes/External/spdlog/include"
IncludeDir["external"] = "Shapes/External"
IncludeDir["glm"] = "Shapes/External/glm"
IncludeDir["GLFW"] = "Shapes/External/glfw/include/"
IncludeDir["ImGui"] = "Shapes/External/imgui/"
IncludeDir["vulkan"] = "Shapes/External/vulkan/"
IncludeDir["stb"] = "Shapes/External/stb"
IncludeDir["SpirvCross"] = "Shapes/External/vulkan/SPIRV-Cross"
IncludeDir["cereal"] = "Shapes/External/cereal/include"
--字体相关的库
IncludeDir["msdf_atlas_gen"] = "Shapes/External/msdf-atlas-gen/msdf-atlas-gen"
IncludeDir["msdfgen"] = "Shapes/External/msdf-atlas-gen/msdfgen"
--IMGui 相关
IncludeDir["freetype"] = "Shapes/External/freetype/include"

IncludeDir["cereal"] = "Shapes/External/cereal/include"
-- 物理引擎相关
IncludeDir["Box2D"] = "Shapes/External/box2d/include/"
-- 脚本控制相关
IncludeDir["lua"] = "Shapes/External/lua/src/"
-- Audio 相关库
IncludeDir["OpenAL"] = "Shapes/External/OpenAL/include/"

workspace "Shapes"
    architecture "x64"
    startproject "Editor"

    configurations{
        "Debug"
    }

    outputdir="%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "External"
	require("Shapes/External/spdlog/premake5")
		SetRecommendedSettings()
	require("Shapes/External/GLFWpremake5")
		SetRecommendedSettings()	
	require("Shapes/External/imguipremake5")
		SetRecommendedSettings()
	require("Shapes/External/SPIRVCrosspremake5")
		SetRecommendedSettings()
	require("Shapes/External/ModelLoaders/meshoptimizer/premake5")
		SetRecommendedSettings()
	require("Shapes/External/msdf-atlas-gen/premake5")
		SetRecommendedSettings()
	require("Shapes/External/freetype/premake5")
		SetRecommendedSettings()
	require("Shapes/External/box2dpremake5")
			SetRecommendedSettings()
	require("Shapes/External/lua/premake5")
			SetRecommendedSettings()



group ""


--静态链接库，核心文件
project "Shapes"
	location "Shapes"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	editandcontinue "Off"

	targetdir("bin/" ..outputdir.. "/%{prj.name}")
	objdir("bin-int/" ..outputdir.. "/%{prj.name}")

	files
	{
		"%{prj.name}/Src/**.h",
		"%{prj.name}/Src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/Src",
		"%{prj.name}/Src/Shapes",
		"%{prj.name}/Assets/Shaders",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.external}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.vulkan}",
		"%{IncludeDir.stb}",
		"%{IncludeDir.SpirvCross}",
		"%{IncludeDir.msdfgen}",
		"%{IncludeDir.msdf_atlas_gen}",
		"%{IncludeDir.freetype}",
		"%{IncludeDir.cereal}",
		"%{IncludeDir.Box2D}",
		"%{IncludeDir.lua}",
		"%{IncludeDir.OpenAL}",

	}

	libdirs 
	{ 
		"spdlog",
		"SpirvCross",
	}

	defines
	{
		"GLM_FORCE_INTRINSICS",
		"GLM_FORCE_DEPTH_ZERO_TO_ONE"
	}

	filter 'architecture:x86_64'
		defines { "USE_VMA_ALLOCATOR"}

	filter { "files:Shapes/External/**"}
		warnings "Off"

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "on"
		systemversion "latest"
		disablewarnings { 4307 }
		characterset ("Unicode")

		pchheader "Precompiled.h"
		pchsource "Shapes/Src/Precompiled.cpp"

		defines
		{
			"SHAPES_ENGINE",
			"FREEIMAGE_LIB",
			"IMGUI_USER_CONFIG=\"../Src/Shapes/ImGui/ImConfig.h\"",
			"SPDLOG_COMPILED_LIB",
			"GLM_FORCE_INTRINSICS",
			"GLM_FORCE_DEPTH_ZERO_TO_ONE",
			"SHAPES_PLATFORM_WINDOWS",
			"SHAPES_RENDER_API_VULKAN",
			"VK_USE_PLATFORM_WIN32_KHR",
			"WIN32_LEAN_AND_MEAN",
			"_CRT_SECURE_NO_WARNINGS",
			"_DISABLE_EXTENDED_ALIGNED_STORAGE",
			"_SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING",
			"_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING",
			"SHAPES_IMGUI",
			"SHAPES_OPENAL",
			"SHAPES_VOLK",
			"SHAPES_USE_GLFW_WINDOWS"
		}
		
		links
		{
			"glfw",
			"imgui",
			"meshoptimizer",
			"msdf-atlas-gen",
			"freetype",
			"box2d",
			"lua",

		}

		buildoptions
		{
		"/MP", "/bigobj"
		}

		filter 'files:Shapes/External/**.cpp'
			flags  { 'NoPCH' }
		filter 'files:Shapes/External/**.c'
			flags  { 'NoPCH' }


	disablewarnings { 4307 }

	filter "configurations:Debug"
		defines {"TRACY_ENABLE","TRACY_ON_DEMAND"}
		runtime "Debug"
		symbols "on"
		optimize "Off"

-- 测试工具项目
project "Editor"
    location "Editor"
	kind "ConsoleApp"
	language "C++"
	editandcontinue "Off"

	targetdir("bin/" ..outputdir.. "/%{prj.name}")
	objdir("bin-int/" ..outputdir.. "/%{prj.name}")
	

	files
	{
		"%{prj.name}/Src/**.h",
		"%{prj.name}/Src/**.cpp"
    }

    includedirs
	{
        "Shapes/Src/Shapes",
        "Shapes/Src",

		"%{IncludeDir.spdlog}",
		"%{IncludeDir.external}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.vulkan}",
		"%{IncludeDir.stb}",
		"%{IncludeDir.SpirvCross}",
		"%{IncludeDir.msdfgen}",
		"%{IncludeDir.msdf_atlas_gen}",
		"%{IncludeDir.freetype}",
		"%{IncludeDir.cereal}",
		"%{IncludeDir.Box2D}",
		"%{IncludeDir.lua}",
		"%{IncludeDir.OpenAL}",
    }

    links
    {
        "Shapes",
		"glfw",
		"imgui",
		"SpirvCross",
		"meshoptimizer",
		"msdf-atlas-gen",
		"freetype",
		"box2d",
		"lua",
    }

	defines
	{
		"IMGUI_USER_CONFIG=\"../../Shapes/Src/Shapes/ImGui/ImConfig.h\"",
		"GLM_FORCE_INTRINSICS",
		"GLM_FORCE_DEPTH_ZERO_TO_ONE",
		"SHAPES_ENGINE",
		"SHAPES_PLATFORM_WINDOWS",
		"SHAPES_RENDER_API_VULKAN",
		"VK_USE_PLATFORM_WIN32_KHR",
	}

	filter { "files:Shapes/External/**"}
	warnings "Off"

filter 'architecture:x86_64'
	defines { "USE_VMA_ALLOCATOR"}

filter "system:windows"
	cppdialect "C++17"
	staticruntime "On"
	systemversion "latest"
	conformancemode "off"

	defines
		{
			"WIN32_LEAN_AND_MEAN",
		}

	disablewarnings { 4307 }
filter "configurations:Debug"
		defines {"TRACY_ENABLE","TRACY_ON_DEMAND"}
		symbols "On"
		runtime "Debug"
		optimize "Off"
		