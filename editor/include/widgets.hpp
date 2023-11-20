#ifndef ENGINEY_EDITOR_WIDGETS_HPP
#define ENGINEY_EDITOR_WIDGETS_HPP

#include <cstdint>
#include <cstdio>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include "ui_utils.hpp"

namespace gui {

    class Widgets {
        public:
            template<uint32_t buffer_size = 256 , typename string>
            static bool Search(
                string& search_string , const char* hint = "< Search >" ,
                bool* grab_focus = nullptr
            ) {
                PushID();

                draw::ShiftCursorY(1.f);

                bool modified = false;
                bool searching = false;

                const float area_posx = ImGui::GetCursorPosX();
                const float fpaddingy = ImGui::GetStyle().FramePadding.y;

                ScopedStyle frounding(ImGuiStyleVar_FrameRounding , 3.f);
                ScopedStyle fpadding(ImGuiStyleVar_FramePadding , ImVec2(28.f , fpaddingy));
                
                if constexpr(std::is_same<string , std::string>::value) {
                    char search_buffer[buffer_size + 1]{};
                    strncpy(search_buffer , search_string.c_str() , buffer_size);
                    if (ImGui::InputText(NewID() , search_buffer , buffer_size)) {
                        search_string = search_buffer;
                        modified = true;
                    } else if (ImGui::IsItemDeactivatedAfterEdit()) {
                        search_string = search_buffer;
                        modified = true;
                    }

                    searching = search_buffer[0] != 0;
                } else {
                    static_assert(
                        std::is_same<decltype(&search_string[0]) , char*>::value , 
                        "string must be either std::string or char*"
                    );        
                    
                    if (ImGui::InputText(NewID() , search_string , buffer_size)) {
                        modified = true;
                    } else if (ImGui::IsItemDeactivatedAfterEdit()) {
                        modified = true;
                    }

                    searching = search_string[0] != 0;
                }
                
                if (grab_focus != nullptr && *grab_focus) {
                    if (
                        ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
                        !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0)
                    ) {
                        ImGui::SetKeyboardFocusHere(-1);
                    }

                    if (ImGui::IsItemFocused())
                        *grab_focus = false;
                }

                // draws an outline when item is hovered (should probably go somewhere else)
	        auto* drawList = ImGui::GetWindowDrawList();
                ImRect item_rect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()); 
	        item_rect.Min.x += 2.f;
                item_rect.Min.y += 2.f;
                item_rect.Max.x -= 2.f;
                item_rect.Max.y -= 2.f;
	        drawList->AddRect(
                    item_rect.Min , item_rect.Max , 
                    IM_COL32(236 , 158 , 36 , 255) , 
                    GImGui->Style.FrameRounding , 0 , 1.5f
                );

                ImGui::SetItemAllowOverlap();

                ImGui::SameLine(area_posx + 5.f);

                {
                    // Here we can put icons like search icons and clear icons
                }
                
                draw::ShiftCursorY(-1.f);
                PopID();
                return modified;
            }
    };

} // namespace gui

#endif // !YE_EDITOR_WIDGETS_HPP
