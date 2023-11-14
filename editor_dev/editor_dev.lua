require("premake_config")
externals = require("externals")

workspace "editor_dev"
    version = "0.0.2"
    projectname = "editor_dev"
    architecture "x64"
    startproject (projectname) 
    configurations {
        "Debug" ,
        "Release"
    }

    defines {
        "_CRT_SECURE_NO_WARNINGS"
    }

    binaries = "bin/%{cfg.buildcfg}"
    objectdir = "bin-obj/%{cfg.buildcfg}"
    tdir = binaries .. "/%{prj.name}"
    odir = objectdir .. "/%{prj.name}"

    include "modules"

    project (projectname)
        location (projectname)
        kind "ConsoleApp"
        language "C++"
        cppdialect "C++20"
        staticruntime "on"
    
        targetdir(tdir)
        objdir(odir)

        runpathdirs {
            "%{tdir}" ,
            "modules" ,
            projectname .. "_modules"
        }
    
        files {
            "%{prj.name}/**.cpp",
            "%{prj.name}/**.hpp"
        }

        includedirs {
            "%{prj.name}/include" , 
        }

        externalincludedirs {
            "C:/Yock/Projects/code/engine2.2/engine/include" ,
            "%{externals.sdl2}/include" ,
            "%{externals.glad}/include" ,
            "%{externals.glm}" ,
            "%{externals.spdlog}/include" ,
            "%{externals.entt}" ,
            "%{externals.stb}" ,
            "%{externals.mono}/include" ,
            "%{externals.imgui}" ,
            "%{externals.imguizmo}" ,
            "%{externals.implot}" ,
            "%{externals.assimp}/include" ,
            "%{externals.react}/include" ,
            "%{externals.magic_enum}" ,
            "%{externals.zep}/include" ,
            "%{externals.nfd}/src" ,
            "%{externals.msdfgen}" ,
            "%{externals.msdfatlasgen}" ,
            "%{externals.choc}"
        }
        
        libdirs {
            "C:/Yock/Projects/code/engine2.2/bin/%{cfg.buildcfg}/engine" ,
            "C:/Yock/Projects/code/engine2.2/bin/%{cfg.buildcfg}/glad" ,
            "C:/Yock/Projects/code/engine2.2/bin/%{cfg.buildcfg}/spdlog" ,
            "C:/Yock/Projects/code/engine2.2/bin/%{cfg.buildcfg}/imgui" ,
            "C:/Yock/Projects/code/engine2.2/bin/%{cfg.buildcfg}/imguizmo" ,
            "C:/Yock/Projects/code/engine2.2/bin/%{cfg.buildcfg}/implot" ,
            "C:/Yock/Projects/code/engine2.2/bin/%{cfg.buildcfg}/reactphysics3d" ,
            "C:/Yock/Projects/code/engine2.2/bin/%{cfg.buildcfg}/zep" ,
            "C:/Yock/Projects/code/engine2.2/bin/%{cfg.buildcfg}/nfd" ,
            "C:/Yock/Projects/code/engine2.2/bin/%{cfg.buildcfg}/msdf" ,
            "%{externals.sdl2}/lib/x64" ,
            "%{externals.mono}/lib/%{cfg.buildcfg}" ,
            "%{externals.assimp}/lib/%{cfg.buildcfg}"
        }
        
        projectmodules = (projectname .. "_modules")
        links {
            "engine" ,
            projectmodules ,
            "SDL2" ,
            "glad" ,
            "spdlog" ,
            "imgui" ,
            "imguizmo" ,
            "implot" ,
            "mono-2.0-sgen" ,
            "reactphysics3d" ,
            "zep" ,
            "nfd" ,
            "msdf" ,
        }

        filter { "system:windows" , "configurations:*" }
            systemversion "latest"
            entrypoint "WinMainCRTStartup"
            defines {
                "YE_PLATFORM_WIN"
            }
            links {
                "shlwapi.lib" ,
                "ole32.lib" ,
                "shell32.lib" ,
                "propsys.lib" ,
            }

        filter { "system:linux" , "configurations:*" }
            defines {
                "YE_PLATFORM_LINUX"
            }

        filter "configurations:Debug"
            runtime "Debug"
            symbols "on"
            defines {
                "YE_DEBUG_BUILD" 
            }
            links {
                "assimp-vc143-mtd"
            }
    
        filter "configurations:Release"
            runtime "Release"
            symbols "off"
            optimize "on"
            links {
                "assimp-vc143-mt"
            }