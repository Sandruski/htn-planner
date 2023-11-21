-- HTN
workspace "HTN"
    startproject "HTNDemo"
    architecture "x64"
    configurations { "Debug", "Release" }

    warnings "Extra"
    flags { "FatalWarnings" }

    filter "configurations:Debug"
        defines { "HTN_DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "HTN_RELEASE" }
        optimize "On"

    filter "system:windows"
        systemversion "latest"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- HTNFramework
project "HTNFramework"
    location "HTNFramework"
    kind "SharedItems"
    language "C++"
    cppdialect "C++20"

    pchheader "pch.h"
    pchsource "%{prj.name}/src/pch.cpp"
    forceincludes "pch.h"

    files { "%{prj.name}/src/**.cpp",
            "%{prj.name}/src/**.h" }

    includedirs { "%{prj.name}/src" }

-- HTNPlanner
project "HTNPlanner"
    location "HTNPlanner"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("int/" .. outputdir .. "/%{prj.name}")

    pchheader "pch.h"
    pchsource "%{prj.name}/src/pch.cpp"
    forceincludes "pch.h"

    files { "%{prj.name}/src/**.cpp",
            "%{prj.name}/src/**.h",                 
            -- Optick
            "ThirdParty/Optick/src/**.cpp",
            "ThirdParty/Optick/src/**.h" }

    includedirs { "%{prj.name}/src", "HTNFramework/src", "ThirdParty/Optick/src" }

    links { "HTNFramework" }

-- HTNDebugger
project "HTNDebugger"
    location "HTNDebugger"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("int/" .. outputdir .. "/%{prj.name}")

    pchheader "pch.h"
    pchsource "%{prj.name}/src/pch.cpp"
    forceincludes "pch.h"

    files { "%{prj.name}/src/**.cpp",
            "%{prj.name}/src/**.h",
            -- ImGui
			"ThirdParty/imgui/imconfig.h", 
			"ThirdParty/imgui/imgui.cpp", 
			"ThirdParty/imgui/imgui.h", 
			"ThirdParty/imgui/imgui_demo.cpp", 
			"ThirdParty/imgui/imgui_draw.cpp", 
			"ThirdParty/imgui/imgui_internal.h", 
			"ThirdParty/imgui/imgui_tables.cpp", 
			"ThirdParty/imgui/imgui_widgets.cpp" }

    includedirs { "%{prj.name}/src", "HTNFramework/src", "HTNPlanner/src", "ThirdParty/imgui" }

    links { "HTNFramework", "HTNPlanner" }

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

    files { "%{prj.name}/src/**.cpp",
            "%{prj.name}/src/**.h" }

    includedirs { "%{prj.name}/src", "HTNFramework/src", "HTNPlanner/src", "ThirdParty/Optick/src" }

    links { "HTNFramework", "HTNPlanner" }

    nuget { "Microsoft.googletest.v140.windesktop.msvcstl.static.rt-dyn:1.8.1.7" }

-- HTNDemo
project "HTNDemo"
    location "HTNDemo"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("int/" .. outputdir .. "/%{prj.name}")

    pchheader "pch.h"
    pchsource "%{prj.name}/src/pch.cpp"
    forceincludes "pch.h"

    files { "%{prj.name}/src/**.cpp",
            "%{prj.name}/src/**.h", 
            -- ImGui
			"ThirdParty/imgui/imconfig.h", 
			"ThirdParty/imgui/imgui.cpp", 
			"ThirdParty/imgui/imgui.h", 
			"ThirdParty/imgui/imgui_demo.cpp", 
			"ThirdParty/imgui/imgui_draw.cpp", 
			"ThirdParty/imgui/imgui_internal.h", 
			"ThirdParty/imgui/imgui_tables.cpp", 
			"ThirdParty/imgui/imgui_widgets.cpp",
			"ThirdParty/imgui/backends/imgui_impl_sdl2.h", 
			"ThirdParty/imgui/backends/imgui_impl_sdl2.cpp", 
			"ThirdParty/imgui/backends/imgui_impl_sdlrenderer2.h", 
			"ThirdParty/imgui/backends/imgui_impl_sdlrenderer2.cpp",
            -- Optick
            "ThirdParty/Optick/src/**.cpp",
            "ThirdParty/Optick/src/**.h" }

    includedirs { "%{prj.name}/src", "HTNFramework/src", "HTNPlanner/src", "HTNDebugger/src", "ThirdParty/Optick/src", "ThirdParty/SDL2/include", "ThirdParty/imgui", "ThirdParty/imgui/backends" }

	libdirs { "ThirdParty/SDL2/lib/x64" }
    links { "HTNFramework", "HTNPlanner", "HTNDebugger", "SDL2", "SDL2main" }
	
	-- TODO JOSE: Make this work properly, we should copy the SDL2.dll in the right target dir, I am pretty sure there is a macro to do that.
	-- filter "configurations:Debug"
		-- postbuildcommands { "copy ThirdParty/SDL2/lib/x64/SDL2.dll bin\\Debug-windows-x86_64\\%{prj.name}\\" }
	