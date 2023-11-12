require("premake_config")
externals = require("externals")

version = "0.0.2"
projectname = "launcher"
engine_root = os.getcwd() 

workspace (projectname) 
    architecture "x64"
    startproject (projectname) 
    configurations {
        "Debug" ,
        "Release"
    }

    filter { "action:vs*" }
        linkoptions { "/ignore:4099" }
        disablewarnings { "4068" }
    
    defines {
        "_CRT_SECURE_NO_WARNINGS" ,
    }

    filter { "options:debug" }
        engine_root = os.getcwd() .. "/.."
    
    print("Building launcher to " .. engine_root)
    
    binaries = engine_root .. "/bin/%{cfg.buildcfg}"
    objectdir = engine_root .. "/bin-obj/%{cfg.buildcfg}" 
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
            "%{prj.name}/include**.hpp" ,
            "%{prj.name}/src/**.cpp",
            "%{prj.name}/enginey_launcher.cpp",
        }

        externalincludedirs {
            "%{prj.name}/include" ,
            engine_root .. "/engine/include" ,
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
        }
        
        libdirs {
            binaries .. "/engine" ,
            binaries .. "/glad" ,
            binaries .. "/spdlog" ,
            binaries .. "/imgui" ,
            binaries .. "/imguizmo" , 
            binaries .. "/implot" ,
            binaries .. "/reactphysics3d" ,
            binaries .. "/zep" ,
            binaries .. "/nfd" ,
            binaries .. "/msdf" ,
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