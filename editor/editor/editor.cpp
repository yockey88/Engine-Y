#include "EngineY.hpp"

#include "editor_key_bindings.hpp"
#include "ui_utils.hpp"

class Editor : public YE::App {
    bool show_file_menu = false;

    YE::Camera* camera = nullptr;
    YE::Shader* shader = nullptr;
    YE::Font* font = nullptr;
    glm::mat4 model{ 1.f };

    YE::Scene scene = YE::Scene("font-atlas-testing");

    std::unique_ptr<YE::EngineConsole> console = nullptr;
    
    bool render = false;
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

            EditorUtils::LoadImGuiStyle();

            console = std::make_unique<YE::EngineConsole>();
            
            std::filesystem::path p =  
                std::filesystem::path(YE::Filesystem::GetEngineResPath()) / 
                "fonts" / "IBMPlexMono" / "BlexMonoNerdFontMono-Regular.ttf"; 
            font = ynew YE::Font(p);
            if (!font->Load(true)) {
                LOG_WARN("Could not load font atlas");
            } else {
                shader = EngineY::ResourceHandler()->GetCoreShader("msdf_text");
                if (shader == nullptr) {
                    LOG_WARN("Could not load shader");
                } else {
                    render = true;
                }

                scene.InitializeScene();

                camera = scene.AttachCamera("main-camera");
                camera->SetFront({ 0.f , 0.f , -1.f });
                camera->SetUp({ 0.f , 1.f , 0.f });
                camera->SetRight({ 1.f , 0.f , 0.f });
                camera->SetPosition({ 0.f , 0.f , 3.f });
                camera->Recalculate();

                // YE::Entity* font_atlas = scene.CreateEntity("font-atlas");

                // auto& transform = font_atlas->GetComponent<YE::components::Transform>();
                // transform.position = glm::vec3(0.f);
                // transform.rotation = glm::vec3(0);
                // transform.scale = glm::vec3(1.f);

                // font_atlas->AddComponent<YE::components::TexturedRenderable>(
                //     EngineY::GetPrimitiveVAO("quad") , YE::Material{} , 
                //     "default" , std::vector<YE::Texture*>{ font->GetAtlasTexture() }
                // );

                scene.LoadScene();
                scene.Start();

                model = glm::translate(model , glm::vec3(-0.5f , 0.f , 0.f));
            }

            return true;
        }

        void Update(float delta) override {
            scene.Update(delta);
        }

        void Draw() override {
            if (render) {
                scene.Draw();
                DRAW(
                    YE::DrawFont ,
                    font , shader ,
                    "Yock" , model  
                );
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