project "nfd"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"
    targetdir(tdir)
    objdir(odir)

    files {
        "src/nfd_common.c",
        "src/nfd_common.h",
        "src/nfd/nfd.h" ,
        "src/simple_exec.h"
    }

    includedirs {
        "src"
    }

    filter { "system:windows" }
        systemversion "latest"
        files {
            "src/nfd_win.cpp"
        }
    
    filter { "system:linux" }
        systemversion "latest"
        files {
            "src/nfd_gtk.c"
        }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        symbols "off"
        optimize "on"
