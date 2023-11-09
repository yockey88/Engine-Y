#ifndef YE_EDITOR_UI_UTILS_HPP
#define YE_EDITOR_UI_UTILS_HPP

#include <imgui/imgui.h>

#define COLOR(r , g , b , a) ImVec4(r / 255.f , g / 255.f , b / 255.f , a / 255.f)

struct EditorStyles {
    inline static constexpr ImVec4 clear_color          = COLOR(0x00 , 0x00 , 0x00 , 0x00);
    inline static constexpr ImVec4 title_bar            = COLOR(0x14 , 0x14 , 0x23 , 0xFF);
    inline static constexpr ImVec4 title_bar_active     = title_bar;
    inline static constexpr ImVec4 title_bar_collapsed  = COLOR(0x03 , 0x03 , 0x17 , 0xFF);
    inline static constexpr ImVec4 menu_bar             = COLOR(0x36 , 0x36 , 0x36 , 0xFF);
    inline static constexpr ImVec4 tab                  = title_bar;
    inline static constexpr ImVec4 tab_hovered          = COLOR(0x54 , 0x57 , 0x5C , 0xFF);
    inline static constexpr ImVec4 tab_active           = COLOR(0x3B , 0x3B , 0x3D , 0xFF);
    inline static constexpr ImVec4 tab_unfocused        = title_bar_collapsed; 
    inline static constexpr ImVec4 tab_unfocused_active = title_bar_collapsed; 
};

void LoadImGuiStyle();

#endif // !YE_EDITOR_UI_UTILS_HPP