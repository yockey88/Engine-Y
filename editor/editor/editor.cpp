#include "EngineY.hpp"

#include "editor_key_bindings.hpp"
#include "ui_utils.hpp"

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
    bool show_file_menu = false;

    YE::Camera* camera = nullptr;
    YE::Shader* shader = nullptr;
    YE::Font* font = nullptr;
    glm::mat4 model{ 1.f };

    YE::Scene scene = YE::Scene("font-atlas-testing");

    std::unique_ptr<YE::EngineConsole> console = nullptr;
    
    bool render = false;
    bool moving_camera = false;
    int count = 0;
    
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

            EngineY::EventManager()->RegisterKeyPressedCallback(
                [&](YE::KeyPressed* event) -> bool {
                    if (
                        event->Key() == YE::Keyboard::Key::YE_C &&
                        YE::Keyboard::LCtrlLayer()
                    ) {
                        if (moving_camera) {
                            camera->UnregisterKeyboardCallback();
                            camera->UnregisterMouseCallback();
                        } else {
                            camera->RegisterKeyboardCallback(CameraKeyboardCallback);
                            camera->RegisterMouseCallback(CameraMouseCallback);
                        }
                        moving_camera = !moving_camera;
                    }
                    return true;
                } , "camera-movement"
            );

            EditorUtils::LoadImGuiStyle();

            console = std::make_unique<YE::EngineConsole>();
            
            std::filesystem::path p =  
                std::filesystem::path(YE::Filesystem::GetEngineResPath()) / 
                "fonts" / "IBMPlexMono" / "BlexMonoNerdFontMono-Regular.ttf"; 
            font = ynew YE::Font(p);
            if (!font->Load(true)) {
                LOG_WARN("Could not load font atlas");
            } else {
                shader = EngineY::GetCoreShader("msdf_text");
                if (shader == nullptr) {
                    LOG_WARN("Could not load shader");
                } 

                scene.InitializeScene();

                camera = scene.AttachCamera("main-camera");
                camera->SetFront({ 0.f , 0.f , -1.f });
                camera->SetUp({ 0.f , 1.f , 0.f });
                camera->SetRight({ 1.f , 0.f , 0.f });
                camera->SetPosition({ 0.f , 0.f , 3.f });
                camera->Recalculate();

                glm::vec4 bgcolor = EngineY::Window()->ClearColor();

                YE::Entity* yock = scene.CreateEntity("yock");

                auto& transform = yock->GetComponent<YE::components::Transform>();
                transform.position = glm::vec3(0.f);
                transform.rotation = glm::vec3(0.f);
                transform.scale = glm::vec3(1.f);

                yock->AddComponent<YE::components::TextComponent>(
                    font , "msdf_text" , "Chris Yockey\nEngine Developer" , 
                    bgcolor , glm::vec4(0.f , 0.f , 1.f , 1.f) ,
                    0.f , 0.2f 
                );

                scene.LoadScene();
                scene.Start();

                model = glm::translate(model , glm::vec3(-0.5f , 0.f , 0.f));
                
                render = true;
            }

            return true;
        }

        void Update(float delta) override {
            scene.Update(delta);
        }
        
        void Draw() override {
            if (render) {
                scene.Draw();
            } 
        }

        void DrawGui() override {
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

            console->DrawGui();
        }

        void Shutdown() override {
            scene.End();
            scene.UnloadScene();
            scene.Shutdown();
            ydelete font;
        }
};

DECLARE_APP(Editor);