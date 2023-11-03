#include "EngineY.hpp"

#include <nfd/nfd.h>

class Launcher : public YE::App {
    bool program_files_found = false;
    bool project_explorer_open = false;
    bool project_builder_open = false;
    bool project_config_open = false;
    bool project_config_editor_open = false;
    bool editor_open = false;
    bool file_explorer_open = false;

    char author_buffer[256] = { 0 };
    char project_name[256] = { 0 };
    char description_buffer[256] = { 0 };

    std::filesystem::path engine_folder;
    std::filesystem::path project_folder;

    std::string project_name_str;
    std::string chosen_location;

    std::unique_ptr<YE::TextEditor> editor = nullptr;

    bool IsIgnoredExtension(const std::filesystem::path& extension);

    void BuildProject();
    void LaunchProject();
    void ProjectExplorer();
    bool GenerateProjectDirectory(const std::string& chosen_path);
    void ConfigureProject(); 
    void ProjectBuilder();
   
    public:
        Launcher() 
            : YE::App("launcher") {}
        virtual ~Launcher() {}

        virtual YE::EngineConfig GetEngineConfig() override {
            YE::EngineConfig config;
            config.project_name = "launcher";
            config.use_project_file = false;
            return config;
        }

        virtual YE::WindowConfig GetWindowConfig() override {
            YE::WindowConfig config;
            config.title = "Engine Y Launcher";
            config.size.x = 800;
            config.size.y = 600;
            config.clear_color = { 0.1f , 0.1f , 0.1f , 1.f };
            config.fullscreen = false;
            config.vsync = false;
            config.rendering_to_screen = true;
            config.flags |= SDL_WINDOW_RESIZABLE;
            return config;
        }

        virtual void PreInitialize() override {
            YE::Filesystem::OverrideResourcePath("launcher/launcher/resources");
        }

        virtual bool Initialize() override {
            EngineY::RegisterKeyPressCallback(
                [&](YE::KeyPressed* event) -> bool {
                    if (event->Key() == YE::Keyboard::Key::YE_ESCAPE && !editor_open) {
                        EngineY::DispatchEvent(ynew YE::ShutdownEvent);
                    }
                    return true;
                } ,
                "editor-keys"
            );

            std::string program_files{};        
            try {
                program_files = std::string{ std::getenv("PROGRAMFILES") };
                program_files_found = true;
            } catch (std::exception& e) {
                YE_ERROR("Failed to get PROGRAMFILES environment variable");
                YE_ERROR(e.what());
                return false;
            }

            engine_folder = std::filesystem::path(program_files) / "EngineY";
            project_folder = engine_folder / "projects";

            if (!std::filesystem::exists(engine_folder)) {
                std::filesystem::create_directory(engine_folder);
                std::filesystem::create_directory(project_folder);
            }

            return true;
        }

        virtual void Update() {
            if (editor != nullptr)
                editor->Update();            
        }

        virtual void DrawGui() {
            if (!project_config_open) { 
                if (ImGui::Begin("Engine Y Project Launcher")) {
                    if (ImGui::Button("Open Project")) {
                        ProjectExplorer();
                    } 
                    ImGui::SameLine();
                    if (ImGui::Button("Create Project")) ImGui::OpenPopup("Project Creator"); 

                    ProjectBuilder(); 
                }
                ImGui::End();  
            } else {
                ConfigureProject();
            }
        }
};

bool Launcher::IsIgnoredExtension(const std::filesystem::path& extension) {
    std::string ext = extension.string();
    if (ext == ".vcxproj" || ext == ".vcxproj.filters" || ext == ".vcxproj.user" || 
        ext == ".sln"     || ext == ".csproj"          || ext == ".csproj.user") {
        return true;
    }
    return false;
}

void Launcher::BuildProject() {
#if YE_PLATFORM_WIN
    std::filesystem::path premake_exe = engine_folder / "premake" / "premake5.exe";
    std::filesystem::path premake_path = project_folder / (project_name_str + ".lua");
    std::string cmd = fmt::format(
        "\"{}\" --file={} vs2022" ,
        // "cmd /c start \"{}\" --file=\"{}\" vs2022" ,
        premake_exe.string() , premake_path.string()
    );
    std::system(cmd.c_str());
#endif

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

#if YE_PLATFORM_WIN
    std::filesystem::path program_files;
    try {
        program_files = std::string{ std::getenv("PROGRAMFILES") };
    } catch (std::exception& e) {
        YE_ERROR("Failed to get Program Files directory");
        YE_ERROR(e.what());
        return;
    }

	std::filesystem::path ms_build = program_files / "Microsoft Visual Studio" / "2022" / "Community" / 
                                     "Msbuild" / "Current" / "Bin" / "MSBuild.exe";
    
    std::filesystem::path sln_path = project_folder / (project_name_str + ".sln");

    if (!std::filesystem::exists(ms_build)) {
        YE_ERROR("MSBuild.exe not found");
        return;
    }

    if (!std::filesystem::exists(sln_path)) {
        YE_ERROR("Failed to generate solution file for project");
        return;
    }

    std::string command = fmt::format(
        "\"\"{}\" \"{}\" /p:Configuration=Debug\"", 
        ms_build.string(), sln_path.string()
    );

    std::system(command.c_str());
#endif
    
    std::filesystem::path assimp_dll = engine_folder / "external" / "assimp" / "lib" / "Debug" / "assimp-vc143-mtd.dll";
    std::filesystem::path assimp_pdb = engine_folder / "external" / "assimp" / "lib" / "Debug" / "assimp-vc143-mtd.pdb"; 
    std::filesystem::path monosgen_dll = engine_folder / "external" / "mono" / "bin" / "Debug" / "mono-2.0-sgen.dll";
    std::filesystem::path monosgen_pdb = engine_folder / "external" / "mono" / "bin" / "Debug" / "mono-2.0-sgen.pdb";
    std::filesystem::path monoposixhelper_dll = engine_folder / "external" / "mono" / "bin" / "Debug" / "MonoPosixHelper.dll";
    std::filesystem::path monoposixhelper_pdb = engine_folder / "external" / "mono" / "bin" / "Debug" / "MonoPosixHelper.pdb";
    std::filesystem::path sdl2_dll = engine_folder / "external" / "SDL2-2.24.2" / "lib" / "x64" / "SDL2.dll";
    std::filesystem::path modules_dll = engine_folder / "bin" / "Debug" / "modules" / "modules.dll";
    std::filesystem::path modules_pdb = engine_folder / "bin" / "Debug" / "modules" / "modules.pdb";

    std::filesystem::copy_file(assimp_dll , project_folder / "bin" / "Debug" / project_name_str / "assimp-vc143-mtd.dll" , std::filesystem::copy_options::overwrite_existing);
    std::filesystem::copy_file(assimp_pdb , project_folder / "bin" / "Debug" / project_name_str / "assimp-vc143-mtd.pdb" , std::filesystem::copy_options::overwrite_existing);
    std::filesystem::copy_file(monosgen_dll , project_folder / "bin" / "Debug" / project_name_str / "mono-2.0-sgen.dll" , std::filesystem::copy_options::overwrite_existing);
    std::filesystem::copy_file(monosgen_pdb , project_folder / "bin" / "Debug" / project_name_str / "mono-2.0-sgen.pdb" , std::filesystem::copy_options::overwrite_existing);
    std::filesystem::copy_file(monoposixhelper_dll , project_folder / "bin" / "Debug" / project_name_str / "MonoPosixHelper.dll" , std::filesystem::copy_options::overwrite_existing);
    std::filesystem::copy_file(monoposixhelper_pdb , project_folder / "bin" / "Debug" / project_name_str / "MonoPosixHelper.pdb" , std::filesystem::copy_options::overwrite_existing);
    std::filesystem::copy_file(sdl2_dll , project_folder / "bin" / "Debug" / project_name_str / "SDL2.dll" , std::filesystem::copy_options::overwrite_existing);
    std::filesystem::copy_file(modules_dll , project_folder / "bin" / "Debug" / project_name_str / "modules.dll" , std::filesystem::copy_options::overwrite_existing);
    std::filesystem::copy_file(modules_pdb , project_folder / "bin" / "Debug" / project_name_str / "modules.pdb" , std::filesystem::copy_options::overwrite_existing);

    std::this_thread::sleep_for(std::chrono::milliseconds(10000));
}

void Launcher::LaunchProject() {
    EngineY::EventManager()->DispatchEvent(ynew YE::ShutdownEvent);

#if YE_PLATFORM_WIN
    std::filesystem::path proj_executable = project_folder / "bin" / "Debug" / project_name_str / (project_name_str + ".exe");

    PROCESS_INFORMATION process_info;
    STARTUPINFOA startup_info;
    ZeroMemory(&startup_info , sizeof(STARTUPINFOA));
    startup_info.cb = sizeof(STARTUPINFOA);

    std::string exec_str = proj_executable.string();

    if (!std::filesystem::exists(proj_executable)) {
        YE_ERROR("Failed to find executable for project");
        return;
    }

    std::wstring wstr = std::wstring(exec_str.begin() , exec_str.end());
    YE_DEBUG("{} | {}" , exec_str.c_str() , project_folder.string());
    bool result = CreateProcessA(
        exec_str.c_str() , nullptr , nullptr , nullptr , false , 
        DETACHED_PROCESS , nullptr , project_folder.string().c_str() , 
        &startup_info , &process_info
    );

    if (result) {
        CloseHandle(process_info.hThread);
        CloseHandle(process_info.hProcess);

        EngineY::EventManager()->DispatchEvent(ynew YE::ShutdownEvent);
    } else {
        YE_ERROR("Failed to launch project");
        DWORD error = GetLastError();
        YE_ERROR("Error: {}" , error);
    }

#endif
}

void Launcher::ProjectExplorer() {
    nfdchar_t* out_path = nullptr;
    nfdresult_t result = NFD_PickFolder(nullptr , &out_path);

    if (out_path == nullptr) return;

    chosen_location = std::string{ out_path };
    std::filesystem::path path{ chosen_location };
    
    YE_DEBUG("Chosen location: {0}" , chosen_location);

    bool project_found = false;
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        if (entry.is_directory()) continue;
        if (entry.path().extension() == ".yproj") {
            project_found = true;
            break;
        }
    }

    if (!project_found) {
        YE_ERROR("No project found in chosen directory");
        return;
    }

    editor = std::make_unique<YE::TextEditor>();
    editor->Initialize({ 1.0f , 1.0f });

    project_folder = path;
    project_name_str = path.filename().string();
    project_config_open = true;
}

/// \todo Change and add option to specify project folder 
bool Launcher::GenerateProjectDirectory(const std::string& chosen_path) {
    std::string proj_folder = chosen_path + "/" + project_name_str;

#ifdef YE_PLATFORM_WIN
    std::replace(proj_folder.begin() , proj_folder.end() , '/' , '\\');
#endif

    project_folder = proj_folder;
    std::filesystem::path code_path = project_folder / project_name_str;
    
    YE_INFO("Project Path: {0}" , project_folder.string());
    YE_INFO("Code Path: {0}" , code_path.string());

    std::string shader_dir = "resources/shaders";
    std::string texture_dir = "resources/textures";
    std::string model_dir = "resources/models";

#ifdef YE_PLATFORM_WIN
    std::replace(shader_dir.begin() , shader_dir.end() , '/' , '\\');
    std::replace(texture_dir.begin() , texture_dir.end() , '/' , '\\');
    std::replace(model_dir.begin() , model_dir.end() , '/' , '\\');
#endif

    std::filesystem::create_directory(project_folder);
    std::filesystem::create_directory(code_path);
    std::filesystem::create_directory(code_path / "src");
    std::filesystem::create_directory(code_path / "include");
    std::filesystem::create_directory(code_path / "modules");
    std::filesystem::create_directory(code_path / "resources");
    std::filesystem::create_directory(code_path / shader_dir);
    std::filesystem::create_directory(code_path / texture_dir);
    std::filesystem::create_directory(code_path / model_dir);

    std::filesystem::path template_dir = YE::Filesystem::GetResPath();
    template_dir /= "project_templates";

    std::stringstream premake , modules , project_cpp , yproj_file;
    std::ifstream premake_template{ (template_dir / "premake5.lua.in").string() };
    std::ifstream module_template{ (template_dir / "modules.lua.in").string() };
    std::ifstream proj_template{ (template_dir / "project.cpp.in").string() };
    std::ifstream yproj_template{ (template_dir / "project.yproj.in").string() };

    if (!premake_template.is_open()) {
        YE_ERROR("Failed to open premake5.lua.in");
        return false;
    } else {
        premake << premake_template.rdbuf();
        premake_template.close();
    }

    if (!module_template.is_open()) {
        YE_ERROR("Failed to open modules.lua.in");
        return false;
    } else {
        modules << module_template.rdbuf();
        module_template.close();
    }

    if (!proj_template.is_open()) {
        YE_ERROR("Failed to open project.cpp.in");
        return false;
    } else {
        project_cpp << proj_template.rdbuf();
        proj_template.close();
    }

    if (!yproj_template.is_open()) {
        YE_ERROR("Failed to open project.yproj.in");
        return false;
    } else {
        yproj_file << yproj_template.rdbuf();
        yproj_template.close();
    }

    std::string premake_str = premake.str();
    std::string modules_str = modules.str();
    std::string project_cpp_str = project_cpp.str();
    std::string yproj_str = yproj_file.str();

    {
        std::ofstream module_file{ (project_folder / "modules.lua").string() };
        module_file << modules_str;
        module_file.close();
    }

    std::string externals_path = engine_folder.string() + "\\external"; 
    
    premake_str = std::regex_replace(premake_str , std::regex("<projectname>") , project_name_str);
    premake_str = std::regex_replace(premake_str , std::regex("<externals-folder>") , externals_path);
    premake_str = std::regex_replace(premake_str , std::regex("<engine-folder>") , engine_folder.string());
    std::replace(premake_str.begin() , premake_str.end() , '\\' , '/');

    {
        std::ofstream premake_file{ (project_folder / (project_name_str + ".lua")).string() };
        premake_file << premake_str;
        premake_file.close();
    }

    project_cpp_str = std::regex_replace(project_cpp_str , std::regex("<projectname>") , project_name_str);

    {
        std::ofstream project_cpp_file{ (code_path / (project_name_str + ".cpp")).string() };
        project_cpp_file << project_cpp_str;
        project_cpp_file.close();
    }


    std::string description{ description_buffer };
    if (description == "") {
        yproj_str = std::regex_replace(yproj_str , std::regex("<description>") , "No description provided");
    } else {
        yproj_str = std::regex_replace(yproj_str , std::regex("<description>") , description);
    }

    std::string author{ author_buffer };
    if (author == "") {
        yproj_str = std::regex_replace(yproj_str , std::regex("<author>") , "No author provided");
    } else {
        yproj_str = std::regex_replace(yproj_str , std::regex("<author>") , author);
    }

    yproj_str = std::regex_replace(yproj_str , std::regex("<projectname>") , project_name_str);
    yproj_str = std::regex_replace(yproj_str , std::regex("<projectpath>") , project_folder.string());

    {
        std::ofstream yproj_file{ (project_folder / (project_name_str + ".yproj")).string() };
        yproj_file << yproj_str;
        yproj_file.close();
    }

    return true;
}

void Launcher::ConfigureProject() {
    YE_CRITICAL_ASSERTION(editor != nullptr , "Editor is null");
    YE_CRITICAL_ASSERTION(std::filesystem::exists(project_folder) , "Failed to generate project directory");
    YE_CRITICAL_ASSERTION(std::filesystem::exists(project_folder / project_name_str) , "Failed to generate project code directory");
    if (ImGui::Begin("Project Configuration" , &project_config_open)) {
        ImGui::Text("Project Build Configuration");
        for (const auto& entry : std::filesystem::directory_iterator(project_folder)) {
            if (entry.is_directory() || IsIgnoredExtension(entry.path().extension())) continue;
            std::string file_name = entry.path().filename().string();
            if (ImGui::Button(file_name.c_str())) {
                editor->LoadFile(entry.path());
                ImGuiIO& io = ImGui::GetIO();
                editor_open = true;
            }
        }
        ImGui::Separator();
        ImGui::Text("Project Configuration");
        for (const auto& entry : std::filesystem::directory_iterator(project_folder / project_name_str)) {
            if (entry.is_directory() || IsIgnoredExtension(entry.path().extension())) continue;
            std::string file_name = entry.path().filename().string();
            if (ImGui::Button(file_name.c_str())) {
                editor->LoadFile(entry.path());
                ImGuiIO& io = ImGui::GetIO();
                editor_open = true;
            }
        }

        if (ImGui::Button("Confirm" , { 100 , 25 })) {
            project_config_open = false;
            editor_open = false;
            editor->Notify("RequestQuit");
            editor->Shutdown();

            BuildProject();
            LaunchProject();

            EngineY::DispatchEvent(ynew YE::ShutdownEvent);
        }
    }
    ImGui::End();

    editor_open = editor->Draw({ 640 , 480 });
}

void Launcher::ProjectBuilder() {
    ImGuiWindowFlags flags = ImGuiWindowFlags_Popup | ImGuiWindowFlags_Modal  | ImGuiWindowFlags_NoCollapse;
    if (ImGui::BeginPopupModal("Project Creator" , nullptr , flags)) {
        ImGui::InputText("Project Name"  , project_name , 256);
        
        
        if (ImGui::Button("Create")) {
            project_name_str = std::string{ project_name };
            nfdchar_t* out_path = nullptr;
            nfdresult_t result = NFD_PickFolder(nullptr , &out_path);
            if (out_path == nullptr) {
                ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
                return;
            }

            chosen_location = std::string{ out_path };
            if (GenerateProjectDirectory(chosen_location)) {
                YE_DEBUG("{}" , YE::Filesystem::GetCWD());
                editor = std::make_unique<YE::TextEditor>();
                editor->Initialize({ 1.0f , 1.0f });
                project_config_open = true;
            } 
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::SameLine();

        if (ImGui::Button("Cancel")) ImGui::CloseCurrentPopup();
        
        ImGui::EndPopup();
    }
}

DECLARE_APP(Launcher);