require("premake_config") 
externals = require("externals")

workspace "engine"
    version = "0.0.2"
    startproject "EngineY"
    architecture "x64"
    configurations {
        "Debug" ,
        "Release"
    }
    
    engine_root = os.getcwd() 
    externals_folder = engine_root .. "/external"

    filter { "action:vs*" }
        linkoptions { "/ignore:4099" }
        disablewarnings { "4068" }
    
    defines {
        "_CRT_SECURE_NO_WARNINGS" ,
    }

    binaries = engine_root .. "/bin"
    objectdir = engine_root .. "/bin-obj"
    tdir = binaries .. "/%{cfg.buildcfg}/%{prj.name}"
    odir = objectdir .. "/%{cfg.buildcfg}/%{prj.name}"

    print("Building engine to " .. binaries)

    include(externals_folder .. "/glad")
    include(externals_folder .. "/spdlog-1.11.0")
    include(externals_folder .. "/imgui-docking")
    include(externals_folder .. "/imguizmo")
    include(externals_folder .. "/implot")
    include(externals_folder .. "/ReactPhysics3d")
    include(externals_folder .. "/zep")
    include(externals_folder .. "/nativefiledialog")

    include "modules"

    project "engine"
        location "engine"
        kind "StaticLib"
        language "C++"
        cppdialect "C++20"
        staticruntime "on"

        targetdir(tdir)
        objdir(odir)

        files {
            "%{prj.name}/src/**.cpp" ,
            "%{prj.name}/include/**.hpp"
        }

        includedirs {
            "%{prj.name}/include" , 
        }

        externalincludedirs {
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
            "%{externals.nfd}/src"
        }
        
        libdirs {
            "%{externals.sdl2}/lib/x64" ,
            "%{externals.mono}/lib/%{cfg.buildcfg}" ,
            "%{externals.assimp}/lib/%{cfg.buildcfg}" 
        }
        
        defines {
            "GLFW_INCLUDE_NONE" ,
            "BUILD_CONFIG=\"%{cfg.buildcfg}\"" ,
            "MONO_DLLS_PATH=\"" .. engine_root .. "/external\"" ,
            "MONO_CONFIG_PATH=\"" .. engine_root .. "/external\"" ,
            "ENGINE_ROOT=\"" .. engine_root .. "\"" ,
        }

        links {
            "SDL2" ,
            "glad" ,
            "spdlog" ,
            "imgui" ,
            "imguizmo" ,
            "implot" ,
            "mono-2.0-sgen" ,
            "reactphysics3d" ,
            "zep" ,
            "nfd"
        }

        filter { "system:windows" , "configurations:*" }
            systemversion "latest"
            links{
                "psapi" ,
                "dbghelp"
            }
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
                "%{defines}" ,
                "YE_DEBUG_BUILD" 
            }
            links {
                "assimp-vc143-mtd" ,
            }
            buildoptions {
                "/Zi" ,
                "/EHa"
            }

        filter "configurations:Release"
            runtime "Release"
            symbols "off"
            optimize "on"
            defines {
                "%{defines}" ,
                "YE_RELEASE_BUILD"
            }
            buildoptions {
                "/verbose:quiet"
            }
            links {
                "assimp-vc143-mt" ,
            }