#ifndef YE_TEXT_EDITOR_HPP
#define YE_TEXT_EDITOR_HPP

#include <memory>

#include <zep/editor.h>
#include <zep/imgui/editor_imgui.h>
#include <zep/imgui/console_imgui.h>

namespace YE {
    std::unique_ptr<Zep::ZepConsole> console;
    std::unique_ptr<Zep::ZepEditor_ImGui> editor;

    class TextEditor {
        public:
            TextEditor() {}
            ~TextEditor() {}

            void Initialize(const std::string& path /* configuration/file/directory */);
            void Update();
            void Draw();
            void Shutdown();
    };

} // namespace YE

#endif // !YE_TEXT_EDITOR_HPP