#ifndef YE_TEXT_EDITOR_HPP
#define YE_TEXT_EDITOR_HPP

#include <memory>
#include <string>
#include <filesystem>
#include <functional>

#include <zep/editor.h>
#include <config_app.h>
#include <zep/imgui/console_imgui.h>

namespace YE {

    struct ZepWrapper : public Zep::IZepComponent {
        Zep::ZepEditor_ImGui editor;
        std::function<void(std::shared_ptr<Zep::ZepMessage>)> message_callback;
        
        ZepWrapper(const std::filesystem::path& root , const Zep::NVec2f& pixel_scale , std::function<void(std::shared_ptr<Zep::ZepMessage>)> message_callback) 
                : editor(std::filesystem::path(root.string()) , pixel_scale) , message_callback(message_callback) {
            editor.RegisterCallback(this);
        }

        virtual Zep::ZepEditor& GetEditor() const override {
            return (Zep::ZepEditor&)editor;
        }

        virtual void Notify(std::shared_ptr<Zep::ZepMessage> message) override {
            message_callback(message);
        }

        virtual void HandleInput() {
            editor.HandleInput();
        }
    };

    class TextEditor {
        std::unique_ptr<ZepWrapper> zep = nullptr;

        public:
            TextEditor() {}
            ~TextEditor() {}

            void Initialize(const std::string& path , Zep::NVec2f pixel_scale);
            void LoadFile(const std::filesystem::path& path);
            void Update();
            void Draw();
            void Shutdown();
    };

} // namespace YE

#endif // !YE_TEXT_EDITOR_HPP