#include "rendering/gui.hpp"

#include <glad/glad.h>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_sdl2.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include "log.hpp"
#include "engine.hpp"
#include "core/filesystem.hpp"
#include "event/event_manager.hpp"

namespace YE {

    struct GuiState{
        bool show_stats = false;
    };

    static GuiState* gui_state = nullptr;

    void Gui::RenderMainMenuBar(Window* window) {
        EventManager* event_manager = EventManager::Instance();
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Options")) {
                if (ImGui::MenuItem("Close"))
                    event_manager->DispatchEvent(ynew ShutdownEvent);
                if (ImGui::MenuItem("Stats" , nullptr , &gui_state->show_stats)) {}
                
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }
    }

    void Gui::RenderMainWindow(Window* window) {
        if (ImGui::Begin("YE2" , nullptr , ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBackground | 
                                            ImGuiWindowFlags_NoBringToFrontOnFocus)) {
            ImGui::Text("YE2");
            ImGui::Separator();
            ImGui::Text("FPS: %.1f" , ImGui::GetIO().Framerate);
            ImGui::Text("Frame Time: %.3f ms" , 1000.0f / ImGui::GetIO().Framerate);
            ImGui::Separator();
            ImGui::Text("Renderer :: %s >> [%s]" , glGetString(GL_VENDOR) , glGetString(GL_RENDERER));
            ImGui::SameLine();
            ImGui::Text("OpenGL :: %s | GLSL :: %s" , glGetString(GL_VERSION) , glGetString(GL_SHADING_LANGUAGE_VERSION));
            ImGui::Separator();
            ImGui::Text("Window Size: %d x %d" , window->GetSize().x , window->GetSize().y);
            ImGui::Separator();
            ImGui::Separator();
        }
        ImGui::End();

        Engine* engine = Engine::Instance();
        if (gui_state->show_stats) {
            if (ImGui::Begin("Engine Stats")) {
                ImGui::PlotLines(
                    "Frame Times" , 
                    engine->GetStats()->frame_times , 
                    IM_ARRAYSIZE(engine->GetStats()->frame_times)    
                );
            }
            ImGui::End();
        }
    }

    void Gui::Initialize(Window* window) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        ImGui::StyleColorsDark((ImGuiStyle*)0);

        io.ConfigWindowsResizeFromEdges = true;
        io.ConfigWindowsMoveFromTitleBarOnly = true;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable | 
                          ImGuiConfigFlags_NavEnableKeyboard;

        SDL_Window* win = window->GetSDLWindow();
        SDL_GLContext gl_context = window->GetGLContext();
        ImGui_ImplSDL2_InitForOpenGL(win , gl_context);
        ImGui_ImplOpenGL3_Init("#version 460");

        io.IniFilename = YE::Filesystem::GetGuiIniPathCStr();        

        gui_state = ynew GuiState;
    }

    void Gui::Update() {

    }

    void Gui::BeginRender(SDL_Window* window) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();
        ImGui::SetNextWindowBgAlpha(0.0f);
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
    }

    void Gui::Render(Window* window) {
        RenderMainMenuBar(window);
        RenderMainWindow(window);
    }

    void Gui::EndRender(SDL_Window* window , void* gl_context) {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            SDL_GL_MakeCurrent(window , gl_context);
        }
    }

    void Gui::Shutdown() {
        ydelete gui_state;

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
    }

}