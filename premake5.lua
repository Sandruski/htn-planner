-- HTN
workspace "HTN"
    startproject "HTN"
    configurations { "Debug", "Release" }
    platforms { "Win32", "Win64" }

    filter "configurations:Debug"
        defines { "HTN_DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "HTN_RELEASE" }
        optimize "On"

    filter "platforms:Win32"
        architecture "x86"

    filter "platforms:Win64"
        architecture "x86_64"

    filter "system:windows"
        systemversion "latest"

-- HTN
project "HTN"
    location "HTN"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"

    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("int/" .. outputdir .. "/%{prj.name}")

    pchheader "pch.h"
    pchsource "%{prj.name}/src/pch.cpp"
    forceincludes "pch.h"

    files { "%{prj.name}/src/**.cpp", "%{prj.name}/src/**.h" }

    includedirs { "%{prj.name}/src" }
