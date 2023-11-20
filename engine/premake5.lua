project "EngineY"
    kind "StaticLib"
    language "C++"
    cppdialect "C++latest"
    staticruntime "on"

    targetdir(tdir)
    objdir(odir)

    files {
        "./src/**.cpp",
        "./include/**.hpp"
    }

    includedirs { "include/" }

    IncludeDependencies()

    defines { "ENGINEY_BUILD" , "GLM_FORCE_DEPTH_ZERO_TO_ONE" }

    filter { "system:windows" , "configurations:*" }
        systemversion "latest"
        defines "ENGINEY_WINDOWS"

    filter { "system:macosx" , "configurations:*" }
        systemversion "latest"
        xcodebuildsettings {
            ["MACOSX_DEPLOYMENT_TARGET"] = "10.15",
            ["UseModernBuildSystem"] = "NO"
        }

    filter { "system:linux" , "configurations:*" }
        defines { "ENGINEY_LINUX"  , "__EMULATE_UUID" }

    filter "configurations:Debug"
        symbols "on"
        defines { "ENGINEY_DEBUG" , "ENGINEY_MEMORY_DEBUG" }

    filter { "system:windows" , "configurations:Debug" }
        defines { "ENGINEY_DEBUG_WINDOWS" }
        sanitize { "Address" }
        flags { "NoRuntimeChecks" }
        links { "psapi" , "dbghelp" }

    filter "configurations:Release"
        optimize "on"
        symbols "off"
        defines { "ENGINEY_RELEASE" }
