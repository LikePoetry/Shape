function SetRecommendedSettings()
	filter ()
	editandcontinue "Off"
	warnings "Off"
end

workspace "shape"
    architecture "x64"
    startproject "editor"

    configurations{
        "Debug"
    }

    outputdir="%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group ""

--静态链接库，核心文件
project "shape"
	location "shape"
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
		"%{prj.name}/Src"
	}

	filter 'architecture:x86_64'

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "on"
		systemversion "latest"
		disablewarnings { 4307 }
		characterset ("Unicode")

		pchheader "hzpch.h"
		pchsource "shape/Src/hzpch.cpp"
		
		links
		{
		}

		buildoptions
		{
		"/MP", "/bigobj"
		}

		filter 'files:shape/external/**.cpp'
			flags  { 'NoPCH' }
		filter 'files:shape/external/**.c'
			flags  { 'NoPCH' }


	disablewarnings { 4307 }

	filter "configurations:Debug"
		defines {"TRACY_ENABLE","TRACY_ON_DEMAND"}
		runtime "Debug"
		symbols "on"
		optimize "Off"

-- 测试工具项目
project "editor"
    location "editor"
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
        "shape/Src"
    }

    links
    {
        "shape"
    }

	filter { "files:shape/external/**"}
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
		