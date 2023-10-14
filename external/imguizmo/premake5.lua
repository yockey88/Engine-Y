project "imguizmo"
    kind "StaticLib"
    language "C++"
    staticruntime "on"
    targetdir(tdir)
    objdir(odir)

    files {
        "imguizmo/*.cpp" ,
        "imguizmo/*.h" 
    }

    includedirs {
        "imguizmo"
    }

    externalincludedirs {
        "../imgui-docking/imgui"
    }

    libdirs {
        "../imgui-docking/bin/%{cfg.buildcfg}/imgui"
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