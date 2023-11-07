#ifndef YE_EDITOR_UI_UTILS_HPP
#define YE_EDITOR_UI_UTILS_HPP

#define COLOR(r , g , b , a) ImVec4(r / 255.f , g / 255.f , b / 255.f , a / 255.f)

struct EditorStyles {
    inline static constexpr ImVec4 clear_color = COLOR(0x00 , 0x00 , 0x00 , 0x00);
    inline static constexpr ImVec4 tab_hovered = COLOR(0x46 , 0x44 , 0x44 , 0xFF);
    inline static constexpr ImVec4 tab         = COLOR(0x22 , 0x20 , 0x20 , 0xFF);
};

void LoadImGuiStyle();

#endif // !YE_EDITOR_UI_UTILS_HPP