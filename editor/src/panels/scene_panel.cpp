#include "panels/scene_panel.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include "core/log.hpp"
#include "input/keyboard.hpp"
#include "scene/components.hpp"
#include "scene/entity.hpp"
#include "scene/scene.hpp"

#include "widgets.hpp"
#include "ui_utils.hpp"

namespace editor {

    void DrawTransformComponent( EngineY::components::Transform&);
    bool DrawVec3Component(
        const std::string& label , glm::vec3& vec ,
        bool& manual_input , float rest_val 
    );
    
    void ScenePanel::DrawEntity( EngineY::Entity* entity) {
        auto& id  = entity->GetComponent< EngineY::components::ID>();
        auto& transform = entity->GetComponent< EngineY::components::Transform>();
        
        const float edge_offset = 4.f;
        const float row_height = 21.f;

        auto* window = ImGui::GetCurrentWindow();
        window->DC.CurrLineSize.y = row_height;

        ImGui::TableNextRow(0 , row_height);

        // Label column
        ImGui::TableNextColumn();
        window->DC.CurrLineTextBaseOffset = 3.f;

        const ImVec2 row_area_min = ImGui::TableGetCellBgRect(ImGui::GetCurrentTable() , 0).Min;
        const ImVec2 row_area_max = {
            ImGui::TableGetCellBgRect(
                ImGui::GetCurrentTable() , 
                ImGui::GetColumnsCount() - 1
            ).Max.x - 20 ,
            row_area_min.y + row_height
        };

        //  const bool is_selected = current selection
        //
        // (isSelected ? ImGuiTreeNodeFlags_Selected : 0) | 
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
	flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

        const std::string str_id = fmt::format("{0}{1}" , id.name , id.id.uuid);

        ImGui::PushClipRect(row_area_min , row_area_max , false);
        bool is_hovered , held;
        bool is_row_clicked = ImGui::ButtonBehavior(
            ImRect(row_area_min , row_area_max) , ImGui::GetID(str_id.c_str()) ,
            &is_hovered , &held , 
            ImGuiButtonFlags_AllowOverlap | ImGuiButtonFlags_PressedOnClickRelease | 
            ImGuiButtonFlags_MouseButtonLeft  | ImGuiButtonFlags_MouseButtonRight
        );
        bool was_row_right_clicked = ImGui::IsMouseReleased(ImGuiMouseButton_Right);

        ImGui::SetItemAllowOverlap();

        ImGui::PopClipRect();

        const bool is_window_focused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

        // const auto& editor_settings = GetEditorSettings();
        auto fill_row_with_colour = [](const ImColor& colour) {
	    for (int column = 0; column < ImGui::TableGetColumnCount(); column++)
		    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, colour, column);
	};

	// if (isSelected)
	// {
	// 	if (isWindowFocused || UI::NavigatedTo())
	// 		fillRowWithColour(Colors::Theme::selection);
	// 	else
	// 	{
	// 		const ImColor col = UI::ColourWithMultipliedValue(Colors::Theme::selection, 0.9f);
	// 		fillRowWithColour(UI::ColourWithMultipliedSaturation(col, 0.7f));
	// 	}
	// }
	if (is_hovered) {
		fill_row_with_colour(IM_COL32(47, 47, 47, 255));
	}

        // set editor settings and such here

        // Tree Node
        ImGuiContext& g = *GImGui;
	auto& style = ImGui::GetStyle();
	const ImVec2 label_size = ImGui::CalcTextSize(str_id.c_str(), nullptr, false);
	const ImVec2 padding = ((flags & ImGuiTreeNodeFlags_FramePadding)) ? style.FramePadding : ImVec2(style.FramePadding.x, ImMin(window->DC.CurrLineTextBaseOffset, style.FramePadding.y));
	const float text_offset_x = g.FontSize + padding.x * 2;           // Collapser arrow width + Spacing
	const float text_offset_y = ImMax(padding.y, window->DC.CurrLineTextBaseOffset);                    // Latch before ItemSize changes it
	const float text_width = g.FontSize + (label_size.x > 0.0f ? label_size.x + padding.x * 2 : 0.0f);  // Include collapser
	ImVec2 text_pos(window->DC.CursorPos.x + text_offset_x, window->DC.CursorPos.y + text_offset_y);
	const float arrow_hit_x1 = (text_pos.x - text_offset_x) - style.TouchExtraPadding.x;
	const float arrow_hit_x2 = (text_pos.x - text_offset_x) + (g.FontSize + padding.x * 2.0f) + style.TouchExtraPadding.x;
	const bool is_mouse_x_over_arrow = (g.IO.MousePos.x >= arrow_hit_x1 && g.IO.MousePos.x < arrow_hit_x2);

        
        bool previousState = ImGui::TreeNodeBehaviorIsOpen(ImGui::GetID(str_id.c_str()));

	if (is_mouse_x_over_arrow && is_row_clicked)
	    ImGui::SetNextItemOpen(!previousState);
        
        // if selected and has child then ::> ImGui::SetNextItemOpen(true);
        
        // this is a cutom tree node with an icon from Cherno???
        // const bool opened = ImGui::TreeNodeWithIcon(nullptr, ImGui::GetID(str_id.c_str()) , flags , id.name , nullptr);

        if (ImGui::TreeNodeEx(id.name.c_str() , flags)) {
            DrawTransformComponent(transform); 
            ImGui::TreePop();
        }
    }

    void ScenePanel::DrawGui(bool& open) {
        {
            gui::ScopedStyle wpadding(ImGuiStyleVar_WindowPadding , ImVec2(0.f , 0.f));
            if (!ImGui::Begin("Scene" , &open)) {
                LOG_WARN("Failed to draw ScenePanel!");
                ImGui::End();
                return;
            }
        }

        currently_focused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
        ImRect window_rect = {
            ImGui::GetWindowContentRegionMin() ,
            ImGui::GetWindowContentRegionMax()
        }; 

        {
            const float edge_offset = 4.f;
            gui::draw::ShiftCursorX(edge_offset * 3.f);
            gui::draw::ShiftCursorY(edge_offset * 2.f);

            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x -  (edge_offset * 3.f));

            static std::string search_buffer;

            // if (searching) {
            //     ImGui::SetKeyboardFocusHere();
            //     searching = false;
            // }

            gui::Widgets::Search(search_buffer);

            ImGui::Spacing();
            ImGui::Spacing();

            gui::ScopedStyle cpadding(ImGuiStyleVar_CellPadding , ImVec2(4.f , 0.f));

            float h , s , v;
            ImGui::ColorConvertRGBtoHSV(26.f / 255 , 26.f / 255 , 26.f / 255 , h , s , v);
            const ImU32 color_row_alt = ImColor::HSV(h , s , std::min(v * 1.2f , 1.f));
            gui::ScopedColor table_bg_alt(ImGuiCol_TableRowBgAlt , color_row_alt);

            {
                                                             // background dark 
                gui::ScopedStyle tablebg(ImGuiCol_ChildBg , IM_COL32(26 , 26 , 26 , 255));

                ImGuiTableFlags table_flags = ImGuiTableFlags_NoPadInnerX | ImGuiTableFlags_Resizable |
                                              ImGuiTableFlags_Reorderable | ImGuiTableFlags_ScrollY;

                const uint32_t num_columns = 3;

                if (ImGui::BeginTable(
                    "##SceneEntityTable" , num_columns , table_flags , 
                    ImGui::GetContentRegionAvail()
                )) {
                    ImGui::TableSetupColumn("Label");
                    ImGui::TableSetupColumn("Type");
                    ImGui::TableSetupColumn("Visibility");

                    // table headers
                    {
                        float hue , sat , val;     // group_header ????
                        ImGui::ColorConvertRGBtoHSV(26.f / 255 , 26.f / 255 , 26.f / 255 , hue , sat , val);
                        const ImU32 color = ImColor::HSV(hue , sat , std::min(val * 1.2f , 1.f));
                        gui::ScopedColorStack header_colors(
                            ImGuiCol_HeaderHovered , color ,
                            ImGuiCol_HeaderActive , color 
                        );

                        ImGui::TableSetupScrollFreeze(ImGui::TableGetColumnCount() , 1);
                        ImGui::TableNextRow(ImGuiTableRowFlags_Headers , 22.f);
                        for (uint32_t column = 0; column < ImGui::TableGetColumnCount(); ++column) {
                            ImGui::TableSetColumnIndex(column);
                            const char* col_name = ImGui::TableGetColumnName(column);
                            gui::ScopedID col_id(column);

                            gui::draw::ShiftCursor(edge_offset * 3.f , edge_offset * 2.f);
                            ImGui::TableHeader(col_name);
                            gui::draw::ShiftCursor(-edge_offset * 3.f , -edge_offset * 2.f);
                        }
                        ImGui::SetCursorPosX(ImGui::GetCurrentTable()->OuterRect.Min.x);
                        gui::draw::Underline(true , 0.f , 5.f);
                    }

                    // entities
                    if (context != nullptr) {
                        gui::ScopedColorStack ent_select(
                            ImGuiCol_Header , IM_COL32_DISABLE ,
                            ImGuiCol_HeaderHovered , IM_COL32_DISABLE ,
                            ImGuiCol_HeaderActive , IM_COL32_DISABLE 
                        );

                        for (const auto& [id , ent] : *context->Entities()) {
                            DrawEntity(ent); 
                        }
                    }

                    if (ImGui::BeginPopupContextWindow(nullptr , ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
                        // Draw entity creation menu
                        ImGui::EndPopup();
                    }

                    ImGui::EndTable();
                }
            }


            window_rect = ImGui::GetCurrentWindow()->Rect();
        }

        if (ImGui::BeginDragDropTargetCustom(window_rect , ImGui::GetCurrentWindow()->ID)) {
            const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(
                "scene_entity_hierarchy" , ImGuiDragDropFlags_AcceptNoDrawDefaultRect
            );

            if (payload != nullptr) {
                size_t count = payload->DataSize / sizeof( EngineY::UUID);

                for (size_t i = 0; i < count; ++i) {
                     EngineY::UUID id = *((( EngineY::UUID*)payload->Data) + i);
                     EngineY::Entity* ent = context->GetEntity(id);
                    if (ent != nullptr) {
                        // move entity
                    } else {
                        // load entity from scene file?
                    }
                }
            }

            ImGui::EndDragDropTarget();
        }

        {
            gui::ScopedStyle wpadding(ImGuiStyleVar_WindowPadding , ImVec2(2.f , 4.f));
            ImGui::Begin("Properties");
            hierarchy_or_props_focused = currently_focused || ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
            // DrawComponents(active_selection);
            ImGui::End();
        }

        ImGui::End();
    }

    void DrawTransformComponent( EngineY::components::Transform& transform) {
        gui::ScopedStyle ispacing(ImGuiStyleVar_ItemSpacing , ImVec2{ 8.f , 8.f });
        gui::ScopedStyle fpadding(ImGuiStyleVar_FramePadding , ImVec2{ 4.f , 4.f });
    
        ImGui::BeginTable("transform-component" , 2 , ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_NoClip);
    	ImGui::TableSetupColumn("label_column" , 0 , 100.0f);
    	ImGui::TableSetupColumn(
            "value_column" , ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoClip , 
            ImGui::GetContentRegionAvail().x - 100.0f
        );
    
        bool translate_manual_input = false;
        bool rotate_manual_input = false;
        bool scale_manual_input = false;
    
        ImGui::TableNextRow();
        DrawVec3Component("Position" , transform.position , translate_manual_input , 0.f);
    
        ImGui::TableNextRow();
        DrawVec3Component("Rotation" , transform.rotation , rotate_manual_input , 0.f);
    
        ImGui::TableNextRow();
        DrawVec3Component("Scale" , transform.scale , scale_manual_input , 1.f);
    
        ImGui::EndTable();
    
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8.f);
    
        gui::draw::Underline();
        
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 18.f);
    }
    
    static uint32_t id = 0;
    
    bool DrawVec3Component(
        const std::string& label , glm::vec3& vec , bool& manual_input , 
        float reset_val
    ) {
        bool modified = false;
    
        ImGui::PushID(id++);
        ImGui::TableSetColumnIndex(0);
        gui::draw::ShiftCursor(17.f , 7.f);
    
        ImGui::Text(label.c_str());
    
        gui::draw::Underline(false , 0.f , 2.f);
    
        ImGui::TableSetColumnIndex(1);
        gui::draw::ShiftCursor(7.f , 0.f);
        
        {
            const float spacing_x = 8.f;
            gui::ScopedStyle ispacing(ImGuiStyleVar_ItemSpacing , ImVec2{ spacing_x , 0.f }); 
            gui::ScopedStyle fpadding(ImGuiStyleVar_FramePadding , ImVec2{ 4.f , 4.f });
            
            {
                gui::ScopedColor bcolor(ImGuiCol_Border , EditorStyles::clear_color);
                gui::ScopedColor fcolor(ImGuiCol_FrameBg , EditorStyles::clear_color);
    
                ImGui::BeginChild(
                    ImGui::GetID((label + "fr").c_str()) , 
                    ImVec2(
                        ImGui::GetContentRegionAvail().x - spacing_x , 
                        ImGui::GetFrameHeightWithSpacing() + 8.f
                    ) , 
                    false , 
                    ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse
                );
            }
    
            const float frame_padding = 2.f;
            const float outline_spacing = 1.f;
            const float line_height = GImGui->Font->FontSize + frame_padding * 2.f; 
    
            const ImVec2 button_size = { line_height + frame_padding , line_height };
            const float input_item_width = (ImGui::GetContentRegionAvail().x -spacing_x) / 3.f - button_size.x;
    
            gui::draw::ShiftCursor(0.f , frame_padding);
    
            auto item_drawer = [&](
                const std::string& label , float& value , 
                const ImVec4& cN , const ImVec4& cH , const ImVec4& cP 
            ) {
                {
                    gui::ScopedStyle fpadding(ImGuiStyleVar_FramePadding , ImVec2{ frame_padding , 0.f });
                    gui::ScopedStyle frounding(ImGuiStyleVar_FrameRounding , 1.f);
    
                    gui::ScopedColorStack color_stack(
                        ImGuiCol_Button , cN , 
                        ImGuiCol_ButtonHovered , cH , 
                        ImGuiCol_ButtonActive , cP
                    );
    
                    ImGuiIO& io = ImGui::GetIO();
                    gui::ScopedFont font(io.Fonts->Fonts[0]);
    
                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.f);
                    if (ImGui::Button(label.c_str() , button_size)) {
                        value = reset_val;
                        modified = true;
                    }
                }
            
                ImGui::SameLine(0.f , outline_spacing);
                ImGui::SetNextItemWidth(input_item_width);
    
                gui::draw::ShiftCursor(0.f , frame_padding);
    
                ImGui::PushItemFlag(ImGuiItemFlags_MixedValue , true);
                bool was_temp_input = ImGui::TempInputIsActive(ImGui::GetID(("##" + label).c_str()));
    
                modified |= ImGui::DragFloat(("##" + label).c_str() , &value , 0.1f , 0.f , 0.f , "%.2f" , 0);
    
                if (modified &&  EngineY::Keyboard::KeyDown( EngineY::Keyboard::Key::YE_TAB))
                    manual_input = true;
                
                if (ImGui::TempInputIsActive(ImGui::GetID(("##" + label).c_str())))
                    manual_input = false;
                
                ImGui::PopItemFlag();
    
                if (was_temp_input)
                    manual_input |= ImGui::IsItemDeactivatedAfterEdit();
            };
    
            item_drawer("X" , vec.x , ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f } , ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f } , ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
            ImGui::SameLine(0.f , outline_spacing);
    
            item_drawer("Y" , vec.y , ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f } , ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f } , ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
            ImGui::SameLine(0.f , outline_spacing);
    
            item_drawer("Z" , vec.z , ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f } , ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f } , ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
    
            ImGui::EndChild();
        }
    
        ImGui::PopID();
        --id;
    
        return modified || manual_input;
    }

} // namespace editor
