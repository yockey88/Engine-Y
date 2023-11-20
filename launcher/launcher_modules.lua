if not (os.isdir(projectname .. "/modules"))
then 
os.mkdir(projectname .. "/modules")
end
projectmodules = (projectname .. "_modules")
project (projectname .. "_modules")
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