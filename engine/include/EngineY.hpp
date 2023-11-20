#ifndef ENGINEY_HPP
#define ENGINEY_HPP

/// Forwarding standard headers to application
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <chrono>
#include <functional>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <filesystem>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <stack>
#include <deque>
#include <atomic>
#include <future>
#include <regex>
#include <coroutine>

#include <SDL.h>
#include <SDL_version.h>
#include <glad/glad.h>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <implot/implot.h>
#include <magic_enum/magic_enum.hpp>
#include <nfd/nfd.h>

#include "engine.hpp"
#include "core/defines.hpp"
#include "core/log.hpp"
#include "core/logger.hpp"
#include "application/app.hpp"
#include "core/defines.hpp"
#include "core/timer.hpp"
#include "core/filesystem.hpp"
#include "rendering/window.hpp"
#include "core/tasks.hpp"
#include "event/event_manager.hpp"
#include "core/task_manager.hpp"
#include "core/resource_handler.hpp"
#include "core/text_editor.hpp"
#include "core/command_line_args.hpp"
#include "core/console.hpp"
#include "parsing/yscript/node_builder.hpp"
#include "parsing/shader/shader_parser.hpp"
#include "event/events.hpp"
#include "event/window_events.hpp"
#include "event/keyboard_events.hpp"
#include "event/mouse_events.hpp"
#include "event/scene_events.hpp"
#include "event/editor_events.hpp"
#include "scripting/native_script_entity.hpp"
#include "scripting/script_engine.hpp"
#include "rendering/vertex.hpp"
#include "rendering/vertex_array.hpp"
#include "rendering/shader.hpp"
#include "rendering/texture.hpp"
#include "rendering/font.hpp"
#include "rendering/framebuffer.hpp"
#include "rendering/camera.hpp"
#include "rendering/model.hpp"
#include "rendering/render_commands.hpp"
#include "rendering/window.hpp"
#include "rendering/renderer.hpp"
#include "scene/scene.hpp"
#include "scene/entity.hpp"
#include "scene/components.hpp"
#include "scene/systems.hpp"
#include "scene/scene_manager.hpp"
#include "world/noise2D.hpp"
#include "world/height_map2D.hpp"
#include "world/terrain_generator.hpp"
                                                                  
int YE2Entry(int argc , char* argv[]);

namespace EY {

    /// Forwarding headers and functions to application for nicer syntax

    /// Convenience macros /////////////////////////////////////////////////////////////////////////////////////////////////
#define MAKE_RENDER_COMMAND(type , ...) std::make_unique<type>(__VA_ARGS__)
#define DRAW(type , ...) std::unique_ptr< EngineY::RenderCommand> command = MAKE_RENDER_COMMAND(type , __VA_ARGS__); \
                          EngineY::Renderer::Instance()->SubmitRenderCmnd(command);
#define ADD_SCRIPT_FUNCTION(class_name , call) mono_add_internal_call(#class_name"::"#call , (void*)call)

     // Getters ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
     EngineY::Window* Window();
     EngineY::Renderer* Renderer();
     EngineY::ResourceHandler* ResourceHandler();
     EngineY::ScriptEngine* ScriptEngine();
     EngineY::EventManager* EventManager();
     EngineY::TaskManager* TaskManager();
     EngineY::PhysicsEngine* PhysicsEngine();
     EngineY::SceneManager* SceneManager();
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /// Engine Functions ////////////////////////////////////////////////////////////////////////////////////////////////////
    void DispatchEvent(EngineY::Event* event);
    void ShaderReload();
    void ScriptReload();
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /// Event Callback Registration //////////////////////////////////////////////////////////////////////////////////////////// 
    void RegisterWindowResizeCallback(std::function<bool(EngineY::WindowResized*)> callback , const std::string& name);
    void RegisterWindowMinimizeCallback(std::function<bool(EngineY::WindowMinimized*)> callback , const std::string& name);
    void RegisterWindowCloseCallback(std::function<bool(EngineY::WindowClosed*)> callback , const std::string& name);
    void RegisterKeyPressCallback(std::function<bool(EngineY::KeyPressed*)> callback , const std::string& name);
    void RegisterKeyReleaseCallback(std::function<bool(EngineY::KeyReleased*)> callback , const std::string& name);
    void RegisterKeyHeldCallback(std::function<bool(EngineY::KeyHeld*)> callback , const std::string& name);
    void RegisterMouseMoveCallback(std::function<bool(EngineY::MouseMoved*)> callback , const std::string& name);
    void RegisterMouseScrollCallback(std::function<bool(EngineY::MouseScrolled*)> callback , const std::string& name);
    void RegisterMouseButtonPresseCallback(std::function<bool(EngineY::MouseButtonPressed*)> callback , const std::string& name);
    void RegisterMouseButtonReleaseCallback(std::function<bool(EngineY::MouseButtonReleased*)> callback , const std::string& name);
    void RegisterMouseButtonHeldCallback(std::function<bool(EngineY::MouseButtonHeld*)> callback , const std::string& name);
    void RegisterSceneLoadCallback(std::function<bool(EngineY::SceneLoad*)> callback , const std::string& name);
    void RegisterSceneStartCallback(std::function<bool(EngineY::SceneStart*)> callback , const std::string& name);
    void RegisterSceneStopCallback(std::function<bool(EngineY::SceneStop*)> callback , const std::string& name);
    void RegisterSceneUnloadCallback(std::function<bool(EngineY::SceneUnload*)> callback , const std::string& name);
    void RegisterEditorPlayCallback(std::function<bool(EngineY::EditorPlay*)> callback , const std::string& name);
    void RegisterEditorPauseCallback(std::function<bool(EngineY::EditorPause*)> callback , const std::string& name);
    void RegisterEditorStopCallback(std::function<bool(EngineY::EditorStop*)> callback , const std::string& name);
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /// Render Callback Registration //////////////////////////////////////////////////////////////////////////////////////////
    void RegisterPreRenderCallback(std::function<void()> callback , const std::string& name);
    void RegisterPostRenderCallback(std::function<void()> callback , const std::string& name);
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /// Framebuffer Functions ////////////////////////////////////////////////////////////////////////////////////////////////
    void PushFrameBuffer(const std::string& name ,  EngineY::Framebuffer* framebuffer);
    void ActivateFrameBuffer(const std::string& name);
    void ActivateLastFrameBuffer();
    void SetDefaultFrameBuffer(const std::string& name);
    void RevertToDefaultFrameBuffer();
    void PopFrameBuffer(const std::string& name);
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /// Resource Functions ///////////////////////////////////////////////////////////////////////////////////////////////////
     EngineY::Shader* GetCoreShader(const std::string& name);
     EngineY::Shader* GetShader(const std::string& name);
     EngineY::Texture* GetCoreTexture(const std::string& name);
     EngineY::Texture* GetTexture(const std::string& name);
     EngineY::VertexArray* GetPrimitiveVAO(const std::string& name);
     EngineY::Model* GetCoreModel(const std::string& name);
     EngineY::Model* GetModel(const std::string& name);
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}
namespace EngineY {

    int Main(int argc , char* argv[] , CreateAppFunc create_app);

} // namespace EngineY

#define DECLARE_APP(X)                                                                                                                       \
    EngineY::App* CreateApp() {                                                                                                              \
        return new X;                                                                                                                       \
    }                                                                                                                                        \
    int YE2Entry(int argc , char* argv[]) {                                                                                                  \
        return EngineY::Main(argc , argv , CreateApp);                                                                                       \
    }
#endif // !ENGINEY_HPP 
