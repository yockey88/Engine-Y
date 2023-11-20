include "externals.lua"

workspace "EngineY"
    startproject "runtime"
    architecture "x64"
    configurations {
        "Debug" ,
        "Release"
    }

    startproject "engine"

    language "C++"
    cppdialect "C++latest"
    staticruntime "on"

    flags { "MultiProcessorCompile" }

    defines {
        "_CRT_SECURE_NO_WARNINGS" ,
        "_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING" ,
        "GLFW_INCLUDE_NONE" ,
    }

    filter "action:vs*"
        linkoptions { "/ignore:4099" }
        disablewarnings { "68" }

    filter "configurations:Debug"
        optimize "Off"
        symbols "On"

    filter { "system:windows" , "configurations:Debug" }
        sanitize { "Address" }
        flags { "NoRuntimeChecks"}
        defines { "NOMINMAX" }

    filter "configurations:Release"
        optimize "On"
        symbols "Default"
    
    filter "system:windows"
        buildoptions { "/EHsc" , "/Zc:preprocessor" , "/Zc:__cplusplus" }

tdir = "%{wks.location}/bin/%{cfg.buildcfg}/%{prj.name}"
odir = "%{wks.location}/bin_obj/%{cfg.buildcfg}/%{prj.name}"

print("Processing external dependencies...")
group "External"
include("external/glad")
include("external/spdlog")
include("external/imgui")
include("external/imguizmo")
include("external/implot")
include("external/ReactPhysics3d")
include("external/zep")
include("external/nativefiledialog")
include("external/msdf-atlas-gen")
group ""

print("Generating Engine Y build pipeline...")
group "Engine"
include "./engine"
include "./modules"
group ""

print("Generating game develepment build pipeline...")
group "GameDevApps"
include "./launcher"
include "./editor"
group ""