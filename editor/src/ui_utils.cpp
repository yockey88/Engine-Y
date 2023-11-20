#include "ui_utils.hpp"

#include <imgui/imgui_internal.h>

namespace gui {
            
    void LoadImGuiStyle() {
        ImGuiStyle& style = ImGui::GetStyle();
        style.Colors[ImGuiCol_TitleBg]            = EditorStyles::title_bar;
        style.Colors[ImGuiCol_TitleBgActive]      = EditorStyles::title_bar_active;
        style.Colors[ImGuiCol_TitleBgCollapsed]   = EditorStyles::title_bar_collapsed;
        style.Colors[ImGuiCol_MenuBarBg]          = EditorStyles::menu_bar;
        style.Colors[ImGuiCol_Tab]                = EditorStyles::tab;
        style.Colors[ImGuiCol_TabHovered]         = EditorStyles::tab_hovered;
        style.Colors[ImGuiCol_TabActive]          = EditorStyles::tab;
        style.Colors[ImGuiCol_TabUnfocused]       = EditorStyles::tab_unfocused;
        style.Colors[ImGuiCol_TabUnfocusedActive] = EditorStyles::tab_unfocused_active;
        style.GrabRounding = 2.3f;
    }
    
    const char* NewID() {
        snprintf(gui_data.id_buffer + 2 , 16 , "%u" , gui_data.id_counter++);
        return gui_data.id_buffer;
    }

    const char* NewLabelID(std::string_view label) {
        *fmt::format_to_n(
            gui_data.label_buffer , std::size(gui_data.label_buffer) , 
            "{}##{}" , label , gui_data.id_counter++
        ).out = 0;
        return gui_data.label_buffer;
    }
    
    void PushID() {
        ImGui::PushID(gui_data.context_id++);
        gui_data.id_counter = 0;
    }

    void PopID() {
        ImGui::PopID();
        gui_data.context_id--;
    }
    
    bool IsInputEnabled() {
    	const auto& io = ImGui::GetIO();
    	return (io.ConfigFlags & ImGuiConfigFlags_NoMouse) == 0 && 
               (io.ConfigFlags & ImGuiConfigFlags_NavNoCaptureKeyboard) == 0;
    }
    
    void SetInputEnabled(bool enabled) {
    	auto& io = ImGui::GetIO();
    
    	if (enabled) {
    	    io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
    	    io.ConfigFlags &= ~ImGuiConfigFlags_NavNoCaptureKeyboard;
    	} else {
    	    io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
    	    io.ConfigFlags |= ImGuiConfigFlags_NavNoCaptureKeyboard;
    	}
    }
   
    // move this to better spot when needed (mayber put right above search widget if needed)
    bool MatchesSearch(
        const std::string& item , std::string_view search_query , 
        bool case_sensitive ,  bool strip_white_spaces , 
        bool strip_underscores
    ) {
    	if (search_query.empty())
    	    return true;
    
    	if (item.empty())
    	    return false;
    
    	std::string item_sanitized = strip_underscores ? choc::text::replace(item , "_" , " ") : item;
    
        if (strip_white_spaces)
    	    item_sanitized = choc::text::replace(item_sanitized , " " , "");
    
    	std::string search_string = strip_white_spaces ? 
            choc::text::replace(search_query, " ", "") : std::string(search_query);
    
        /// todo :: make util for this string stransformation 
    	if (!case_sensitive) {
    	    std::transform(
                item_sanitized.begin() , item_sanitized.end() , 
                item_sanitized.begin() ,			   
                [](const unsigned char c) { 
                    return std::tolower(c); 
                }
            );
    	    std::transform(
                search_string.begin() , search_string.end() , 
                search_string.begin() ,
	        [](const unsigned char c) { 
                    return std::tolower(c); 
                }
            );
    	}
    
    	bool result = false;
    	if (choc::text::contains(search_string, " "))
    	{
    	    std::vector<std::string> search_terms = choc::text::splitAtWhitespace(search_string);
    	    for (const auto& search_term : search_terms)
    	    {
    	    	if (
                        !search_term.empty() && 
                        choc::text::contains(item_sanitized, search_string)
                    ) {
    	    		result = true;
    	    	} else {
    	    		result = false;
    	    		break;
    	    	}
    	    }
    	} else {
    	    result = choc::text::contains(item_sanitized, search_string);
    	}
    
    	return result;
    }
    
    ScopedFont::ScopedFont(ImFont* font) {
        if (font != nullptr) {
            ImGui::PushFont(font);
            pushed = true;
        }
    }

    ScopedFont::~ScopedFont() {
        if (pushed)
            ImGui::PopFont();
    }
    
    ScopedStyle::ScopedStyle(ImGuiStyleVar var , float new_val) {
        const ImGuiDataVarInfo* var_info = ImGui::GetStyleVarInfo(var);
        if (var_info->Type == ImGuiDataType_Float && var_info->Count == 1) {
            ImGui::PushStyleVar(var , new_val);
            pushed = true;
        }
    }

    ScopedStyle::ScopedStyle(ImGuiStyleVar var , const ImVec2& new_col) {
        const ImGuiDataVarInfo* var_info = ImGui::GetStyleVarInfo(var);
        if (var_info->Type == ImGuiDataType_Float && var_info->Count == 2) {
            ImGui::PushStyleVar(var , new_col);
            pushed = true;
        }
    }

    ScopedStyle::~ScopedStyle() {
        if (pushed)
            ImGui::PopStyleVar();
    }
    
    ScopedColor::ScopedColor(ImGuiCol col , uint32_t new_col) {
        ImGui::PushStyleColor(col , new_col);
    }

    ScopedColor::ScopedColor(ImGuiCol col , const ImVec4& new_col) {
        ImGui::PushStyleColor(col , new_col);
    }

    ScopedColor::~ScopedColor() {
        ImGui::PopStyleColor();
    }

    ScopedColorStack::~ScopedColorStack() {    
        ImGui::PopStyleColor(count);
    }

namespace draw {

    void ShiftCursor(float x , float y) {
        const ImVec2 cursor_pos = ImGui::GetCursorPos();
        ImGui::SetCursorPos(ImVec2(cursor_pos.x + x , cursor_pos.y + y));
    }
    
    void ShiftCursorX(float x) {
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + x);
    }

    void ShiftCursorY(float y) {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + y);
    }
    
    void Underline(bool full_width , float offx , float offy) {
        if (full_width) {
            if (ImGui::GetCurrentWindow()->DC.CurrentColumns != nullptr)
                ImGui::PushColumnsBackground();
            else if (ImGui::GetCurrentTable() != nullptr)
                ImGui::TablePushBackgroundChannel();
        }

        const float width = full_width ? ImGui::GetWindowWidth() : ImGui::GetContentRegionAvail().x;
        const ImVec2 cursor = ImGui::GetCursorScreenPos();
        ImGui::GetWindowDrawList()->AddLine(
            ImVec2(cursor.x  + offx , cursor.y + offy),
            ImVec2(cursor.x + width , cursor.y + offy),
            IM_COL32(26, 26, 26, 255) /* dark background */, 1.0f
        );

        if (full_width) {
             if (ImGui::GetCurrentWindow()->DC.CurrentColumns != nullptr)
                 ImGui::PopColumnsBackground();
             else if (ImGui::GetCurrentTable() != nullptr)
                 ImGui::TablePopBackgroundChannel();
	}
    }

    bool BeginTreeNode(const char* name, bool default_open) {
    	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | 
                                        ImGuiTreeNodeFlags_FramePadding;
    	if (default_open)
    	    node_flags |= ImGuiTreeNodeFlags_DefaultOpen;
    
    	return ImGui::TreeNodeEx(name , node_flags);
    }
    
    void EndTreeNode() {
    	ImGui::TreePop();
    }
    
    bool ColoredButton(const char* label, const ImVec4& backgroundColor, ImVec2 buttonSize) {
    	ScopedColor buttonColor(ImGuiCol_Button, backgroundColor);
    	return ImGui::Button(label , buttonSize);
    }
    
    bool ColoredButton(
        const char* label , const ImVec4& background , 
        const ImVec4& foreground , ImVec2 size
    ) {
    	ScopedColor tcolor(ImGuiCol_Text, foreground);
    	ScopedColor bcolor(ImGuiCol_Button, background);
    	return ImGui::Button(label , size);
    }
    
    bool TableRowClickable(const char* id, float rowHeight) {
    	ImGuiWindow* window = ImGui::GetCurrentWindow();
    	window->DC.CurrLineSize.y = rowHeight;
    
    	ImGui::TableNextRow(0, rowHeight);
    	ImGui::TableNextColumn();
    
    	window->DC.CurrLineTextBaseOffset = 3.0f;
    	const ImVec2 rowAreaMin = ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), 0).Min;
    	const ImVec2 rowAreaMax = { 
            ImGui::TableGetCellBgRect(
                ImGui::GetCurrentTable() , 
                ImGui::TableGetColumnCount() - 1
            ).Max.x , 
            rowAreaMin.y + rowHeight 
        };
    
    	ImGui::PushClipRect(rowAreaMin, rowAreaMax, false);
    
    	bool isRowHovered, held;
    	bool isRowClicked = ImGui::ButtonBehavior(
            ImRect(rowAreaMin , rowAreaMax) , ImGui::GetID(id) ,
    	    &isRowHovered , &held , ImGuiButtonFlags_AllowOverlap
        );
    
    	ImGui::SetItemAllowOverlap();
    	ImGui::PopClipRect();
    
    	return isRowClicked;
    }
    
    void Separator(ImVec2 size, ImVec4 color) {
    	ImGui::PushStyleColor(ImGuiCol_ChildBg, color);
    	ImGui::BeginChild("sep", size);
    	ImGui::EndChild();
    	ImGui::PopStyleColor();
    }
    
    bool IsWindowFocused(const char* window_name, const bool check_root_window) {
    	ImGuiWindow* curr_nav_win = GImGui->NavWindow;
    
    	if (check_root_window) {
    	    // Get the actual nav window (not e.g a table)
    	    ImGuiWindow* lastWindow = NULL;
    	    while (lastWindow != curr_nav_win){
                lastWindow = curr_nav_win;
                curr_nav_win = curr_nav_win->RootWindow;
    	    }
    	}
    
    	return curr_nav_win == ImGui::FindWindowByName(window_name);
    }
    
    void HelpMarker(const char* desc) {
    	ImGui::TextDisabled("(?)");
    	if (ImGui::IsItemHovered()) {
    	    ImGui::BeginTooltip();
    	    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
    	    ImGui::TextUnformatted(desc);
    	    ImGui::PopTextWrapPos();
    	    ImGui::EndTooltip();
    	}
    }

    void BeginPropertyGrid(
        uint32_t columns , 
        const ImVec2& item_spacing , 
        const ImVec2& frame_pading
    ) {
    	PushID();
    	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
    	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 4.0f));
    	ImGui::Columns(columns);
    }
    
    void EndPropertyGrid() {
    	ImGui::Columns(1);
    	draw::Underline();
    	ImGui::PopStyleVar(2); // ItemSpacing, FramePadding
        draw::ShiftCursorY(18.0f);
    	PopID();
    }

} // namespace draw

} // namespace gui
