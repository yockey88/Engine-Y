project "modules"
    location "modules"
    kind "SharedLib"
    language "C#"
    dotnetframework "4.7.2"

    targetdir(tdir)
    objdir(odir)

    files {
        "%{prj.name}/Source/**.cs"
    }

    filter "configurations:Debug"
        optimize "Off"
        symbols "Default"

    filter "configurations:Release"
        optimize "On"
        symbols "Default"