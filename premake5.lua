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

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- HTN
project "HTN"
    location "HTN"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("int/" .. outputdir .. "/%{prj.name}")

    pchheader "pch.h"
    pchsource "%{prj.name}/src/pch.cpp"
    forceincludes "pch.h"

    files { "%{prj.name}/src/**.cpp", "%{prj.name}/src/**.h" }

    includedirs { "%{prj.name}/src" }

-- HTNTest
project "HTNTest"
    location "HTNTest"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("int/" .. outputdir .. "/%{prj.name}")

    pchheader "pch.h"
    pchsource "%{prj.name}/src/pch.cpp"
    forceincludes "pch.h"

    files { "%{prj.name}/src/**.cpp", "%{prj.name}/src/**.h" }

    includedirs { "%{prj.name}/src", "HTN/src" }

    links { "HTN" }

    nuget { "Microsoft.googletest.v140.windesktop.msvcstl.static.rt-dyn:1.8.1.7" }
