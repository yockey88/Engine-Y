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
    }

    void Gui::Update() {}

    void Gui::BeginRender(SDL_Window* window) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
    }

    void Gui::Render(Window* window) {}

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
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
    }

}