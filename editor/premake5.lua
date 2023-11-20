include "./postbuild.lua"

projectname = "editor"

include (projectname .. "_modules")
project (projectname)
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++latest"
    staticruntime "on"

    targetdir(tdir)
    objdir(odir)

    libdirs { "./bin/Debug/EngineY" }
    links { "EngineY" , projectmodules }

    defines { "GLM_FORCE_DEPTH_ZERO_TO_ONE" }

    files {
        "./src/**.cpp",
        "./include/**.hpp"
    }

    includedirs { "include/" , "../engine/include" }

    filter "system:windows"
        entrypoint "WinMainCRTStartup"
        systemversion "latest"
        defines { "ENGINEY_WINDOWS" }

    filter { "system:linux" }
        defines { "ENGINEY_LINUX" , "__EMULATE_UUID" }

        result , err = os.outputof("pkg-config --cflags --libs gtk+-3.0")
        linkoptions { result }

    RunPostBuildCommands()

    filter "configurations:Debug"
        symbols "on"
        defines { "ENGINEY_DEBUG" , "ENGINEY_MEMORY_DEBUG" }

        ProcessDependencies("Debug")

    filter "configurations:Release"
        optimize "on"
        symbols "off"
        defines { "ENGINEY_RELEASE" }

        ProcessDependencies("Release")

    filter "configurations:Debug or configurations:Release"
        defines { "ENGINEY_TRACK_MEMORY" }
