#include "rendering/gui.hpp"

#include <glad/glad.h>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_sdl2.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imguizmo/ImGuizmo.h>
#include <implot/implot.h>

#include "core/log.hpp"
#include "engine.hpp"
#include "core/filesystem.hpp"

namespace EngineY {
    
    void Gui::Initialize(Window* window) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImPlot::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        ImGui::StyleColorsDark((ImGuiStyle*)0);

        io.ConfigWindowsResizeFromEdges = true;
        io.ConfigWindowsMoveFromTitleBarOnly = true;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable; 

        SDL_Window* win = window->GetSDLWindow();
        SDL_GLContext gl_context = window->GetGLContext();
        ImGui_ImplSDL2_InitForOpenGL(win , gl_context);
        ImGui_ImplOpenGL3_Init("#version 460");

        io.IniFilename = Filesystem::GetGuiIniPathCStr();  
    }

    void Gui::Update() {}

    void Gui::BeginRender(SDL_Window* window) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();
        ImGuizmo::BeginFrame();
        
        // ImGui::SetNextWindowBgAlpha(0.0f);
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport() , ImGuiDockNodeFlags_PassthruCentralNode);
    }

    void Gui::Render(Window* window) {}

    void Gui::EndRender(SDL_Window* window , void* gl_context) {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        SDL_GL_MakeCurrent(window , gl_context);
    }

    void Gui::Shutdown() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImPlot::DestroyContext();
        ImGui::DestroyContext();
    }
    
}