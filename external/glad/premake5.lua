project "glad"
    kind "StaticLib"
    language "C"
    staticruntime "on"
    targetdir(tdir)
    objdir(odir)

    files {
        "include/**.h",
        "src/**.c"
    }

    includedirs {
        "include"
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