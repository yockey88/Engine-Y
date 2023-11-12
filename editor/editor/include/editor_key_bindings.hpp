#ifndef YE_EDITOR_KEY_BINDINGS_HPP
#define YE_EDITOR_KEY_BINDINGS_HPP

#include "event/keyboard_events.hpp"

inline static bool EditorKeyBindings(YE::KeyPressed* event)  {
    if (event->Key() == YE::Keyboard::Key::YE_ESCAPE) {
        EngineY::DispatchEvent(ynew YE::ShutdownEvent);
    }
    if (
        event->Key() == YE::Keyboard::Key::YE_R && 
        YE::Keyboard::LCtrlShiftLayer()
    ) {
        EngineY::ScriptReload();
    }
    if (
        event->Key() == YE::Keyboard::Key::YE_S &&
        YE::Keyboard::LCtrlShiftLayer()
    ) {
        EngineY::ShaderReload();
    }
    return true;
}

#endif // !YE_EDITOR_KEY_BINDINGS_HPP