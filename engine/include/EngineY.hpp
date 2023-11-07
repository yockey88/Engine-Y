#ifndef YE_HPP
#define YE_HPP

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

#include <SDL.h>
#include <glad/glad.h>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <magic_enum/magic_enum.hpp>
#include <nfd/nfd.h>

#ifdef YE_PLATFORM_WIN
    #include <ShlObj.h>
    #include <minwindef.h>
#ifdef YE_DEBUG_BUILD
    #include "platform/win_crash_handler.hpp"
#endif
#endif

#include "log.hpp"
#include "engine.hpp"
#include "core/logger.hpp"
#include "core/app.hpp"
#include "core/defines.hpp"
#include "core/timer.hpp"
#include "core/filesystem.hpp"
#include "core/window.hpp"
#include "core/tasks.hpp"
#include "core/task_manager.hpp"
#include "core/resource_handler.hpp"
#include "core/text_editor.hpp"
#include "parsing/yscript/node_builder.hpp"
#include "parsing/shader/shader_parser.hpp"
#include "event/events.hpp"
#include "event/window_events.hpp"
#include "event/keyboard_events.hpp"
#include "event/mouse_events.hpp"
#include "event/scene_events.hpp"
#include "event/editor_events.hpp"
#include "event/event_manager.hpp"
#include "scripting/script_engine.hpp"
#include "rendering/vertex.hpp"
#include "rendering/vertex_array.hpp"
#include "rendering/shader.hpp"
#include "rendering/texture.hpp"
#include "rendering/framebuffer.hpp"
#include "rendering/camera.hpp"
#include "rendering/model.hpp"
#include "rendering/render_commands.hpp"
#include "rendering/renderer.hpp"
#include "scene/scene.hpp"
#include "scene/entity.hpp"
#include "scene/components.hpp"
#include "scene/systems.hpp"
#include "scene/native_script_entity.hpp"
#include "world/noise2D.hpp"
#include "world/height_map2D.hpp"
#include "world/terrain_generator.hpp"
                                                                  
int YE2Entry(int argc , char* argv[]);

namespace EngineY {

    /// Forwarding headers and functions to application for nicer syntax

    /// Convenience macros /////////////////////////////////////////////////////////////////////////////////////////////////
#define MAKE_RENDER_COMMAND(type , ...) std::make_unique<type>(__VA_ARGS__)
#define DRAW(type , ...) std::unique_ptr<YE::RenderCommand> command = MAKE_RENDER_COMMAND(type , __VA_ARGS__); \
                         YE::Renderer::Instance()->SubmitRenderCmnd(command);
#define ADD_SCRIPT_FUNCTION(class_name , call) mono_add_internal_call(#class_name"::"#call , (void*)call)
#define EVENT(event) YE::EventManager::Instance()->DispatchEvent(ynew event)

    // Getters ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    YE::Window* Window();
    YE::Renderer* Renderer();
    YE::ResourceHandler* ResourceHandler();
    YE::ScriptEngine* ScriptEngine();
    YE::EventManager* EventManager();
    YE::TaskManager* TaskManager();
    YE::PhysicsEngine* PhysicsEngine();
    /* YE::SceneGraph* */ YE::Scene* ProjectSceneGraph();
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /// Engine Functions ////////////////////////////////////////////////////////////////////////////////////////////////////
    void DispatchEvent(YE::Event* event);
    void ShaderReload();
    void ScriptReload();
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /// Event Callback Registration //////////////////////////////////////////////////////////////////////////////////////////// 
    void RegisterWindowResizeCallback(std::function<bool(YE::WindowResized*)> callback , const std::string& name);
    void RegisterWindowMinimizeCallback(std::function<bool(YE::WindowMinimized*)> callback , const std::string& name);
    void RegisterWindowCloseCallback(std::function<bool(YE::WindowClosed*)> callback , const std::string& name);
    void RegisterKeyPressCallback(std::function<bool(YE::KeyPressed*)> callback , const std::string& name);
    void RegisterKeyReleaseCallback(std::function<bool(YE::KeyReleased*)> callback , const std::string& name);
    void RegisterKeyHeldCallback(std::function<bool(YE::KeyHeld*)> callback , const std::string& name);
    void RegisterMouseMoveCallback(std::function<bool(YE::MouseMoved*)> callback , const std::string& name);
    void RegisterMouseScrollCallback(std::function<bool(YE::MouseScrolled*)> callback , const std::string& name);
    void RegisterMouseButtonPresseCallback(std::function<bool(YE::MouseButtonPressed*)> callback , const std::string& name);
    void RegisterMouseButtonReleaseCallback(std::function<bool(YE::MouseButtonReleased*)> callback , const std::string& name);
    void RegisterMouseButtonHeldCallback(std::function<bool(YE::MouseButtonHeld*)> callback , const std::string& name);
    void RegisterSceneLoadCallback(std::function<bool(YE::SceneLoad*)> callback , const std::string& name);
    void RegisterSceneStartCallback(std::function<bool(YE::SceneStart*)> callback , const std::string& name);
    void RegisterSceneStopCallback(std::function<bool(YE::SceneStop*)> callback , const std::string& name);
    void RegisterSceneUnloadCallback(std::function<bool(YE::SceneUnload*)> callback , const std::string& name);
    void RegisterEditorPlayCallback(std::function<bool(YE::EditorPlay*)> callback , const std::string& name);
    void RegisterEditorPauseCallback(std::function<bool(YE::EditorPause*)> callback , const std::string& name);
    void RegisterEditorStopCallback(std::function<bool(YE::EditorStop*)> callback , const std::string& name);
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /// Render Callback Registration //////////////////////////////////////////////////////////////////////////////////////////
    void RegisterPreRenderCallback(std::function<void()> callback , const std::string& name);
    void RegisterPostRenderCallback(std::function<void()> callback , const std::string& name);
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /// Framebuffer Functions ////////////////////////////////////////////////////////////////////////////////////////////////
    void PushFrameBuffer(const std::string& name , YE::Framebuffer* framebuffer);
    void ActivateFrameBuffer(const std::string& name);
    void ActivateLastFrameBuffer();
    void SetDefaultFrameBuffer(const std::string& name);
    void RevertToDefaultFrameBuffer();
    void PopFrameBuffer(const std::string& name);
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /// Resource Functions ///////////////////////////////////////////////////////////////////////////////////////////////////
    YE::Shader* GetCoreShader(const std::string& name);
    YE::Shader* GetShader(const std::string& name);
    YE::Texture* GetCoreTexture(const std::string& name);
    YE::Texture* GetTexture(const std::string& name);
    YE::VertexArray* GetPrimitiveVAO(const std::string& name);
    YE::Model* GetCoreModel(const std::string& name);
    YE::Model* GetModel(const std::string& name);
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#define DECLARE_APP(X)                                                                                                                       \
    YE::App* CreateApp() {                                                                                                                   \
        return ynew X;                                                                                                                       \
    }                                                                                                                                        \
    int YE2Entry(int argc , char* argv[]) {                                                                                                  \
        /* ConfigurationLoader::LoadCmndLineArgs(argc , argv); */                                                                            \
        YE::Engine* engine = YE::Engine::Instance();                                                                                         \
        engine->RegisterApplication(CreateApp());                                                                                            \
        if (engine->AppLoaded()) {                                                                                                           \
            engine->Initialize();                                                                                                            \
            engine->Run();                                                                                                                   \
            engine->Shutdown();                                                                                                              \
        }                                                                                                                                    \
        return 0;                                                                                                                            \
    }

#endif // !YE_HPP 