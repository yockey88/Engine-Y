project "EngineModules"
    kind "SharedLib"
    language "C#"
    dotnetframework "4.7.2"

    targetdir(tdir)
    objdir(odir)

    files {
        "./Source/**.cs",
    }

    filter "configurations:Debug"
        optimize "Off"
        symbols "Default"

    filter "configurations:Release"
        optimize "On"
        symbols "Default"