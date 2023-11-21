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

    EngineY::Ref<EngineY::VertexArray> vao = nullptr;
    EngineY::Shader* shader = nullptr;

    glm::mat4 model = glm::mat4(1.0f);
   
    EngineY::components::Renderable renderable;

    EngineY::Camera* camera = nullptr;

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

            vao = EY::ResourceHandler()->GetPrimitiveVAO("icosahedron"); 

            shader = EY::ResourceHandler()->GetShader("default");

            if (shader == nullptr) {
                ENGINE_ERROR("Failed to load default shader");
            }

            renderable.vao = EngineY::Ref<EngineY::VertexArray>::WriteTo(vao);
            renderable.shader = shader; 
            renderable.shader_name = "default";

            camera = ynew(
                EngineY::Camera ,
                "editor-camera" 
            );
            
            glLineWidth(16.f);
    
            return true;
        }

        void Start() override {
        }

        void Update(float dt) override {
            model = glm::rotate(model , glm::radians(1.0f) , glm::vec3(0.0f , 1.0f , 0.0f));
        }

        void Draw() override {
            EY::Renderer()->PushCamera(camera);
            DRAW(
                EngineY::DrawRenderable ,
                renderable , model
            );
        }

        void DrawGui() override {
            main_menu_bar->DrawGui(scene_panel_open);
            // scene_panel->DrawGui(scene_panel_open);
            // console->DrawGui();
        }

        void Stop() override {
        }

        void Shutdown() override {
            ydelete(camera);
        }
};

DECLARE_APP(Editor);
