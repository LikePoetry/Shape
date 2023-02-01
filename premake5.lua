function SetRecommendedSettings()
	filter ()
	editandcontinue "Off"
	warnings "Off"
end

IncludeDir = {}
IncludeDir["spdlog"] = "Shape/External/spdlog/include"
IncludeDir["external"] = "Shape/External"

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
	}

	libdirs 
	{ 
		"spdlog",
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
			"_CRT_SECURE_NO_WARNINGS",
			"_DISABLE_EXTENDED_ALIGNED_STORAGE",
			"SHAPE_ENGINE"
		}
		
		links
		{
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
    }

    links
    {
        "Shape"
    }

	defines
	{
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
		