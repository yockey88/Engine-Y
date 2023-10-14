#include "EngineY.hpp"

#include "editor_glue.hpp"
#include "test_native_script.hpp"

void CameraKeyboardCallback(YE::Camera* camera , float dt) {
    if (!YE::Keyboard::Released(YE::Keyboard::Key::YE_W)) camera->SetPosition(camera->Position() + camera->Front() * camera->Speed());
    if (!YE::Keyboard::Released(YE::Keyboard::Key::YE_S)) camera->SetPosition(camera->Position() - camera->Front() * camera->Speed());
    if (!YE::Keyboard::Released(YE::Keyboard::Key::YE_A)) camera->SetPosition(camera->Position() - camera->Right() * camera->Speed());
    if (!YE::Keyboard::Released(YE::Keyboard::Key::YE_D)) camera->SetPosition(camera->Position() + camera->Right() * camera->Speed());
    if (!YE::Keyboard::Released(YE::Keyboard::Key::YE_LSHIFT)) camera->SetPosition(camera->Position() + camera->Up() * camera->Speed());
    if (!YE::Keyboard::Released(YE::Keyboard::Key::YE_LCTRL)) camera->SetPosition(camera->Position() - camera->Up() * camera->Speed());

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

class Sandbox : public YE::App {
    bool scene_controls = true;

    YE::Scene* scene = nullptr;
    bool scene_created = false;

    public:
        Sandbox() {}
        virtual ~Sandbox() override {}

        virtual YE::WindowConfig GetWindowConfig() override {
            YE::WindowConfig config;
            config.title = "Engine Y";
            config.size.x = 1920;
            config.size.y = 1080;
            config.clear_color = { 0.1f , 0.1f , 0.1f , 1.f };
            config.fullscreen = false;
            config.vsync = false;
            config.rendering_to_screen = true;
            config.flags |= SDL_WINDOW_RESIZABLE;
            return config;
        }

        virtual void Initialize() override {
            EngineY::RegisterKeyPressCallback(
                [&](YE::KeyPressed* event) -> bool {
                    if (event->Key() == YE::Keyboard::Key::YE_ESCAPE)
                        EngineY::DispatchEvent(ynew YE::ShutdownEvent);
                    if (event->Key() == YE::Keyboard::Key::YE_F1)
                        EngineY::ScriptReload();
                    return true;
                } ,
                "editor-keys"
            );

            scene = EngineY::ProjectSceneGraph();

            if (scene != nullptr) {
                scene->LoadScene();
                scene->Start();

                YE::Camera* camera = scene->GetCamera("main_camera");
                if (camera != nullptr) {
                    // camera->RegisterKeyboardCallback(CameraKeyboardCallback);
                    // camera->RegisterMouseCallback(CameraMouseCallback);
                }
                scene_created = true;
            }
        }
        
        virtual void Update(float dt) override {
            if (scene_created) {
                scene->Update(dt);
            }
        }

        virtual void Draw() override {
            if (scene_created) {
                scene->Draw();
            }
        }
        
        virtual void DrawGui() override {
            if (ImGui::Begin("Scene Controls" , &scene_controls , ImGuiWindowFlags_MenuBar)) {
                if (ImGui::Button("Destroy Scene")) {
                    if (scene_created) {
                        scene->UnloadScene();
                        scene->Shutdown();
                        ydelete scene;
                        scene_created = false;
                    }
                }

                if (scene_created) {
                    ImGui::TextColored({ 0.f , 1.f , 0.f , 1.f } , scene->SceneName().c_str());

                    for (auto& [id , ent] : (*scene->Entities())) {
                        auto& id = ent->GetComponent<YE::components::ID>();
                        auto& transform = ent->GetComponent<YE::components::Transform>();

                        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
                        if (ImGui::TreeNodeEx((void*)id.id.uuid , flags , id.name.c_str())) {
                            ImGui::Selectable(id.name.c_str() , false);
                            ImGui::Text("[%llu]" , id.id.uuid);
                            ImGui::Text("Position :: [%f , %f , %f]" , transform.position.x , transform.position.y , transform.position.z);
                            ImGui::Text("Rotation :: [%f , %f , %f]" , transform.rotation.x , transform.rotation.y , transform.rotation.z);
                            ImGui::Text("Scale :: [%f , %f , %f]" , transform.scale.x , transform.scale.y , transform.scale.z);
                            ImGui::Separator();

                            ImGui::TreePop();
                        }
                    }
                }
            }
            ImGui::End();
        }

        virtual void Shutdown() override {
            if (scene_created) {
                scene->UnloadScene();
                scene->Shutdown();
                ydelete scene;
            }
        }
};

YE::App* CreateApp() {
    return ynew Sandbox;
}