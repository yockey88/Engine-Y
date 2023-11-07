if not (os.isdir(projectname .. "/modules"))
then 
os.mkdir(projectname .. "/modules")
end
project (projectname .. "_modules")
    location (projectname .. "/modules")
    kind "SharedLib"
    language "C#"
    dotnetframework "4.7.2"

    targetdir(binaries .. "/" .. projectname)
    objdir(objectdir .. "/" .. projectname)

    files {
        projectname .. "/modules/**.cs"
    }

    libdirs {
        binaries .. "/modules"
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