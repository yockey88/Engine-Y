#include "EngineY.hpp"

#include "editor_key_bindings.hpp"
#include "ui_utils.hpp"

#include "panels/main_menu_bar.hpp"
#include "panels/scene_panel.hpp"

void CameraKeyboardCallback(EngineY::Camera* camera , float dt) {
    if (!EngineY::Keyboard::Released(EngineY::Keyboard::Key::YE_W)) 
        camera->SetPosition(camera->Position() + camera->Front() * camera->Speed());
    if (!EngineY::Keyboard::Released(EngineY::Keyboard::Key::YE_S)) 
        camera->SetPosition(camera->Position() - camera->Front() * camera->Speed());
    if (!EngineY::Keyboard::Released(EngineY::Keyboard::Key::YE_A)) 
        camera->SetPosition(camera->Position() - camera->Right() * camera->Speed());
    if (!EngineY::Keyboard::Released(EngineY::Keyboard::Key::YE_D)) 
        camera->SetPosition(camera->Position() + camera->Right() * camera->Speed());
    if (!EngineY::Keyboard::Released(EngineY::Keyboard::Key::YE_LSHIFT)) 
        camera->SetPosition(camera->Position() + camera->Up() * camera->Speed());
    if (!EngineY::Keyboard::Released(EngineY::Keyboard::Key::YE_LCTRL)) 
        camera->SetPosition(camera->Position() - camera->Up() * camera->Speed());

    camera->Recalculate();
}

void CameraMouseCallback(EngineY::Camera* camera , float dt) {
    EngineY::Mouse::SnapToCenter();
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

class Editor 
        : public EngineY::App {
    std::unique_ptr<editor::EditorPanel> main_menu_bar = nullptr;

    bool scene_panel_open = true;
    std::unique_ptr<editor::EditorPanel> scene_panel = nullptr;

    // once we move UI into the engine update this
    std::unique_ptr<EngineY::EngineConsole> console = nullptr;

    ImVec4 color = { 0.0f , 0.0f , 0.0f , 1.0f };

    std::vector<float> fb_verts;
    std::vector<uint32_t> fb_indices;
    std::vector<uint32_t> fb_layout;
    EngineY::Framebuffer* framebuffer = nullptr; 
    EngineY::Shader* fb_shader = nullptr;
    
    public:
        Editor() : EngineY::App("Editor") {}
        ~Editor() override {}

        bool Initialize() override {
            EY::EventManager()->RegisterKeyPressedCallback(
                EditorKeyBindings , "editor-key-press"
            );

            gui::LoadImGuiStyle();

            console = std::make_unique<EngineY::EngineConsole>();
          
            main_menu_bar = std::make_unique<editor::MainMenuBar>();
            scene_panel = std::make_unique<editor::ScenePanel>();

            fb_verts = {
                 1.f ,  1.f , 1.f , 1.f ,
                 1.f , -1.f , 1.f , 0.f ,
                -1.f , -1.f , 0.f , 0.f ,
                -1.f ,  1.f , 0.f , 1.f
            };

            fb_indices = {
                0 , 1 , 3 ,
                1 , 2 , 3
            };

            fb_layout = {
                2 , 2
            };
            
            fb_shader = EY::ResourceHandler()->GetShader("default_framebuffer");

            if (fb_shader == nullptr) {
                ENGINE_ERROR("Failed to load default framebuffer shader");
            }

            framebuffer = ynew(
                EngineY::Framebuffer ,
                fb_verts , fb_indices , 
                fb_layout , 
                glm::ivec2{ 200 , 200 }
            );    
            return true;
        }

        void Start() override {
            framebuffer->SetBufferType({
                EngineY::BufferBit::COLOR_BUFFER ,
                EngineY::BufferBit::DEPTH_BUFFER ,
                EngineY::BufferBit::STENCIL_BUFFER
            });

            framebuffer->SetClearColor({ 0.3f , 0.2f , 0.1f , 1.0f });

            framebuffer->AttachShader(fb_shader);

            // EY::Renderer()->PushFramebuffer("editor-fb" , framebuffer);
        }

        void Update(float dt) override {
        }

        void Draw() override {
        }

        void DrawGui() override {
            main_menu_bar->DrawGui(scene_panel_open);
    
            uint32_t fb_tex = framebuffer->GetColorAttachment();
            ImGui::Begin("Framebuffer");
            ImGui::Image(
                (void*)(intptr_t)fb_tex ,
                ImVec2(ImGui::GetWindowWidth() , ImGui::GetWindowHeight()) ,
                ImVec2(0 , 1) ,
                ImVec2(1 , 0)
            );
            ImGui::End();

            // scene_panel->DrawGui(scene_panel_open);
            // console->DrawGui();
        }

        void Stop() override {
        }

        void Shutdown() override {
        }
};

DECLARE_APP(Editor);
