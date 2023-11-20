project "implot"
    kind "StaticLib"
    language "C++"
    staticruntime "on"
    targetdir(tdir)
    objdir(odir)

    files {
        "implot/*.cpp" ,
        "implot/*.h" 
    }

    includedirs {
        "implot"
    }

    externalincludedirs {
        "../imgui"
    }

    libdirs {
        "../imgui/bin/%{cfg.buildcfg}/imgui"
    }

    links {
        "imgui"
    }

    defines {
        "IMGUI_DEFINE_MATH_OPERATORS" ,
    }

    filter { "system:windows" }
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        symbols "off"
        optimize "on"
