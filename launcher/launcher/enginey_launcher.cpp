#include "EngineY.hpp"

class Launcher : public YE::App {
    bool app_data_found = false;
    bool project_explorer_open = false;
    bool project_builder_open = false;
    bool project_config_open = false;
    bool editor_open = false;

    char author_buffer[256] = { 0 };
    char project_name[256] = { 0 };
    char description_buffer[256] = { 0 };

    std::filesystem::path engine_folder;
    std::filesystem::path project_folder;

    std::string project_name_str;

    std::unique_ptr<YE::TextEditor> editor = nullptr;

    void BuildProject(const std::string& project_name);
    void LaunchProject(const std::string& project_name);
    void ProjectExplorer();
    bool GenerateProjectDirectory(const std::string& project_name);
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
            config.size.x = 1920;
            config.size.y = 1080;
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
                    if (event->Key() == YE::Keyboard::Key::YE_ESCAPE && !editor_open)
                        EngineY::DispatchEvent(ynew YE::ShutdownEvent);
                    return true;
                } ,
                "editor-keys"
            );

            std::string app_data_path{};        
            try {
                app_data_path = std::string{ std::getenv("APPDATA") };
                app_data_found = true;
            } catch (std::exception& e) {
                YE_ERROR("Failed to get APPDATA environment variable");
                YE_ERROR(e.what());
                return false;
            }

            engine_folder = app_data_path;
            engine_folder /= "EngineY";
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
                    if (ImGui::Button("Open Project")) ImGui::OpenPopup("Project Explorer");
                    ImGui::SameLine();
                    if (ImGui::Button("Create Project")) ImGui::OpenPopup("Project Creator"); 

                    ProjectExplorer();
                    ProjectBuilder(); 
                }
                ImGui::End();  
            } else {
                ConfigureProject();
            }
        }
};

void Launcher::BuildProject(const std::string& name) {
#if YE_PLATFORM_WIN
    std::string premake_exe = engine_folder.string() + "\\premake\\premake5.exe";
    std::string premake_path = project_folder.string() + "\\" + name + "\\premake5.lua";
    std::string cmd = fmt::format(
        "cmd /c start {} --file={} vs2022" ,
        premake_exe , premake_path        
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
    
    std::string sln_path = project_folder.string() + "\\" + name + "\\" + name + ".sln";
    std::replace(sln_path.begin() , sln_path.end() , '/' , '\\');

    if (std::filesystem::exists(sln_path))
        YE_INFO("Solution path: {0}" , sln_path);
        
    std::string command = fmt::format(
        "\"\"{}\" \"{}\" /p:Configuration=Debug\"", 
        ms_build.string(), sln_path
    );

    YE_INFO("{0}" , command);

    std::system(command.c_str());
#endif
    
    std::filesystem::path assimp_dll = engine_folder / "external" / "assimp" / "lib" / "Debug" / "assimp-vc143-mtd.dll";
    std::filesystem::path assimp_pdb = engine_folder / "external" / "assimp" / "lib" / "Debug" / "assimp-vc143-mtd.pdb"; 
    std::filesystem::path monosgen_dll = engine_folder / "external" / "mono" / "bin" / "Debug" / "mono-2.0-sgen.dll";
    std::filesystem::path monosgen_pdb = engine_folder / "external" / "mono" / "bin" / "Debug" / "mono-2.0-sgen.pdb";
    std::filesystem::path monoposixhelper_dll = engine_folder / "external" / "mono" / "bin" / "Debug" / "MonoPosixHelper.dll";
    std::filesystem::path monoposixhelper_pdb = engine_folder / "external" / "mono" / "bin" / "Debug" / "MonoPosixHelper.pdb";

    std::filesystem::path modules_dll = engine_folder / "bin" / "Debug" / "modules" / "modules.dll";
    std::filesystem::path modules_pdb = engine_folder / "bin" / "Debug" / "modules" / "modules.pdb";

    std::filesystem::copy_file(assimp_dll , engine_folder / "bin" / "Debug" / project_name / "assimp-vc143-mtd.dll" , std::filesystem::copy_options::overwrite_existing);
    std::filesystem::copy_file(assimp_pdb , engine_folder / "bin" / "Debug" / project_name / "assimp-vc143-mtd.pdb" , std::filesystem::copy_options::overwrite_existing);
    std::filesystem::copy_file(monosgen_dll , engine_folder / "bin" / "Debug" / project_name / "mono-2.0-sgen.dll" , std::filesystem::copy_options::overwrite_existing);
    std::filesystem::copy_file(monosgen_pdb , engine_folder / "bin" / "Debug" / project_name / "mono-2.0-sgen.pdb" , std::filesystem::copy_options::overwrite_existing);
    std::filesystem::copy_file(monoposixhelper_dll , engine_folder / "bin" / "Debug" / project_name / "MonoPosixHelper.dll" , std::filesystem::copy_options::overwrite_existing);
    std::filesystem::copy_file(monoposixhelper_pdb , engine_folder / "bin" / "Debug" / project_name / "MonoPosixHelper.pdb" , std::filesystem::copy_options::overwrite_existing);

    std::filesystem::copy_file(modules_dll , engine_folder / "bin" / "Debug" / project_name / "modules.dll" , std::filesystem::copy_options::overwrite_existing);
    std::filesystem::copy_file(modules_pdb , engine_folder / "bin" / "Debug" / project_name / "modules.pdb" , std::filesystem::copy_options::overwrite_existing);

    std::this_thread::sleep_for(std::chrono::milliseconds(10000));
}

void Launcher::LaunchProject(const std::string& project_name) {
    EngineY::EventManager()->DispatchEvent(ynew YE::ShutdownEvent);

#if YE_PLATFORM_WIN
    // std::string proj_executable = engine_folder.string() + "\\" + "bin\\Debug\\" + project_name + "\\" + project_name + ".exe";
    // std::string cmd = fmt::format(
    //     "cmd /c start {}" ,
    //     proj_executable
    // );

    // std::system(cmd.c_str());

    // std::wstring wproj_executable(proj_executable.begin() , proj_executable.end());
    // STARTUPINFO start_info;
    // PROCESS_INFORMATION process_info;
    // ZeroMemory(&start_info , sizeof(start_info));
    // start_info.cb = sizeof(start_info);
    // ZeroMemory(&process_info , sizeof(process_info));

    // CreateProcess(
    //     wproj_executable.c_str() , 
    //     nullptr , nullptr , nullptr , 
    //     false , 0 , 
    //     nullptr , nullptr , 
    //     &start_info , &process_info
    // );
#endif
}

void Launcher::ProjectExplorer() {
    ImGuiWindowFlags flags = ImGuiWindowFlags_Popup | ImGuiWindowFlags_Modal | ImGuiWindowFlags_NoCollapse;
    if (ImGui::BeginPopupModal("Project Explorer" , nullptr , flags)) {
        for (auto& entry : std::filesystem::directory_iterator(project_folder)) {
            if (!entry.is_directory()) continue;
        
            std::string proj_name = entry.path().filename().string();
            if (ImGui::Button(proj_name.c_str())) {
                ImGui::CloseCurrentPopup();
                BuildProject(proj_name); 
                LaunchProject(proj_name);
            }
        }
        ImGui::EndPopup();
    }
}

bool Launcher::GenerateProjectDirectory(const std::string& project_name) {
    YE_INFO("Generating project directory for {0}" , project_name);

    YE_DEBUG("Project Folder: {0}" , project_folder.string()); 
    std::filesystem::path proj_path = project_folder / project_name;
    std::filesystem::path code_path = proj_path / project_name;
    
    YE_INFO("Project Path: {0}" , proj_path.string());
    YE_INFO("Code Path: {0}" , code_path.string());

    std::filesystem::create_directory(proj_path);
    std::filesystem::create_directory(code_path);
    std::filesystem::create_directory(code_path / "src");
    std::filesystem::create_directory(code_path / "include");
    std::filesystem::create_directory(code_path / "modules");
    std::filesystem::create_directory(code_path / "resources");
    std::filesystem::create_directory(code_path / "resources\\shaders");
    std::filesystem::create_directory(code_path / "resources\\textures");
    std::filesystem::create_directory(code_path / "resources\\models");

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

    project_name_str = project_name;

    {
        std::ofstream module_file{ (proj_path / "modules.lua").string() };
        module_file << modules_str;
        module_file.close();
    }

    std::string externals_path = engine_folder.string() + "\\external"; 
    
    premake_str = std::regex_replace(premake_str , std::regex("<projectname>") , project_name);
    premake_str = std::regex_replace(premake_str , std::regex("<externals-folder>") , externals_path);
    premake_str = std::regex_replace(premake_str , std::regex("<engine-folder>") , engine_folder.string());
    std::replace(premake_str.begin() , premake_str.end() , '\\' , '/');

    {
        std::ofstream premake_file{ (proj_path / "premake5.lua").string() };
        premake_file << premake_str;
        premake_file.close();
    }

    project_cpp_str = std::regex_replace(project_cpp_str , std::regex("<projectname>") , project_name);

    {
        std::ofstream project_cpp_file{ (code_path / (project_name + ".cpp")).string() };
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

    yproj_str = std::regex_replace(yproj_str , std::regex("<projectname>") , project_name);

    {
        std::ofstream yproj_file{ (proj_path / (project_name + ".yproj")).string() };
        yproj_file << yproj_str;
        yproj_file.close();
    }

    return true;
}

void Launcher::ConfigureProject() {
    YE_CRITICAL_ASSERTION(editor != nullptr , "Editor is null");
    if (ImGui::Begin("Project Configuration" , &project_config_open)) {
        ImGui::Text("Project Build Configuration");
        for (const auto& entry : std::filesystem::directory_iterator(project_folder / project_name)) {
            if (entry.is_directory()) continue;
            std::string file_name = entry.path().filename().string();
            if (ImGui::Button(file_name.c_str())) {
                editor->Initialize({ 1.0f , 1.0f });
                editor->LoadFile(entry.path());
                editor_open = true;
            }
        }
        ImGui::Separator();
        ImGui::Text("Project Configuration");
        for (const auto& entry : std::filesystem::directory_iterator(project_folder / project_name / project_name)) {
            if (entry.is_directory()) continue;
            std::string file_name = entry.path().filename().string();
            if (ImGui::Button(file_name.c_str())) {
                editor->Initialize({ 1.0f , 1.0f });
                editor->LoadFile(entry.path());
                editor_open = true;
            }
        }

        if (ImGui::Button("Confirm" , { 100 , 25 })) {
            project_config_open = false;
            editor_open = false;
            editor->Notify("RequestQuit");
            editor->Shutdown();

            // BuildProject(project_name);
            // LaunchProject(project_name);

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
            std::string proj_name{ project_name };
            if (GenerateProjectDirectory(proj_name)) {
                editor = std::make_unique<YE::TextEditor>();
                project_config_open = true;
            } 
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

DECLARE_APP(Launcher);