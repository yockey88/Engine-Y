#include "event/event_manager.hpp"

#include <SDL.h>
#include <imgui/backends/imgui_impl_sdl2.h>

#include "core/RNG.hpp"
#include "core/window.hpp"
#include "rendering/renderer.hpp"

namespace YE {

    EventManager* EventManager::singleton = nullptr;

    EventManager* EventManager::Instance() {
        if (singleton == nullptr) {
            singleton = ynew EventManager;
        }
        return singleton;
    }
        
    void EventManager::RegisterWindowResizedCallback(std::function<bool(WindowResized*)> callback , const std::string& name) {
        UUID32 id = Hash::FNV32(name);
        if (!CheckID<WindowResized>(id , name , window_resized_callbacks)) return;
        window_resized_callbacks[id] = callback;
    }

    void EventManager::RegisterWindowMinimizedCallback(std::function<bool(WindowMinimized*)> callback , const std::string& name) {
        UUID32 id = Hash::FNV32(name);
        if (!CheckID<WindowMinimized>(id , name , window_minimized_callbacks)) return;
        window_minimized_callbacks[id] = callback;
    }

    void EventManager::RegisterWindowClosedCallback(std::function<bool(WindowClosed*)> callback , const std::string& name) {
        UUID32 id = Hash::FNV32(name);
        if (!CheckID<WindowClosed>(id , name , window_closed_callbacks)) return;
        window_closed_callbacks[id] = callback;
    }

    void EventManager::RegisterKeyPressedCallback(std::function<bool(KeyPressed*)> callback , const std::string& name) {
        UUID32 id = Hash::FNV32(name);
        if (!CheckID<KeyPressed>(id , name , key_pressed_callbacks)) return;
        key_pressed_callbacks[id] = callback;
    }

    void EventManager::RegisterKeyReleasedCallback(std::function<bool(KeyReleased*)> callback , const std::string& name) {
        UUID32 id = Hash::FNV32(name);
        if (!CheckID<KeyReleased>(id , name , key_released_callbacks)) return;
        key_released_callbacks[id] = callback;
    }

    void EventManager::RegisterKeyHeldCallback(std::function<bool(KeyHeld*)> callback , const std::string& name) {
        UUID32 id = Hash::FNV32(name);
        if (!CheckID<KeyHeld>(id , name , key_held_callbacks)) return;
        key_held_callbacks[id] = callback;
    }

    void EventManager::RegisterMouseMovedCallback(std::function<bool(MouseMoved*)> callback , const std::string& name) {
        UUID32 id = Hash::FNV32(name);
        if (!CheckID<MouseMoved>(id , name , mouse_moved_callbacks)) return;
        mouse_moved_callbacks[id] = callback;
    }

    void EventManager::RegisterMouseScrolledCallback(std::function<bool(MouseScrolled*)> callback , const std::string& name) {
        UUID32 id = Hash::FNV32(name);
        if (!CheckID<MouseScrolled>(id , name , mouse_scrolled_callbacks)) return;
        mouse_scrolled_callbacks[id] = callback;
    }

    void EventManager::RegisterMouseButtonPressedCallback(std::function<bool(MouseButtonPressed*)> callback , const std::string& name) {
        UUID32 id = Hash::FNV32(name);
        if (!CheckID<MouseButtonPressed>(id , name , mouse_button_pressed_callbacks)) return;
        mouse_button_pressed_callbacks[id] = callback;
    }

    void EventManager::RegisterMouseButtonReleasedCallback(std::function<bool(MouseButtonReleased*)> callback , const std::string& name) {
        UUID32 id = Hash::FNV32(name);
        if (!CheckID<MouseButtonReleased>(id , name , mouse_button_released_callbacks)) return;
        mouse_button_released_callbacks[id] = callback;
    }

    void EventManager::RegisterMouseButtonHeldCallback(std::function<bool(MouseButtonHeld*)> callback , const std::string& name) {
        UUID32 id = Hash::FNV32(name);
        if (!CheckID<MouseButtonHeld>(id , name , mouse_button_held_callbacks)) return;
        mouse_button_held_callbacks[id] = callback;
    }

    void EventManager::RegisterSceneLoadCallback(std::function<bool(SceneLoad*)> callback , const std::string& name) {
        UUID32 id = Hash::FNV32(name);
        if (!CheckID<SceneLoad>(id , name , scene_load_callbacks)) return;
        scene_load_callbacks[id] = callback;
    }

    void EventManager::RegisterSceneStartCallback(std::function<bool(SceneStart*)> callback , const std::string& name) {
        UUID32 id = Hash::FNV32(name);
        if (!CheckID<SceneStart>(id , name , scene_start_callbacks)) return;
        scene_start_callbacks[id] = callback;
    }

    void EventManager::RegisterSceneStopCallback(std::function<bool(SceneStop*)> callback , const std::string& name) {
        UUID32 id = Hash::FNV32(name);
        if (!CheckID<SceneStop>(id , name , scene_stop_callbacks)) return;
        scene_stop_callbacks[id] = callback;
    }

    void EventManager::RegisterSceneUnloadCallback(std::function<bool(SceneUnload*)> callback , const std::string& name) {
        UUID32 id = Hash::FNV32(name);
        if (!CheckID<SceneUnload>(id , name , scene_unload_callbacks)) return;
        scene_unload_callbacks[id] = callback;
    }

    void EventManager::RegisterEditorPlayCallback(std::function<bool(EditorPlay*)> callback , const std::string& name) {
        UUID32 id = Hash::FNV32(name);
        if (!CheckID<EditorPlay>(id , name , editor_scene_play_callbacks)) return;
        editor_scene_play_callbacks[id] = callback;
    }   

    void EventManager::RegisterEditorPauseCallback(std::function<bool(EditorPause*)> callback , const std::string& name) {
        UUID32 id = Hash::FNV32(name);
        if (!CheckID<EditorPause>(id , name , editor_scene_pause_callbacks)) return;
        editor_scene_pause_callbacks[id] = callback;
    }

    void EventManager::RegisterEditorStopCallback(std::function<bool(EditorStop*)> callback , const std::string& name) {
        UUID32 id = Hash::FNV32(name);
        if (!CheckID<EditorStop>(id , name , editor_scene_stop_callbacks)) return;
        editor_scene_stop_callbacks[id] = callback;
    }
    
    void EventManager::RegisterShutdownCallback(std::function<bool(ShutdownEvent*)> callback) {
        shutdown_callback = callback;
    }
    
    void EventManager::UnregisterCallback(const std::string& name , EventType type) {
        UUID32 id = Hash::FNV32(name);
        switch (type) {
            case EventType::WINDOW_RESIZE:
                for (auto it = window_resized_callbacks.begin(); it != window_resized_callbacks.end(); ++it) {
                    if (it->first == id) {
                        window_resized_callbacks.erase(it);
                        break;
                    }
                }
            break;
            case EventType::WINDOW_MINIMIZE:
                for (auto it = window_minimized_callbacks.begin(); it != window_minimized_callbacks.end(); ++it) {
                    if (it->first == id) {
                        window_minimized_callbacks.erase(it);
                        break;
                    }
                }
            break;
            case EventType::WINDOW_CLOSE:
                for (auto it = window_closed_callbacks.begin(); it != window_closed_callbacks.end(); ++it) {
                    if (it->first == id) {
                        window_closed_callbacks.erase(it);
                        break;
                    }
                }
            break;
            case EventType::KEY_PRESSED:
                for (auto it = key_pressed_callbacks.begin(); it != key_pressed_callbacks.end(); ++it) {
                    if (it->first == id) {
                        key_pressed_callbacks.erase(it);
                        break;
                    }
                }
            break;
            case EventType::KEY_RELEASED:
                for (auto it = key_released_callbacks.begin(); it != key_released_callbacks.end(); ++it) {
                    if (it->first == id) {
                        key_released_callbacks.erase(it);
                        break;
                    }
                }
            break;
            case EventType::KEY_HELD:
                for (auto it = key_held_callbacks.begin(); it != key_held_callbacks.end(); ++it) {
                    if (it->first == id) {
                        key_held_callbacks.erase(it);
                        break;
                    }
                }
            break;
            case EventType::MOUSE_MOVD:
                for (auto it = mouse_moved_callbacks.begin(); it != mouse_moved_callbacks.end(); ++it) {
                    if (it->first == id) {
                        mouse_moved_callbacks.erase(it);
                        break;
                    }
                }
            break;
            case EventType::MOUSE_SCROLLED:
                for (auto it = mouse_scrolled_callbacks.begin(); it != mouse_scrolled_callbacks.end(); ++it) {
                    if (it->first == id) {
                        mouse_scrolled_callbacks.erase(it);
                    }
                }
            break;
            case EventType::MOUSE_BUTTON_PRESSED:
                for (auto it = mouse_button_pressed_callbacks.begin(); it != mouse_button_pressed_callbacks.end(); ++it) {
                    if (it->first == id) {
                        mouse_button_pressed_callbacks.erase(it);
                        break;
                    }
                }
            break;
            case EventType::MOUSE_BUTTON_RELEASED:
                for (auto it = mouse_button_released_callbacks.begin(); it != mouse_button_released_callbacks.end(); ++it) {
                    if (it->first == id) {
                        mouse_button_released_callbacks.erase(it);
                        break;
                    }
                }
            break;
            case EventType::MOUSE_BUTTON_HELD:
                for (auto it = mouse_button_held_callbacks.begin(); it != mouse_button_held_callbacks.end(); ++it) {
                    if (it->first == id) {
                        mouse_button_held_callbacks.erase(it);
                        break;
                    }
                }
            break;
            case EventType::SCENE_LOAD:
                for (auto it = scene_load_callbacks.begin(); it != scene_load_callbacks.end(); ++it) {
                    if (it->first == id) {
                        scene_load_callbacks.erase(it);
                        break;
                    }
                }
            break;
            case EventType::SCENE_START:
                for (auto it = scene_start_callbacks.begin(); it != scene_start_callbacks.end(); ++it) {
                    if (it->first == id) {
                        scene_start_callbacks.erase(it);
                        break;
                    }
                }
            break;
            case EventType::SCENE_STOP:
                for (auto it = scene_stop_callbacks.begin(); it != scene_stop_callbacks.end(); ++it) {
                    if (it->first == id) {
                        scene_stop_callbacks.erase(it);
                        break;
                    }
                }
            break;
            case EventType::SCENE_UNLOAD:
                for (auto it = scene_unload_callbacks.begin(); it != scene_unload_callbacks.end(); ++it) {
                    if (it->first == id) {
                        scene_unload_callbacks.erase(it);
                        break;
                    }
                }
            break;
            case EventType::EDITOR_SCENE_PLAY:
                for (auto it = editor_scene_play_callbacks.begin(); it != editor_scene_play_callbacks.end(); ++it) {
                    if (it->first == id) {
                        editor_scene_play_callbacks.erase(it);
                        break;
                    }
                }
            break;
            case EventType::EDITOR_SCENE_PAUSE:
                for (auto it = editor_scene_pause_callbacks.begin(); it != editor_scene_pause_callbacks.end(); ++it) {
                    if (it->first == id) {
                        editor_scene_pause_callbacks.erase(it);
                        break;
                    }
                }
            break;
            case EventType::EDITOR_SCENE_STOP:
                for (auto it = editor_scene_stop_callbacks.begin(); it != editor_scene_stop_callbacks.end(); ++it) {
                    if (it->first == id) {
                        editor_scene_stop_callbacks.erase(it);
                        break;
                    }
                }
            break;
            default: break;
        }
    }
    
    void EventManager::PollEvents() {
        SDL_Event e;
        std::unique_ptr<Event> event = nullptr;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_WINDOWEVENT:
                    if (e.window.event == SDL_WINDOWEVENT_RESIZED)
                        DispatchEvent(ynew WindowResized( 
                            { e.window.data1 , e.window.data2 } ,
                            Renderer::Instance()->ActiveWindow()->GetSize()
                        ));
                    if (e.window.event == SDL_WINDOWEVENT_MINIMIZED)
                        DispatchEvent(ynew WindowMinimized);
                    if (e.window.event == SDL_WINDOWEVENT_CLOSE)
                        DispatchEvent(ynew WindowClosed);
                break;
                case SDL_QUIT:
                    DispatchEvent(ynew ShutdownEvent);
                break;
                default: break;
            }
            ImGui_ImplSDL2_ProcessEvent(&e);
        }
    }

    void EventManager::DispatchEvent(Event* event) {
        switch (event->Type()) {
            case EventType::WINDOW_RESIZE:
                for (auto& callback : window_resized_callbacks)
                    callback.second(dynamic_cast<WindowResized*>(event));
            break;
            case EventType::WINDOW_MINIMIZE:
                for (auto& callback : window_minimized_callbacks)
                    callback.second(dynamic_cast<WindowMinimized*>(event));
            break;
            case EventType::WINDOW_CLOSE:
                for (auto& callback : window_closed_callbacks)
                    callback.second(dynamic_cast<WindowClosed*>(event));
            break;
            case EventType::KEY_PRESSED:
                for (auto& callback : key_pressed_callbacks)
                    callback.second(dynamic_cast<KeyPressed*>(event));
            break;
            case EventType::KEY_RELEASED:
                for (auto& callback : key_released_callbacks)
                    callback.second(dynamic_cast<KeyReleased*>(event));
            break;
            case EventType::KEY_HELD:
                for (auto& callback : key_held_callbacks)
                    callback.second(dynamic_cast<KeyHeld*>(event));
            break;
            case EventType::MOUSE_MOVD:
                for (auto& callback : mouse_moved_callbacks)
                    callback.second(dynamic_cast<MouseMoved*>(event));
            break;
            case EventType::MOUSE_SCROLLED:
                for (auto& callback : mouse_scrolled_callbacks)
                    callback.second(dynamic_cast<MouseScrolled*>(event));
            break;
            case EventType::MOUSE_BUTTON_PRESSED:
                for (auto& callback : mouse_button_pressed_callbacks)
                    callback.second(dynamic_cast<MouseButtonPressed*>(event));
            break;
            case EventType::MOUSE_BUTTON_RELEASED:
                for (auto& callback : mouse_button_released_callbacks)
                    callback.second(dynamic_cast<MouseButtonReleased*>(event));
            break;
            case EventType::MOUSE_BUTTON_HELD:
                for (auto& callback : mouse_button_held_callbacks)
                    callback.second(dynamic_cast<MouseButtonHeld*>(event));
            break;
            case EventType::SCENE_LOAD:
                for (auto& callback : scene_load_callbacks)
                    callback.second(dynamic_cast<SceneLoad*>(event));
            break;
            case EventType::SCENE_START:
                for (auto& callback : scene_start_callbacks)
                    callback.second(dynamic_cast<SceneStart*>(event));
            break;
            case EventType::SCENE_STOP:
                for (auto& callback : scene_stop_callbacks)
                    callback.second(dynamic_cast<SceneStop*>(event));
            break;
            case EventType::SCENE_UNLOAD:
                for (auto& callback : scene_unload_callbacks)
                    callback.second(dynamic_cast<SceneUnload*>(event));
            break;
            case EventType::EDITOR_SCENE_PLAY:
                for (auto& callback : editor_scene_play_callbacks)
                    callback.second(dynamic_cast<EditorPlay*>(event));
            break;
            case EventType::EDITOR_SCENE_PAUSE:
                for (auto& callback : editor_scene_pause_callbacks)
                    callback.second(dynamic_cast<EditorPause*>(event));
            break;
            case EventType::EDITOR_SCENE_STOP:
                for (auto& callback : editor_scene_stop_callbacks)
                    callback.second(dynamic_cast<EditorStop*>(event));
            break;
            case EventType::SHUTDOWN:
                shutdown_callback(dynamic_cast<ShutdownEvent*>(event));
            break;
            default: break;
        }

        dispatched_events.push(event);
    }

    void EventManager::FlushEvents() {
        while (!dispatched_events.empty()) {
            Event* event = dispatched_events.front();
            dispatched_events.pop();
            ydelete event;
        }
    }
    
    void EventManager::Cleanup() {
        window_resized_callbacks.clear();
        window_minimized_callbacks.clear();
        window_closed_callbacks.clear();
        key_pressed_callbacks.clear();
        key_released_callbacks.clear();
        key_held_callbacks.clear();
        mouse_moved_callbacks.clear();
        mouse_scrolled_callbacks.clear();
        mouse_button_pressed_callbacks.clear();
        mouse_button_released_callbacks.clear();
        mouse_button_held_callbacks.clear();
        scene_load_callbacks.clear();
        scene_start_callbacks.clear();
        scene_stop_callbacks.clear();
        scene_unload_callbacks.clear();
        editor_scene_play_callbacks.clear();
        editor_scene_pause_callbacks.clear();
        editor_scene_stop_callbacks.clear();
        if (singleton != nullptr) ydelete singleton;
    }

}