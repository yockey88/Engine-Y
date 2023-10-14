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

--- TODO: generate this part with a new project 
if not (os.isdir(projectname .. "/modules"))
then 
os.mkdir(projectname .. "/modules")
end
project (projectname .. "_modules")
    location (projectname .. "/modules")
    kind "SharedLib"
    language "C#"
    dotnetframework "4.7.2"

    targetdir(binaries .. "/%{cfg.buildcfg}/" .. projectname)
    objdir(objectdir .. "/%{cfg.buildcfg}/" .. projectname)

    files {
        projectname .. "/modules/**.cs"
    }

    links {
        "modules"
    }

    filter "configurations:Debug"
        optimize "Off"
        symbols "Default"

    filter "configurations:Release"
        optimize "On"
        symbols "Default"
----------------------------------------------