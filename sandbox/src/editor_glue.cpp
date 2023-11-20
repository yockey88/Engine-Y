#include "editor_glue.hpp"

#include <iostream>

#include "scripting/script_base.hpp"

void EditorLog(MonoString* msg) {
    std::cout << "[Editor( C# )]> " <<  EngineY::ScriptUtils::MonoStringToStr(msg) << "\n";
}