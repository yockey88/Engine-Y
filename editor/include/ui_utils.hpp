#ifndef ENGINEY_EDITOR_UI_UTILS_HPP
#define ENGINEY_EDITOR_UI_UTILS_HPP

#include <cstdint>
#include <spdlog/fmt/bundled/core.h>
#include <utility>
#include <string>
#include <string_view>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <choc/text/choc_StringUtilities.h>
#include <spdlog/fmt/fmt.h>

#define COLOR(r , g , b , a) ImVec4(r / 255.f , g / 255.f , b / 255.f , a / 255.f)

struct EditorStyles {
    inline static constexpr ImVec4 clear_color          = COLOR(0x00 , 0x00 , 0x00 , 0x00);
    inline static constexpr ImVec4 title_bar            = COLOR(0x22 , 0x22 , 0x22 , 0xFF);
    inline static constexpr ImVec4 title_bar_active     = title_bar;
    inline static constexpr ImVec4 title_bar_collapsed  = COLOR(0x17 , 0x14 , 0x14 , 0xFF);
    inline static constexpr ImVec4 menu_bar             = COLOR(0x36 , 0x36 , 0x36 , 0xFF);
    inline static constexpr ImVec4 tab                  = title_bar;
    inline static constexpr ImVec4 tab_hovered          = title_bar;
    inline static constexpr ImVec4 tab_active           = title_bar;
    inline static constexpr ImVec4 tab_unfocused        = title_bar_collapsed; 
    inline static constexpr ImVec4 tab_unfocused_active = title_bar_collapsed; 
};

/// \todo move this to Engine for game UI component stuff
namespace gui {

    void LoadImGuiStyle();
    
    struct GuiData {
        uint32_t context_id = 0;
        uint32_t id_counter = 0;
        char id_buffer[16 + 2 + 1] = "##";
        char label_buffer[1024 + 1];
    };

    static GuiData gui_data;
    
    const char* NewID();
    const char* NewLabelID(std::string_view label);
    
    void PushID();
    void PopID();
    
    bool IsInputEnabled();
    void SetInputEnabled(bool enabled);  
    
    bool MatchesSearch(
        const std::string& item, std::string_view search_query, 
        bool case_sensitive = false, bool strip_white_spaces = false, 
        bool strip_underscores = false
    );
   
    class ScopedID {
        public:
            ScopedID(const ScopedID&) = delete;
            ScopedID& operator=(const ScopedID&) = delete;

            template<typename T>
            ScopedID(T id) { ImGui::PushID(id); }
            ~ScopedID() { ImGui::PopID(); }
    };

    class ScopedFont {
        bool pushed = false;

        public:
            ScopedFont(ImFont* font);
            ~ScopedFont();
    };

    class ScopedStyle {        
        bool pushed = false;

        public:
            ScopedStyle(ImGuiStyleVar var , float new_val);
            ScopedStyle(ImGuiStyleVar var , const ImVec2& new_col);
            ~ScopedStyle(); 
    };

    class ScopedColor {
        public:
            ScopedColor(ImGuiCol col , uint32_t new_col);
            ScopedColor(ImGuiCol col , const ImVec4& new_col);
            ~ScopedColor();
    };

    class ScopedColorStack {
        uint32_t count;

        template <typename ColorType , typename... Colors>
        void PushColor(ImGuiCol col , ColorType color , Colors&&... colors) {
            if constexpr (sizeof... (colors) == 0) {
                ImGui::PushStyleColor(col , ImColor(color).Value);
            } else {
                ImGui::PushStyleColor(col , ImColor(color).Value);
                PushColor(std::forward<Colors>(colors)...);
            }
        }

        public:
            ScopedColorStack(const ScopedColorStack&) = delete;
            ScopedColorStack& operator=(const ScopedColorStack&) = delete;

            template <typename ColorType , typename... Colors>
            ScopedColorStack(ImGuiCol first_color , ColorType first , Colors&&... colors) 
                    : count((sizeof... (colors) / 2) + 1) {
                static_assert(
                    (sizeof... (colors) & 1u) == 0 ,
                    "ScopedColorStack requires an even number of arguments"
                );

                PushColor(first_color , first , std::forward<Colors>(colors)...);
            }

            ~ScopedColorStack();
    };

namespace draw {
        void ShiftCursor(float x , float y);
        void ShiftCursorX(float x);
        void ShiftCursorY(float y);
        void Underline(bool full_width = false , float offx = 0.f , float offy = -1.f);

        bool BeginTreeNode(const char* name, bool default_open);    
        void EndTreeNode();    
       
        bool ColoredButton(const char* label, const ImVec4& backgroundColor, ImVec2 buttonSize);    
        bool ColoredButton(
            const char* label , const ImVec4& background , 
            const ImVec4& foreground , ImVec2 size
        );    
       
        bool TableRowClickable(const char* id, float rowHeight);
        
        void Separator(ImVec2 size, ImVec4 color);
        
        bool IsWindowFocused(const char* window_name, const bool check_root_window);
        
        void HelpMarker(const char* desc);
   
        void BeginPropertyGrid(
            uint32_t columns , 
            const ImVec2& item_spacing = ImVec2(8.f , 8.f) , 
            const ImVec2& frame_pading = ImVec2(4.f , 4.f)
       );

       void EndPropertyGrid(); 
        
        // once i have a better texture implementation implement this
        // bool ImageButton(const Ref<Texture2D>& texture, const ImVec2& size, const ImVec4& tint) {
        // 	return ImageButton(texture, size, ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), tint);
        // }
} // namespace draw

} // namespace gui

#endif // !YE_EDITOR_UI_UTILS_HPP
