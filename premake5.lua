function SetRecommendedSettings()
	filter ()
	editandcontinue "Off"
	warnings "Off"
end

IncludeDir = {}
IncludeDir["spdlog"] = "Shape/External/spdlog/include"
IncludeDir["external"] = "Shape/External"
IncludeDir["glm"] = "Shape/External/glm"
IncludeDir["GLFW"] = "Shape/External/glfw/include/"
IncludeDir["ImGui"] = "Shape/External/imgui/"
IncludeDir["vulkan"] = "Shape/External/vulkan/"
IncludeDir["stb"] = "Shape/External/stb"
IncludeDir["SpirvCross"] = "Shape/External/vulkan/SPIRV-Cross"
IncludeDir["cereal"] = "Shape/External/cereal/include"
--字体相关的库
IncludeDir["msdf_atlas_gen"] = "Shape/External/msdf-atlas-gen/msdf-atlas-gen"
IncludeDir["msdfgen"] = "Shape/External/msdf-atlas-gen/msdfgen"
--IMGui 相关
IncludeDir["freetype"] = "Shape/External/freetype/include"

IncludeDir["cereal"] = "Shape/External/cereal/include"






workspace "Shape"
    architecture "x64"
    startproject "Editor"

    configurations{
        "Debug"
    }

    outputdir="%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "External"
	require("Shape/External/spdlog/premake5")
		SetRecommendedSettings()
	require("Shape/External/GLFWpremake5")
		SetRecommendedSettings()	
	require("Shape/External/imguipremake5")
		SetRecommendedSettings()
	require("Shape/External/SPIRVCrosspremake5")
		SetRecommendedSettings()
	require("Shape/External/ModelLoaders/meshoptimizer/premake5")
		SetRecommendedSettings()
	require("Shape/External/msdf-atlas-gen/premake5")
		SetRecommendedSettings()
	require("Shape/External/freetype/premake5")
		SetRecommendedSettings()



group ""


--静态链接库，核心文件
project "Shape"
	location "Shape"
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

	filter { "files:Shaper/External/**"}
		warnings "Off"

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "on"
		systemversion "latest"
		disablewarnings { 4307 }
		characterset ("Unicode")

		pchheader "hzpch.h"
		pchsource "Shape/Src/hzpch.cpp"

		defines
		{
			"IMGUI_USER_CONFIG=\"../Src/Shape/ImGui/ImConfig.h\"",
			"_CRT_SECURE_NO_WARNINGS",
			"_DISABLE_EXTENDED_ALIGNED_STORAGE",
			"SHAPE_ENGINE"
		}
		
		links
		{
			"glfw",
			"imgui",
			"meshoptimizer",
			"msdf-atlas-gen",
			"freetype",

		}

		buildoptions
		{
		"/MP", "/bigobj"
		}

		filter 'files:Shape/External/**.cpp'
			flags  { 'NoPCH' }
		filter 'files:Shape/External/**.c'
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
        "Shape/Src",
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
    }

    links
    {
        "Shape",
		"glfw",
		"imgui",
		"SpirvCross",
		"meshoptimizer",
		"msdf-atlas-gen",
		"freetype",
    }

	defines
	{
		"IMGUI_USER_CONFIG=\"../../Shape/Src/Shape/ImGui/ImConfig.h\"",
		"GLM_FORCE_INTRINSICS",
		"GLM_FORCE_DEPTH_ZERO_TO_ONE",
		"SHAPE_ENGINE"
	}

	filter { "files:Shape/External/**"}
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
		