#include "EngineY.hpp"

#include "editor_key_bindings.hpp"
#include "ui_utils.hpp"

#include "panels/scene_panel.hpp"

void CameraKeyboardCallback(YE::Camera* camera , float dt) {
    if (!YE::Keyboard::Released(YE::Keyboard::Key::YE_W)) camera->SetPosition(camera->Position() + camera->Front() * camera->Speed());
    if (!YE::Keyboard::Released(YE::Keyboard::Key::YE_S)) camera->SetPosition(camera->Position() - camera->Front() * camera->Speed());
    if (!YE::Keyboard::Released(YE::Keyboard::Key::YE_A)) camera->SetPosition(camera->Position() - camera->Right() * camera->Speed());
    if (!YE::Keyboard::Released(YE::Keyboard::Key::YE_D)) camera->SetPosition(camera->Position() + camera->Right() * camera->Speed());

    camera->Recalculate();
}

void CameraMouseCallback(YE::Camera* camera , float dt) {
    YE::Mouse::SnapToCenter();
    SDL_SetRelativeMouseMode(SDL_TRUE);

    camera->SetLastMouse(camera->Mouse());
    
    int x, y;
    SDL_GetGlobalMouseState(&x, &y);
    camera->SetMousePos({ 
        static_cast<float>(x) , 
        static_cast<float>(y) 
    });

    camera->SetDeltaMouse({ 
        camera->Mouse().x - camera->LastMouse().x ,
        camera->LastMouse().y - camera->Mouse().y
    });

    glm::vec3 front = camera->Front();

    int relx , rely;
    SDL_GetRelativeMouseState(&relx , &rely);
    camera->SetYaw(camera->Yaw() + (relx * camera->Sensitivity()));
    camera->SetPitch(camera->Pitch() - (rely * camera->Sensitivity()));

    if (camera->ConstrainPitch()) {

        if (camera->Pitch() > 89.0f) camera->SetPitch(89.0f);
        if (camera->Pitch() < -89.0f) camera->SetPitch(-89.0f);

    }

    front.x = cos(glm::radians(camera->Yaw())) * cos(glm::radians(camera->Pitch()));
    front.y = sin(glm::radians(camera->Pitch()));
    front.z = sin(glm::radians(camera->Yaw())) * cos(glm::radians(camera->Pitch()));
    camera->SetFront(glm::normalize(front));

    camera->SetRight(glm::normalize(glm::cross(camera->Front() , camera->WorldUp())));
    camera->SetUp(glm::normalize(glm::cross(camera->Right() , camera->Front())));

    camera->Recalculate();
}

class Editor : public YE::App {
    YE::Scene* scene = nullptr;

    std::unique_ptr<editor::ScenePanel> scene_panel = nullptr;
    std::unique_ptr<YE::EngineConsole> console = nullptr;
    
    bool scene_load_successful = false;

    ImVec4 color = { 0.0f , 0.0f , 0.0f , 1.0f };

    public:
        Editor() 
            : YE::App("editor") {}
        ~Editor() override {}

        YE::EngineConfig GetEngineConfig() override {
            YE::EngineConfig config;
            config.window_config.title = "Engine Y Editor";
            config.window_config.size.x = 1920;
            config.window_config.size.y = 1080;
            config.window_config.clear_color = {
                48 / 255.f  , 56 / 255.f , 
                126 / 255.f , 1.0f
            };
            config.window_config.vsync = true;
            config.window_config.rendering_to_screen = true;
            config.window_config.flags |= SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
            return config;
        }

        void PreInitialize() override {
            YE::Filesystem::OverrideResourcePath("editor/editor/resources");
            YE::Filesystem::OverrideProjectModulePath(
                YE::Filesystem::GetEngineRoot() +  
                "/bin/Debug/editor/editor_modules.dll"
            );
        }

        bool Initialize() override {
            EngineY::EventManager()->RegisterKeyPressedCallback(
                EditorKeyBindings , "editor-key-press"
            );

            gui::LoadImGuiStyle();

            console = std::make_unique<YE::EngineConsole>();
           
            scene_panel = std::make_unique<editor::ScenePanel>();
            scene = EngineY::SceneManager()->CurrentScene();

            if (scene == nullptr) {
                ENGINE_WARN("Could not load scene");
                return false;
            } else {
                scene->LoadScene();
                scene->Start();

                scene_load_successful = true;

                scene_panel->SetSceneContext(scene);
            }

            
            return true;
        }

        void Update(float delta) override {
            scene->Update(delta);
        }
        
        void Draw() override {
            scene->Draw();
        }

        void DrawGui() override {
            ImGuiIO& io = ImGui::GetIO();

            if (ImGui::BeginMainMenuBar()) {
                if (ImGui::BeginMenu("File")) {
                    if (ImGui::MenuItem("New Project" , "Ctrl+N")) {}
                    if (ImGui::MenuItem("Open Project")) {}
                    if (ImGui::MenuItem("Save Project")) {}
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Edit")) {
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Options")) {
                    if (ImGui::MenuItem("Reload Shaders" , "Crtl+Shift+S")) { EngineY::ShaderReload(); }
                    if (ImGui::MenuItem("Reload Scripts" , "Crtl+Shift+R")) { EngineY::ScriptReload(); }
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Tools")) {
                    ImGui::EndMenu();
                }

                ImGui::EndMainMenuBar();
            }

            scene_panel->DrawGui(scene_load_successful);
            // if (ImGui::Begin("Scenes")) {
            //     if (ImGui::BeginChild(scene->SceneName().c_str())) { 
            //         ImGui::Text("Scene Name :: %s" , scene->SceneName().c_str());
            //         ImGui::Separator();
            //         ImGui::Separator();

            //         for (auto& [id , ent] : *scene->Entities()) {
            //             auto& name = ent->GetComponent<YE::components::ID>().name;
            //             auto& transform = ent->GetComponent<YE::components::Transform>();
            //             
            //             ImGui::Text("Name :: [%s] | ID :: [%llu]" , name.c_str() , id.uuid);
            //             ImGui::Separator();
            // 
            //             ImGui::Text("Components");
            //             ImGui::Separator();
            //             
            //             DrawTransformComponent(transform);
            //         }
            //     }
            //     ImGui::EndChild();
            // }
            // ImGui::End();
            
            if (ImGui::Begin("Theme Design")) {
                ImGui::ColorEdit4("Color" , &color.x);
            }
            ImGui::End();

            console->DrawGui();
        }

        void Shutdown() override {
            if (scene_load_successful) {
                scene->End();
                scene->UnloadScene();
            }
        }
};

DECLARE_APP(Editor);
