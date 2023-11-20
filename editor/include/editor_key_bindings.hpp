#ifndef ENGINEY_EDITOR_KEY_BINDINGS_HPP
#define ENGINEY_EDITOR_KEY_BINDINGS_HPP

#include "EngineY.hpp"

inline static bool EditorKeyBindings(EngineY::KeyPressed* event)  {
    if (event->Key() ==  EngineY::Keyboard::Key::YE_ESCAPE) {
        EY::DispatchEvent(ynew(EngineY::ShutdownEvent , 0));
    }
    if (
        event->Key() ==  EngineY::Keyboard::Key::YE_R && 
        EngineY::Keyboard::LCtrlShiftLayer()
    ) {
        EY::ScriptReload();
    }
    if (
        event->Key() ==  EngineY::Keyboard::Key::YE_S &&
        EngineY::Keyboard::LCtrlShiftLayer()
    ) {
        EY::ShaderReload();
    }
    return true;
}

#endif // !YE_EDITOR_KEY_BINDINGS_HPP