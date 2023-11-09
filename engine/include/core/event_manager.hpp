#ifndef YE_EVENT_MANAGER_HPP
#define YE_EVENT_MANAGER_HPP

#include <unordered_map>
#include <functional>
#include <queue>

#include "UUID.hpp"
#include "log.hpp"
#include "event/events.hpp"
#include "event/window_events.hpp"
#include "event/keyboard_events.hpp"
#include "event/mouse_events.hpp"
#include "event/scene_events.hpp"
#include "event/editor_events.hpp"
#include "input/mouse.hpp"
#include "input/keyboard.hpp"

namespace YE {

    template<typename T>
    using CallbackMap = std::unordered_map<UUID32 , std::function<bool(T*)>>;

    class EventManager {
        
        static EventManager* singleton;

        CallbackMap<WindowResized> window_resized_callbacks;
        CallbackMap<WindowMinimized> window_minimized_callbacks;
        CallbackMap<WindowClosed> window_closed_callbacks;

        CallbackMap<KeyPressed> key_pressed_callbacks;
        CallbackMap<KeyReleased> key_released_callbacks;
        CallbackMap<KeyHeld> key_held_callbacks;

        CallbackMap<MouseMoved> mouse_moved_callbacks;
        CallbackMap<MouseScrolled> mouse_scrolled_callbacks;
        CallbackMap<MouseButtonPressed> mouse_button_pressed_callbacks;
        CallbackMap<MouseButtonReleased> mouse_button_released_callbacks;
        CallbackMap<MouseButtonHeld> mouse_button_held_callbacks;

        CallbackMap<SceneLoad> scene_load_callbacks;
        CallbackMap<SceneStart> scene_start_callbacks;
        CallbackMap<SceneStop> scene_stop_callbacks;
        CallbackMap<SceneUnload> scene_unload_callbacks;

        CallbackMap<EditorPlay> editor_scene_play_callbacks;
        CallbackMap<EditorPause> editor_scene_pause_callbacks;
        CallbackMap<EditorStop> editor_scene_stop_callbacks;

        std::function<bool(ShutdownEvent*)> shutdown_callback;

        std::queue<Event*> dispatched_events;

        template<typename T>
        bool CheckID(UUID32 id , const std::string& name , const CallbackMap<T>& map) {
            if (map.find(id) != map.end()) {
                YE_WARN("Failed to register callback :: [{0}] | Name already exists" , name);
                return false;
            }
            return true;
        }

        EventManager() {}
        ~EventManager() {}

        EventManager(EventManager&&) = delete;
        EventManager(const EventManager&) = delete;
        EventManager& operator=(EventManager&&) = delete;
        EventManager& operator=(const EventManager&) = delete;

        public:

            static EventManager* Instance();

            void RegisterWindowResizedCallback(std::function<bool(WindowResized*)> callback , const std::string& name);
            void RegisterWindowMinimizedCallback(std::function<bool(WindowMinimized*)> callback , const std::string& name);
            void RegisterWindowClosedCallback(std::function<bool(WindowClosed*)> callback , const std::string& name);
            void RegisterKeyPressedCallback(std::function<bool(KeyPressed*)> callback , const std::string& name);
            void RegisterKeyReleasedCallback(std::function<bool(KeyReleased*)> callback , const std::string& name);
            void RegisterKeyHeldCallback(std::function<bool(KeyHeld*)> callback , const std::string& name);
            void RegisterMouseMovedCallback(std::function<bool(MouseMoved*)> callback , const std::string& name);
            void RegisterMouseScrolledCallback(std::function<bool(MouseScrolled*)> callback , const std::string& name);
            void RegisterMouseButtonPressedCallback(std::function<bool(MouseButtonPressed*)> callback , const std::string& name);
            void RegisterMouseButtonReleasedCallback(std::function<bool(MouseButtonReleased*)> callback , const std::string& name);
            void RegisterMouseButtonHeldCallback(std::function<bool(MouseButtonHeld*)> callback , const std::string& name);
            void RegisterSceneLoadCallback(std::function<bool(SceneLoad*)> callback , const std::string& name);
            void RegisterSceneStartCallback(std::function<bool(SceneStart*)> callback , const std::string& name);
            void RegisterSceneStopCallback(std::function<bool(SceneStop*)> callback , const std::string& name);
            void RegisterSceneUnloadCallback(std::function<bool(SceneUnload*)> callback , const std::string& name);
            void RegisterEditorPlayCallback(std::function<bool(EditorPlay*)> callback , const std::string& name);
            void RegisterEditorPauseCallback(std::function<bool(EditorPause*)> callback , const std::string& name);
            void RegisterEditorStopCallback(std::function<bool(EditorStop*)> callback , const std::string& name);
            
            void RegisterShutdownCallback(std::function<bool(ShutdownEvent*)> callback);

            void UnregisterCallback(const std::string& name , EventType type);

            void PollEvents();
            void DispatchEvent(Event* event);
            void FlushEvents();

            void Cleanup();

    };

}

#endif // !YE_EVENT_MANAGER_HPP