workspace "launcher"
    version = "0.0.2"
    projectname = "launcher"
    externals_folder = "../external"

    architecture "x64"

    startproject (projectname) 

    configurations {
        "Debug" ,
        "Release"
    }

    defines {
        "_CRT_SECURE_NO_WARNINGS"
    }

    binaries = "../bin/%{cfg.buildcfg}"
    objectdir = "../bin-obj/%{cfg.buildcfg}" 
    tdir = binaries .. "/%{prj.name}"
    odir = objectdir .. "/%{prj.name}"

    -- External Dependency Directories
    externals = {}
    externals["sdl2"] = externals_folder .. "/SDL2-2.24.2"
    externals["imgui"] = externals_folder .. "/imgui-docking"
    externals["imguizmo"] = externals_folder .. "/imguizmo"
    externals["entt"] = externals_folder .. "/entt"
    externals["spdlog"] = externals_folder .. "/spdlog-1.11.0"
    externals["glad"] = externals_folder .. "/glad"
    externals["glm"] = externals_folder .. "/glm-master"
    externals["stb"] = externals_folder .. "/stb"
    externals["mono"] = externals_folder .. "/mono"
    externals["assimp"] = externals_folder .. "/assimp"
    externals["react"] = externals_folder .. "/ReactPhysics3d"
    externals["magic_enum"] = externals_folder .. "/magic_enum"
    externals["zep"] = externals_folder .. "/zep"

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
        }

        externalincludedirs {
            "../engine/include" ,
            "%{externals.sdl2}/include" ,
            "%{externals.glad}/include" ,
            "%{externals.glm}" ,
            -- "${externals.fmt}/include" ,
            "%{externals.spdlog}/include" ,
            "%{externals.entt}" ,
            "%{externals.stb}" ,
            "%{externals.mono}/include" ,
            "%{externals.imgui}" ,
            "%{externals.imguizmo}" ,
            "%{externals.assimp}/include" ,
            "%{externals.react}/include" ,
            "%{externals.magic_enum}" ,
            "%{externals.zep}/include"
        }
        
        libdirs {
            binaries .. "/engine" ,
            "%{externals.glad}/bin/%{cfg.buildcfg}/glad" ,
            "%{externals.spdlog}/bin/%{cfg.buildcfg}/spdlog" ,
            "%{externals.imgui}/bin/%{cfg.buildcfg}/imgui" ,
            "%{externals.imguizmo}/bin/%{cfg.buildcfg}/imguizmo" ,
            "%{externals.react}/bin/%{cfg.buildcfg}/reactphysics3d" ,
            "%{externals.zep}/bin/%{cfg.buildcfg}/zep" ,
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
            "mono-2.0-sgen" ,
            "reactphysics3d" ,
            "zep"
        }

        filter { "system:windows" , "configurations:*" }
            systemversion "latest"
            entrypoint "WinMainCRTStartup"
            defines {
                "YE_PLATFORM_WIN"
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