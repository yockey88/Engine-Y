include "premake/lua/ordered_pairs.lua"

function FirstToUpper(str)
    return (str:gsub("^%l", string.upper))
end

external = {
    SDL2 = {
        include_dir = "%{wks.location}/external/SDL2/SDL2" ,
        lib_name = "SDL2d" , 
        lib_dir = "%{wks.location}/external/SDL2/lib/%{cfg.buildcfg}" ,
    } , 
    ImGui = {
        include_dir = "%{wks.location}/external/imgui" ,
        lib_name = "imgui" ,
    } ,
    ImGuizmo = {
        include_dir = "%{wks.location}/external/imguizmo" ,
        lib_name = "imguizmo" ,
    } ,
    ImPlot = {
        include_dir = "%{wks.location}/external/implot" ,
        lib_name = "implot" ,
    } ,
    Entt = {
        include_dir = "%{wks.location}/external/entt" ,
    } ,
    Spdlog = {
        include_dir = "%{wks.location}/external/spdlog/include" ,
        lib_name = "spdlog" ,
    } ,
    Glad = {
        include_dir = "%{wks.location}/external/glad/include" ,
        lib_name = "glad" ,
    } ,
    GLM = {
        include_dir = "%{wks.location}/external/glm" ,
    } ,
    STB = {
        include_dir = "%{wks.location}/external/stb" ,
    } ,
    Mono = {
        include_dir = "%{wks.location}/external/mono/include" ,
        lib_name = "mono-2.0-sgen" ,
        lib_dir = "%{wks.location}/external/mono/lib/%{cfg.buildcfg}" ,
        configurations = "Debug" , "Release"
    } ,
    Assimp = {
        include_dir = "%{wks.location}/external/assimp/include" ,
        -- debug_lib_name = "assimp-vc143-mtd" , ??? why wont this one link?
        lib_name = "assimp-vc143-mtd" ,
        lib_dir = "%{wks.location}/external/assimp/lib/%{cfg.buildcfg}" ,
        configurations = "Debug" , "Release"
    } ,
    React = {
        lib_name = "reactphysics3d" ,
        include_dir = "%{wks.location}/external/ReactPhysics3d/include" ,
    } ,
    MagicEnum = {
        include_dir = "%{wks.location}/external/magic_enum" ,
    } ,
    Zep = {
        lib_name = "zep" ,
        include_dir = "%{wks.location}/external/zep/include" ,
    } ,
    NFD = {
        include_dir = "%{wks.location}/external/nativefiledialog/src" ,
        lib_name = "nfd" ,
    } ,
    MSDF = {
        include_dir = "%{wks.location}/external/msdf-atlas-gen/msdfgen" ,
    } ,
    MSDFAtlasGen = {
        include_dir = "%{wks.location}/external/msdf-atlas-gen" ,
        lib_name = "msdf" ,
    } ,
    Choc = {
        include_dir = "%{wks.location}/external/choc" ,
    }
}

function LinkDependency(table , debug , target)
    -- Setup library directory
	if table.lib_dir ~= nil then
		libdirs { table.lib_dir }
	end

	-- Try linking
	local lib_name = nil
	if table.lib_name ~= nil then
		lib_name = table.lib_name
	end

	if table.debug_lib_name ~= nil and debug and target == "Windows" then
	    lib_name = table.debug_lib_name
	end

	if lib_name ~= nil then
		links { lib_name }
		return true
	end

	return false
end

function AddInclude(table)
    if table.include_dir ~= nil then
        externalincludedirs{ table.include_dir }
    end
end

function ProcessDependencies(configuration)
    local target = FirstToUpper(os.target())

    for key , lib_data in orderedPairs(external) do

        local matches_config = true

        if config_name ~= nil and lib_data.Configurations ~= nil then
            matches_config = string.find(lib_data.Configurations , config_name)
        end

        local is_debug = configuration == "Debug"

        if matches_config then
            local continue_link = true

            if lib_data[target] ~= nil then 
                continue_link = not LinkDependency(lib_data[target] , is_debug , target)
                AddInclude(lib_data[target])
            end

            if continue_link then
                LinkDependency(lib_data , is_debug , target)
            end

            AddInclude(lib_data)
        end
    end
end

function IncludeDependencies(condition)
    local target = FirstToUpper(os.target())

    for key , lib_data in orderedPairs(external) do

        local matches_config = true

        if config_name ~= nil and lib_data.Configurations ~= nil then
            matches_config = string.find(lib_data.Configurations , config_name)
        end

        if matches_config then
            AddInclude(lib_data)

            if lib_data[target] ~= nil then
                AddInclude(lib_data[target])
            end
        end

    end
end
