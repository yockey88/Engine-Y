local program_files = os.getenv("PROGRAMFILES")
local externals_folder = "C:/Yock/Projects/code/engine2.2/external"

externals = {}
externals["sdl2"] = externals_folder .. "/SDL2-2.24.2"
externals["imgui"] = externals_folder .. "/imgui-docking"
externals["imguizmo"] = externals_folder .. "/imguizmo"
externals["implot"] = externals_folder .. "/implot"
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
externals["nfd"] = externals_folder .. "/nativefiledialog"
externals["msdfgen"] = externals_folder .. "/msdf-atlas-gen/msdfgen"
externals["msdfatlasgen"] = externals_folder .. "/msdf-atlas-gen"
externals["choc"] = externals_folder .. "/choc"

return externals