-- HTN
workspace "HTN"
    startproject "HTNDemo"
    architecture "x64"
    configurations { "Debug"--[[, "Release"--]] }

    warnings "Extra"
    flags { "FatalWarnings" }

    filter "configurations:Debug"
        defines { "HTN_DEBUG" }
        symbols "On"

    --[[filter "configurations:Release"
        defines { "HTN_RELEASE" }
        optimize "On"--]]

    filter "system:windows"
        systemversion "latest"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

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

    files { "%{prj.name}/src/**.cpp", "%{prj.name}/src/**.h" }

    includedirs { "%{prj.name}/src" }

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

    files { "%{prj.name}/src/**.cpp", "%{prj.name}/src/**.h",
                -- imgui includes --
				"ThirdParty/imgui-1.89.4/imconfig.h", 
				"ThirdParty/imgui-1.89.4/imgui.cpp", 
				"ThirdParty/imgui-1.89.4/imgui.h", 
				"ThirdParty/imgui-1.89.4/imgui_demo.cpp", 
				"ThirdParty/imgui-1.89.4/imgui_draw.cpp", 
				"ThirdParty/imgui-1.89.4/imgui_internal.h", 
				"ThirdParty/imgui-1.89.4/imgui_tables.cpp", 
				"ThirdParty/imgui-1.89.4/imgui_widgets.cpp" }

    includedirs { "%{prj.name}/src", "HTNPlanner/src", "ThirdParty/imgui-1.89.4" }

    links { "HTNPlanner" }

-- HTNTest
--[[
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

    includedirs { "%{prj.name}/src", "HTNPlanner/src" }

    links { "HTNPlanner" }

    nuget { "Microsoft.googletest.v140.windesktop.msvcstl.static.rt-dyn:1.8.1.7" }
]]

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

    files { "%{prj.name}/src/**.cpp", "%{prj.name}/src/**.h", 
				-- imgui includes --
				"ThirdParty/imgui-1.89.4/imconfig.h", 
				"ThirdParty/imgui-1.89.4/imgui.cpp", 
				"ThirdParty/imgui-1.89.4/imgui.h", 
				"ThirdParty/imgui-1.89.4/imgui_demo.cpp", 
				"ThirdParty/imgui-1.89.4/imgui_draw.cpp", 
				"ThirdParty/imgui-1.89.4/imgui_internal.h", 
				"ThirdParty/imgui-1.89.4/imgui_tables.cpp", 
				"ThirdParty/imgui-1.89.4/imgui_widgets.cpp",
				-- imgui backend includes --
				"ThirdParty/imgui-1.89.4/backends/imgui_impl_sdl2.h", 
				"ThirdParty/imgui-1.89.4/backends/imgui_impl_sdl2.cpp", 
				"ThirdParty/imgui-1.89.4/backends/imgui_impl_sdlrenderer.h", 
				"ThirdParty/imgui-1.89.4/backends/imgui_impl_sdlrenderer.cpp" }

    includedirs { "%{prj.name}/src", "HTNPlanner/src", "HTNDebugger/src", "ThirdParty/SDL2-2.26.4/include", "ThirdParty/imgui-1.89.4", "ThirdParty/imgui-1.89.4/backends" }

	libdirs { "ThirdParty/SDL2-2.26.4/lib/x64" }
    links { "HTNPlanner", "HTNDebugger", "SDL2", "SDL2main" }
	
	-- TODO JOSE: Make this work properly, we should copy the SDL2.dll in the right target dir, I am pretty sure there is a macro to do that.
	-- filter "configurations:Debug"
		-- postbuildcommands { "copy ThirdParty/SDL2-2.26.4/lib/x64/SDL2.dll bin\\Debug-windows-x86_64\\%{prj.name}\\" }
	