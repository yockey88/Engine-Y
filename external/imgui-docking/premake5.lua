project "imgui"
    kind "StaticLib"
    language "C++"
    staticruntime "on"
    targetdir(tdir)
    objdir(odir)

    files {
        "imgui/*.cpp" ,
        "imgui/*.h" ,
        "imgui/backends/imgui_impl_sdl2.cpp" ,
        "imgui/backends/imgui_impl_sdl2.h" ,
        "imgui/backends/imgui_impl_opengl3.cpp" ,
        "imgui/backends/imgui_impl_opengl3.h" ,
        "imgui/misc/cpp/imgui_stdlib.cpp" ,
        "imgui/misc/cpp/imgui_stdlib.h" ,
        "imgui/misc/freetype/imgui_freetype.cpp" ,
        "imgui/misc/freetype/imgui_freetype.h" ,
        "imgui/misc/fonts/binary_to_compressed_c.cpp" ,
    }

    includedirs {
        "imgui" ,
        "imgui/backends" ,
        "imgui/misc/cpp"
    }

    externalincludedirs {
        "../SDL2-2.24.2/include" 
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