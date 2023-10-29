workspace "engine"
    version = "0.0.2"

    projectname = "sandbox"

    architecture "x64"

    startproject "EngineY"

    configurations {
        "Debug" ,
        "Release"
    }

    defines {
        "_CRT_SECURE_NO_WARNINGS"
    }

    binaries = "bin"
    objectdir = "bin-obj"
    tdir = binaries .. "/%{cfg.buildcfg}/%{prj.name}"
    odir = objectdir .. "/%{cfg.buildcfg}/%{prj.name}"

    -- External Dependency Directories
    externals = {}
    externals["sdl2"] = "external/SDL2-2.24.2"
    externals["imgui"] = "external/imgui-docking"
    externals["imguizmo"] = "external/imguizmo"
    externals["entt"] = "external/entt"
    externals["spdlog"] = "external/spdlog-1.11.0"
    externals["glad"] = "external/glad"
    externals["glm"] = "external/glm-master"
    externals["stb"] = "external/stb"
    externals["mono"] = "external/mono"
    externals["assimp"] = "external/assimp"
    externals["react"] = "external/ReactPhysics3d"
    externals["magic_enum"] = "external/magic_enum"
    externals["zep"] = "external/zep"
 
    include "external/glad"
    include "external/spdlog-1.11.0"
    include "external/imgui-docking"
    include "external/imguizmo"
    include "external/ReactPhysics3d"

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
            "%{externals.sdl2}/lib/x64" ,
            "%{externals.mono}/lib/%{cfg.buildcfg}" ,
            "%{externals.assimp}/lib/%{cfg.buildcfg}" 
        }

        defines {
            "GLFW_INCLUDE_NONE" ,
            "MONO_DLLS_PATH=\"external\"" ,
            "MONO_CONFIG_PATH=\"external\"" ,
            "MODULES_PATH=\"" .. binaries .. "/%{cfg.buildcfg}/modules\"" ,
            "INTERNAL_RESOURCES_DIR=\"%{prj.name}/resources\"" ,

            "ENGINE_NAME=\"Engine Y " .. version .. "\"" ,
            "ENGINE_BIN=\"" .. binaries .. "/%{cfg.buildcfg}/%{prj.name}\"" ,
            "ENGINE_RESOURCES_DIR=\"%{prj.name}/resources\"" ,

            "PROJECT_NAME=\"" .. projectname .. "\"" ,
            "PROJECT_BIN=\"" .. binaries .. "/%{cfg.buildcfg}/" .. projectname .. "\"" ,
            "PROJECT_MODULES=\"" .. projectname .. "/modules\"" ,
            "PROJECT_RESOURCES_DIR=\"" .. projectname .. "/resources\"" ,
        }

        links {
            "SDL2" ,
            "glad" ,
            "spdlog" ,
            "imgui" ,
            "imguizmo" ,
            "mono-2.0-sgen" ,
            "reactphysics3d"
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

    if not (os.isdir(projectname .. "/resources"))
    then 
        os.mkdir(projectname .. "/resources")
    end
    if not (os.isdir(projectname .. "/resources/shaders"))
    then 
        os.mkdir(projectname .. "/resources/shaders")
    end
    if not (os.isdir(projectname .. "/resources/textures"))
    then 
        os.mkdir(projectname .. "/resources/textures")
    end
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
            "engine/include" ,
            "EngineY/include" ,
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
            "%{externals.sdl2}/lib/x64" ,
            "%{externals.mono}/lib/%{cfg.buildcfg}" ,
            "%{externals.assimp}/lib/%{cfg.buildcfg}"
        }
        
        projectmodules = projectname .. "_modules"
        links {
            "engine" ,
            "modules" ,
            projectmodules ,
            "SDL2" ,
            "glad" ,
            "spdlog" ,
            "imgui" ,
            "imguizmo" ,
            "mono-2.0-sgen" ,
            "reactphysics3d"
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
                "assimp-vc143-mtd" ,
            }
    
        filter "configurations:Release"
            runtime "Release"
            symbols "off"
            optimize "on"
            links {
                "assimp-vc143-mt" ,
            }