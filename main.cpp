#include "web_client/Core/Core.hpp"
#include <iostream>
#include <windows.h>
#include <dwmapi.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <vector>

#include "libs/imgui/imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "libs/imgui/imgui_internal.h"
#include "libs/imgui/imgui_impl_dx9.h"
#include "libs/imgui/imgui_impl_win32.h"

#include "classes/vector.hpp"
#include "hacks/hack.hpp"
#include "render/render.hpp"
#include "draggable_esp/items.h"
#include "logs/logs.h"
#include "menu.h"
#include "bytes/cheat_logo.h"
#include "bytes/model.h"
#include "bytes/icons_font_awesome5.hpp"
#include "Utils/ProcessManager.hpp"
#include "binds/binds.hpp"
#include "game.h"
#include "classes/Entity.h"
#include "configs/cheat_cfg.hpp"
#include "offsets/Offsets.h"
#include "libs/json.hpp"
#include "libs/Curl/curl.h"

#pragma comment( lib, "LibCurl.lib" )
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")

//#define TEST_WINDOW
std::string new_cfg_name = "";
inline IDirect3DTexture9* logo = nullptr;
inline IDirect3DTexture9* model_preview = nullptr;
inline ImColor current_color;

struct {
	ImVec2 WindowPadding;
	float  WindowRounding;
	ImVec2 WindowMinSize;
	float  ChildRounding;
	float  PopupRounding;
	ImVec2 FramePadding;
	float  FrameRounding;
	ImVec2 ItemSpacing;
	ImVec2 ItemInnerSpacing;
	ImVec2 TouchExtraPadding;
	float  IndentSpacing;
	float  ColumnsMinSpacing;
	float  ScrollbarSize;
	float  ScrollbarRounding;
	float  GrabMinSize;
	float  GrabRounding;
	float  TabRounding;
	float  TabMinWidthForUnselectedCloseButton;
	ImVec2 DisplayWindowPadding;
	ImVec2 DisplaySafeAreaPadding;
	float  MouseCursorScale;
} styles;

struct board_key
{
	std::string name;
	DWORD key;
};

std::vector<board_key> inputtable_keys =
{
	{"1", 0x31},
	{"2", 0x32},
	{"3", 0x33},
	{"4", 0x34},
	{"5", 0x35},
	{"6", 0x36},
	{"7", 0x37},
	{"8", 0x38},
	{"9", 0x39},
	{"0", 0x30},
	{"q", 0x51},
	{"w", 0x57},
	{"e", 0x45},
	{"r", 0x52},
	{"t", 0x54},
	{"y", 0x59},
	{"u", 0x55},
	{"i", 0x49},
	{"o", 0x4F},
	{"p", 0x50},
	{"a", 0x41},
	{"s", 0x53},
	{"d", 0x44},
	{"f", 0x46},
	{"g", 0x47},
	{"h", 0x48},
	{"j", 0x4A},
	{"k", 0x4B},
	{"l", 0x4C},
	{"z", 0x5A},
	{"x", 0x58},
	{"c", 0x43},
	{"v", 0x56},
	{"b", 0x42},
	{"n", 0x4E},
	{"m", 0x4D},
};


#include "libs/imgui/flag.h"

inline std::string cached_cfg_name = "";
inline std::string config_name = "";

namespace CutomGUIConfig
{
	inline int bucket_pos = 0;

	inline std::string get_last_char()
	{
		if (!menuShow)
			return "";

		std::string result = "";
		for (auto item : inputtable_keys)
			if (GetAsyncKeyState(item.key) & 1)
			{
				if (::GetAsyncKeyState(VK_LSHIFT) || ::GetAsyncKeyState(VK_RSHIFT))
				{
					std::string buf_str = item.name;
					std::transform(buf_str.begin(), buf_str.end(), buf_str.begin(), ::toupper);
					result = buf_str;
					break;
				}
				else
				{
					result = item.name;
					break;
				}	
			}
		return result;
	}

	inline void add_key() //add key after bucket
	{
		if (!menuShow)
			return;

		std::string last_char = get_last_char();
		if (last_char != "" && config_name.length() < 32)
		{
			if (bucket_pos < config_name.size() - 1)
				config_name.insert(bucket_pos, last_char);
			else
				config_name.append(last_char);

			bucket_pos++;
		}
	}

	inline void remove_key_backspace() 
	{
		if (bucket_pos > 0)
		{
			config_name.erase(bucket_pos-1, 1);
			bucket_pos--;
		}
	}

	inline void remove_key_del()
	{
		if (bucket_pos < config_name.length())
			config_name.erase(bucket_pos, 1);
	}

	inline void manage_bucket()
	{
		if (GetAsyncKeyState(VK_LEFT) & 1)
		{
			if (bucket_pos > 0)
				bucket_pos-=1;
		}
		else if (GetAsyncKeyState(VK_RIGHT) & 1)
		{
			if (bucket_pos < config_name.length())
				bucket_pos+=1;
		}
	}

	inline void control_string()
	{
		manage_bucket();

		if (GetAsyncKeyState(VK_DELETE) & 1)
			remove_key_del();
		else if (GetAsyncKeyState(VK_BACK) & 1)
			remove_key_backspace();
		else
			add_key();
		

		cart_pos = bucket_pos;
		new_cfg_name = config_name;

		char* tab2 = new char[config_name.length() + 1];
		strcpy(tab2, config_name.c_str());
		ImGui::InputText(("##configname"), tab2, sizeof(tab2));
	}
}

std::vector<std::vector<board_key>> board_keys =
{
	std::vector<board_key>{
		{"Esc", VK_ESCAPE},
		{"F1", VK_F1},
		{"F2", VK_F2},
		{"F3", VK_F3},
		{"F4", VK_F4},
		{"F5", VK_F5},
		{"F6", VK_F6},
		{"F7", VK_F7},
		{"F8", VK_F8},
		{"F9", VK_F9},
		{"F10", VK_F10},
		{"F11", VK_F11},
		{"F12", VK_F12},
		{"Print", VK_PRINT},
		{"Scroll", VK_SCROLL},
		{"Pause", VK_PAUSE},
	},

	std::vector<board_key>{
		{"~", VK_OEM_3},
		{"1", 0x31},
		{"2", 0x32},
		{"3", 0x33},
		{"4", 0x34},
		{"5", 0x35},
		{"6", 0x36},
		{"7", 0x37},
		{"8", 0x38},
		{"9", 0x39},
		{"0", 0x30},
		{"<-", VK_BACK},
		{"Insert", VK_INSERT},
		{"Home", VK_HOME},
		{"Num Lc", VK_NUMLOCK},
	},

	std::vector<board_key>{
		{"TAB", VK_TAB},
		{"Q", 0x51},
		{"W", 0x57},
		{"E", 0x45},
		{"R", 0x52},
		{"T", 0x54},
		{"Y", 0x59},
		{"U", 0x55},
		{"I", 0x49},
		{"O", 0x4F},
		{"P", 0x50},
		{"Enter", VK_RETURN},
		{"Delete", VK_DELETE},
		{"End", VK_END},
	},

	std::vector<board_key>{
		{"CAPS", VK_TAB},
		{"A", 0x41},
		{"S", 0x53},
		{"D", 0x44},
		{"F", 0x46},
		{"G", 0x47},
		{"H", 0x48},
		{"J", 0x4A},
		{"K", 0x4B},
		{"L", 0x4C},
	},

	std::vector<board_key>{
		{"L Shift", VK_LSHIFT},
		{"Z", 0x5A},
		{"X", 0x58},
		{"C", 0x43},
		{"V", 0x56},
		{"B", 0x42},
		{"N", 0x4E},
		{"M", 0x4D},
		{"R Shift", VK_RSHIFT},
		{"Up Arrow", VK_UP},
	},

	std::vector<board_key>{
		{"L Ctrl", VK_LCONTROL},
		{"L Win", VK_LWIN},
		{"L Alt", VK_LMENU},
		{"Space", VK_SPACE},
		{"R Alt", VK_RMENU},
		{"R Win", VK_RWIN},
		{"R Ctrl", VK_RCONTROL},
		{"L Arrow", VK_UP},
		{"D Arrow", VK_DOWN},
		{"R Arrow", VK_RIGHT},
	},

};

DWORD last_board_key;
bool use_focus = false;
bool use_bind = false;
std::string board_caller_name = "";
ImGuiWindow* main_window;

void menu_setup(ImGuiStyle& style) //-V688
{
	ImGui::StyleColorsDark(); // colors setup
	ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Once); // window pos setup
	ImGui::SetNextWindowBgAlpha(min(style.Alpha, 0.94f)); // window bg alpha setup

	styles.WindowPadding = style.WindowPadding;
	styles.WindowRounding = style.WindowRounding;
	styles.WindowMinSize = style.WindowMinSize;
	styles.ChildRounding = style.ChildRounding;
	styles.PopupRounding = style.PopupRounding;
	styles.FramePadding = style.FramePadding;
	styles.FrameRounding = style.FrameRounding;
	styles.ItemSpacing = style.ItemSpacing;
	styles.ItemInnerSpacing = style.ItemInnerSpacing;
	styles.TouchExtraPadding = style.TouchExtraPadding;
	styles.IndentSpacing = style.IndentSpacing;
	styles.ColumnsMinSpacing = style.ColumnsMinSpacing;
	styles.ScrollbarSize = style.ScrollbarSize;
	styles.ScrollbarRounding = style.ScrollbarRounding;
	styles.GrabMinSize = style.GrabMinSize;
	styles.GrabRounding = style.GrabRounding;
	styles.TabRounding = style.TabRounding;
	styles.DisplayWindowPadding = style.DisplayWindowPadding;
	styles.DisplaySafeAreaPadding = style.DisplaySafeAreaPadding;
	styles.MouseCursorScale = style.MouseCursorScale;

	dpi_scale = max(1, ImGui::GetIO().DisplaySize.y / 1080);
	update_dpi = true;
	menu_setupped = true; // we dont want to setup menu again
}

void dpi_resize(float scale_factor, ImGuiStyle& style) //-V688
{
	style.WindowPadding = (styles.WindowPadding * scale_factor);
	style.WindowRounding = (styles.WindowRounding * scale_factor);
	style.WindowMinSize = (styles.WindowMinSize * scale_factor);
	style.ChildRounding = (styles.ChildRounding * scale_factor);
	style.PopupRounding = (styles.PopupRounding * scale_factor);
	style.FramePadding = (styles.FramePadding * scale_factor);
	style.FrameRounding = (styles.FrameRounding * scale_factor);
	style.ItemSpacing = (styles.ItemSpacing * scale_factor);
	style.ItemInnerSpacing = (styles.ItemInnerSpacing * scale_factor);
	style.TouchExtraPadding = (styles.TouchExtraPadding * scale_factor);
	style.IndentSpacing = (styles.IndentSpacing * scale_factor);
	style.ColumnsMinSpacing = (styles.ColumnsMinSpacing * scale_factor);
	style.ScrollbarSize = (styles.ScrollbarSize * scale_factor);
	style.ScrollbarRounding = (styles.ScrollbarRounding * scale_factor);
	style.GrabMinSize = (styles.GrabMinSize * scale_factor);
	style.GrabRounding = (styles.GrabRounding * scale_factor);
	style.TabRounding = (styles.TabRounding * scale_factor);
	style.DisplayWindowPadding = (styles.DisplayWindowPadding * scale_factor);
	style.DisplaySafeAreaPadding = (styles.DisplaySafeAreaPadding * scale_factor);
	style.MouseCursorScale = (styles.MouseCursorScale * scale_factor);
}

HWND _HWND = NULL;

std::vector<std::string> weapon_names =
{
	_S("Global"),
	_S("M249"),
	_S("MAG-7"),
	_S("Negev"),
	_S("Nova"),
	_S("Sawed-Off"),
	_S("XM1014"),
	_S("CZ75-Auto"),
	_S("Desert Eagle"),
	_S("Dual Berettas"),
	_S("Five-SeveN"),
	_S("Glock-18"),
	_S("P2000"),
	_S("P250"),
	_S("R8 Revolver"),
	_S("Tec-9"),
	_S("USP-S"),
	_S("AK-47"),
	_S("AUG"),
	_S("AWP"),
	_S("FAMAS"),
	_S("G3SG1"),
	_S("Galil AR"),
	_S("M4A4"),
	_S("M4A1-S"),
	_S("SCAR-20"),
	_S("SG 553"),
	_S("SSG 08"),
	_S("PP-Bizon"),
	_S("MAC-10"),
	_S("MP5-SD"),
	_S("MP7"),
	_S("MP9"),
	_S("P90"),
	_S("UMP-45"),
};


int selected_config = -1;

inline std::string GetWeaponIcon(std::string weapon)
{
	if (weapon == _S("m249")) return "l";
	if (weapon == _S("mag7")) return "K";
	if (weapon == _S("negev")) return "L";
	if (weapon == _S("nova")) return "S";
	if (weapon == _S("sawedoff")) return "M";
	if (weapon == _S("xm1014")) return "I";
	if (weapon == _S("cz75a")) return "o";
	if (weapon == _S("deagle")) return "1";
	if (weapon == _S("elite")) return "2";
	if (weapon == _S("fiveseven")) return "3";
	if (weapon == _S("glock")) return "4";
	if (weapon == _S("hkp2000")) return "C";
	if (weapon == _S("p250")) return "P";
	if (weapon == _S("revolver")) return "";
	if (weapon == _S("tec9")) return "N";
	if (weapon == _S("usp_silencer")) return "m";
	if (weapon == _S("ak47")) return "7";
	if (weapon == _S("aug")) return "8";
	if (weapon == _S("awp")) return "9";
	if (weapon == _S("famas")) return ":";
	if (weapon == _S("g3sg1")) return ";";
	if (weapon == _S("galilar")) return "=";
	if (weapon == _S("m4a1")) return ">";
	if (weapon == _S("m4a1_silencer")) return "@";
	if (weapon == _S("scar20")) return "V";
	if (weapon == _S("sg556")) return "W";
	if (weapon == _S("ssg08")) return "X";
	if (weapon == _S("bizon")) return "J";
	if (weapon == _S("mac10")) return "A";
	if (weapon == _S("mp5sd")) return "";
	if (weapon == _S("mp7")) return "Q";
	if (weapon == _S("mp9")) return "R";
	if (weapon == _S("p90")) return "T";
	if (weapon == _S("ump45")) return "H";

	if (weapon == _S("M249")) return "l";
	if (weapon == _S("MAG-7")) return "K";
	if (weapon == _S("Negev")) return "L";
	if (weapon == _S("Nova")) return "S";
	if (weapon == _S("Sawed-Off")) return "M";
	if (weapon == _S("XM1014")) return "I";
	if (weapon == _S("CZ75-Auto")) return "o";
	if (weapon == _S("Desert Eagle")) return "1";
	if (weapon == _S("Dual Berettas")) return "2";
	if (weapon == _S("Five-SeveN")) return "3";
	if (weapon == _S("Glock-18")) return "4";
	if (weapon == _S("P2000")) return "C";
	if (weapon == _S("P250")) return "P";
	if (weapon == _S("R8 Revolver")) return "";
	if (weapon == _S("Tec-9")) return "N";
	if (weapon == _S("USP-S")) return "m";
	if (weapon == _S("AK-47")) return "7";
	if (weapon == _S("AUG")) return "8";
	if (weapon == _S("AWP")) return "9";
	if (weapon == _S("FAMAS")) return ":";
	if (weapon == _S("G3SG1")) return ";";
	if (weapon == _S("Galil AR")) return "=";
	if (weapon == _S("M4A4")) return ">";
	if (weapon == _S("M4A1-S")) return "@";
	if (weapon == _S("SCAR-20")) return "V";
	if (weapon == _S("SG 553")) return "W";
	if (weapon == _S("SSG 08")) return "X";
	if (weapon == _S("PP-Bizon")) return "J";
	if (weapon == _S("MAC-10")) return "A";
	if (weapon == _S("MP5-SD")) return "";
	if (weapon == _S("MP7")) return "Q";
	if (weapon == _S("MP9")) return "R";
	if (weapon == _S("P90")) return "T";
	if (weapon == _S("UMP-45")) return "H";
	return "H";
}

namespace ImGuiEx
{
	inline bool Selectable(const char* label, bool selected, ImGuiSelectableFlags flags = 0, ImVec2 size_arg = ImVec2(0, 0))
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;

		// Submit label or explicit size to ItemSize(), whereas ItemAdd() will submit a larger/spanning rectangle.
		ImGuiID id = window->GetID(label);
		ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
		ImVec2 size(size_arg.x != 0.0f ? size_arg.x : label_size.x, size_arg.y != 0.0f ? size_arg.y : label_size.y);
		ImVec2 pos = window->DC.CursorPos;
		pos.y += window->DC.CurrLineTextBaseOffset;
		ImGui::ItemSize(size, 0.0f);

		// Fill horizontal space
		// We don't support (size < 0.0f) in Selectable() because the ItemSpacing extension would make explicitly right-aligned sizes not visibly match other widgets.
		const bool span_all_columns = (flags & ImGuiSelectableFlags_SpanAllColumns) != 0;
		const float min_x = span_all_columns ? window->ParentWorkRect.Min.x : pos.x;
		const float max_x = span_all_columns ? window->ParentWorkRect.Max.x : window->WorkRect.Max.x;
		if (size_arg.x == 0.0f || (flags & ImGuiSelectableFlags_SpanAvailWidth))
			size.x = ImMax(label_size.x, max_x - min_x);

		// Text stays at the submission position, but bounding box may be extended on both sides
		const ImVec2 text_min = pos;
		const ImVec2 text_max(min_x + size.x, pos.y + size.y);

		// Selectables are meant to be tightly packed together with no click-gap, so we extend their box to cover spacing between selectable.
		ImRect bb(min_x, pos.y, text_max.x, text_max.y);
		if ((flags & ImGuiSelectableFlags_NoPadWithHalfSpacing) == 0)
		{
			const float spacing_x = span_all_columns ? 0.0f : style.ItemSpacing.x;
			const float spacing_y = style.ItemSpacing.y;
			const float spacing_L = IM_FLOOR(spacing_x * 0.50f);
			const float spacing_U = IM_FLOOR(spacing_y * 0.50f);
			bb.Min.x -= spacing_L;
			bb.Min.y -= spacing_U;
			bb.Max.x += (spacing_x - spacing_L);
			bb.Max.y += (spacing_y - spacing_U);
		}
		//if (g.IO.KeyCtrl) { GetForegroundDrawList()->AddRect(bb.Min, bb.Max, IM_COL32(0, 255, 0, 255)); }

		// Modify ClipRect for the ItemAdd(), faster than doing a PushColumnsBackground/PushTableBackground for every Selectable..
		const float backup_clip_rect_min_x = window->ClipRect.Min.x;
		const float backup_clip_rect_max_x = window->ClipRect.Max.x;
		if (span_all_columns)
		{
			window->ClipRect.Min.x = window->ParentWorkRect.Min.x;
			window->ClipRect.Max.x = window->ParentWorkRect.Max.x;
		}

		const bool disabled_item = (flags & ImGuiSelectableFlags_Disabled) != 0;
		const bool item_add = ImGui::ItemAdd(bb, id, NULL, disabled_item ? ImGuiItemFlags_Disabled : ImGuiItemFlags_None);
		if (span_all_columns)
		{
			window->ClipRect.Min.x = backup_clip_rect_min_x;
			window->ClipRect.Max.x = backup_clip_rect_max_x;
		}

		if (!item_add)
			return false;

		const bool disabled_global = (g.CurrentItemFlags & ImGuiItemFlags_Disabled) != 0;
		if (disabled_item && !disabled_global) // Only testing this as an optimization
			ImGui::BeginDisabled();

		// FIXME: We can standardize the behavior of those two, we could also keep the fast path of override ClipRect + full push on render only,
		// which would be advantageous since most selectable are not selected.
		if (span_all_columns && window->DC.CurrentColumns)
			ImGui::PushColumnsBackground();
		else if (span_all_columns && g.CurrentTable)
			ImGui::TablePushBackgroundChannel();

		// We use NoHoldingActiveID on menus so user can click and _hold_ on a menu then drag to browse child entries
		ImGuiButtonFlags button_flags = 0;
		if (flags & ImGuiSelectableFlags_NoHoldingActiveID) { button_flags |= ImGuiButtonFlags_NoHoldingActiveId; }
		if (flags & ImGuiSelectableFlags_SelectOnClick) { button_flags |= ImGuiButtonFlags_PressedOnClick; }
		if (flags & ImGuiSelectableFlags_SelectOnRelease) { button_flags |= ImGuiButtonFlags_PressedOnRelease; }
		if (flags & ImGuiSelectableFlags_AllowDoubleClick) { button_flags |= ImGuiButtonFlags_PressedOnClickRelease | ImGuiButtonFlags_PressedOnDoubleClick; }
		if (flags & ImGuiSelectableFlags_AllowItemOverlap) { button_flags |= ImGuiButtonFlags_AllowItemOverlap; }

		const bool was_selected = selected;
		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, button_flags);

		// Auto-select when moved into
		// - This will be more fully fleshed in the range-select branch
		// - This is not exposed as it won't nicely work with some user side handling of shift/control
		// - We cannot do 'if (g.NavJustMovedToId != id) { selected = false; pressed = was_selected; }' for two reasons
		//   - (1) it would require focus scope to be set, need exposing PushFocusScope() or equivalent (e.g. BeginSelection() calling PushFocusScope())
		//   - (2) usage will fail with clipped items
		//   The multi-select API aim to fix those issues, e.g. may be replaced with a BeginSelection() API.
		if ((flags & ImGuiSelectableFlags_SelectOnNav) && g.NavJustMovedToId != 0 && g.NavJustMovedToFocusScopeId == window->DC.NavFocusScopeIdCurrent)
			if (g.NavJustMovedToId == id)
				selected = pressed = true;

		// Update NavId when clicking or when Hovering (this doesn't happen on most widgets), so navigation can be resumed with gamepad/keyboard
		if (pressed || (hovered && (flags & ImGuiSelectableFlags_SetNavIdOnHover)))
		{
			if (!g.NavDisableMouseHover && g.NavWindow == window && g.NavLayer == window->DC.NavLayerCurrent)
			{
				ImGui::SetNavID(id, window->DC.NavLayerCurrent, window->DC.NavFocusScopeIdCurrent, ImGui::WindowRectAbsToRel(window, bb)); // (bb == NavRect)
				g.NavDisableHighlight = true;
			}
		}
		if (pressed)
			ImGui::MarkItemEdited(id);

		if (flags & ImGuiSelectableFlags_AllowItemOverlap)
			ImGui::SetItemAllowOverlap();

		// In this branch, Selectable() cannot toggle the selection so this will never trigger.
		if (selected != was_selected) //-V547
			g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_ToggledSelection;

		// Render
		if (held && (flags & ImGuiSelectableFlags_DrawHoveredWhenHeld))
			hovered = true;
		if (hovered || selected)
		{
			const ImU32 col = ImGui::GetColorU32((held && hovered) ? ImGuiCol_HeaderActive : hovered ? ImGuiCol_HeaderHovered : ImGuiCol_Header);
			ImGui::RenderFrame(bb.Min, bb.Max, col, false, 0.0f);
		}
		ImGui::RenderNavHighlight(bb, id, ImGuiNavHighlightFlags_TypeThin | ImGuiNavHighlightFlags_NoRounding);

		if (span_all_columns && window->DC.CurrentColumns)
			ImGui::PopColumnsBackground();
		else if (span_all_columns && g.CurrentTable)
			ImGui::TablePopBackgroundChannel();

		if (label != "Global")
		{
			std::string icon = GetWeaponIcon(label);
			auto iconsize = ImGui::CalcTextSize(icon.c_str());
			ImVec2 spacing = ImVec2(10.f * dpi_scale, 0);
			ImGui::PushFont(icons_font);
			ImGui::RenderTextClipped(spacing + text_min, text_max, icon.c_str(), NULL, &label_size, style.SelectableTextAlign, &bb);
			ImGui::PopFont();
			ImGui::RenderTextClipped(spacing + spacing + iconsize + text_min, text_max, label, NULL, &label_size, style.SelectableTextAlign, &bb);
		}
		else
		{
			auto iconsize = ImGui::CalcTextSize(" ");
			ImVec2 spacing = ImVec2(10.f * dpi_scale, 0);
			ImGui::PushFont(icons_font);
			ImGui::RenderTextClipped(spacing + text_min, text_max, " ", NULL, &label_size, style.SelectableTextAlign, &bb);
			ImGui::PopFont();
			ImGui::RenderTextClipped(spacing + spacing + iconsize + text_min, text_max, label, NULL, &label_size, style.SelectableTextAlign, &bb);
		}
		// Automatically close popups
		if (pressed && (window->Flags & ImGuiWindowFlags_Popup) && !(flags & ImGuiSelectableFlags_DontClosePopups) && !(g.LastItemData.InFlags & ImGuiItemFlags_SelectableDontClosePopup))
			ImGui::CloseCurrentPopup();

		if (disabled_item && !disabled_global)
			ImGui::EndDisabled();

		IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
		return pressed; //-V1020
	}

}

namespace OverlayWindow
{
	WNDCLASSEX WindowClass;
	HWND Hwnd;
	LPCWSTR Name;
}

namespace DirectX9Interface
{
	IDirect3D9Ex* Direct3D9 = NULL;
	IDirect3DDevice9Ex* pDevice = NULL;
	D3DPRESENT_PARAMETERS pParams = { NULL };
	MARGINS Margin = { -1 };
	MSG Message = { NULL };
}

void InputHandler() {
	for (int i = 0; i < 5; i++) 
		ImGui::GetIO().MouseDown[i] = false;

	int button = -1;
	
	if (GetAsyncKeyState(VK_LBUTTON)) 
		button = 0;

	if (GetAsyncKeyState(VK_RBUTTON))
		button = 1;

	if (button != -1) 
		ImGui::GetIO().MouseDown[button] = true;

}

void load_config()
{
	if (cfg_manager->files.empty())
		return;

	cfg_manager->load(cfg_manager->files.at(selected_config), false);
	/*c_lua::get().unload_all_scripts();

	for (auto& script : g_cfg.scripts.scripts)
		c_lua::get().load_script(c_lua::get().get_script_id(script));

	scripts = c_lua::get().scripts;

	if (selected_script >= scripts.size())
		selected_script = scripts.size() - 1;

	for (auto& current : scripts)
	{
		if (current.size() >= 5 && current.at(current.size() - 1) == 'c')
			current.erase(current.size() - 5, 5);
		else if (current.size() >= 4)
			current.erase(current.size() - 4, 4);
	}

	g_cfg.scripts.scripts.clear();
	*/
	//cfg_manager->load(cfg_manager->files.at(selected_config), true);
	cfg_manager->config_files();

	logs_system::push_log("Loaded " + cfg_manager->files.at(selected_config) + " config", false);
}

void save_config()
{
	if (cfg_manager->files.empty())
		return;

	/*
	g_cfg.scripts.scripts.clear();

	for (auto i = 0; i < c_lua::get().scripts.size(); ++i)
	{
		auto script = c_lua::get().scripts.at(i);

		if (c_lua::get().loaded.at(i))
			g_cfg.scripts.scripts.emplace_back(script);
	}
	*/

	cfg_manager->save(cfg_manager->files.at(selected_config));
	cfg_manager->config_files();

	logs_system::push_log("Saved " + cfg_manager->files.at(selected_config) + " config", false);
}

void remove_config()
{
	if (cfg_manager->files.empty())
		return;

	logs_system::push_log("Removed " + cfg_manager->files.at(selected_config) + " config", false);

	cfg_manager->remove(cfg_manager->files.at(selected_config));
	cfg_manager->config_files();

	if (selected_config >= cfg_manager->files.size())
		selected_config = cfg_manager->files.size() - 1;
}

void add_config()
{
	auto empty = true;

	for (auto current : new_cfg_name)
	{
		if (current != ' ')
		{
			empty = false;
			break;
		}
	}

	if (empty)
		new_cfg_name = "genesis_external_config";

	logs_system::push_log("Added " + new_cfg_name + " config", false);

	if (new_cfg_name.find(".gn") == std::string::npos)
		new_cfg_name += ".gn";

	cfg_manager->save(new_cfg_name);
	cfg_manager->config_files();

	//selected_config = cfg_manager->files.size() - 1;
}

void RenderScreenBoard()
{
	float button_width = 0;
	float button_height = 0;
	int spacing = 3;
	int spacings_val = 0;
	int buttons_in_row = 0;

	int board_width = ScreenWidth;
	int board_height = ScreenHeight / 3;
	ImGui::SetNextWindowPos(ImVec2(0, (ScreenHeight / 3) * 2));
	ImGui::SetNextWindowSize(ImVec2(board_width, board_height));
	ImGui::SetNextWindowBgAlpha(0.7f);
	ImGui::Begin("ButtonBoard", nullptr, ImGuiWindowFlags_NoDecoration);
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(spacing, spacing));
		for (int i = 0; i < board_keys.size(); i++) //enum rows (6)
		{
			buttons_in_row = board_keys.at(i).size() - 1;
			spacings_val = board_keys.at(i).size();
			button_width = (board_width - spacings_val * spacing) / (buttons_in_row + 1);
			button_height = (board_height - board_keys.size() * spacing) / (board_keys.size());
			for (int j = 0; j <= buttons_in_row; j++)
			{
				if (ImGui::Button(board_keys.at(i).at(j).name.c_str(), ImVec2(button_width, button_height)))
					last_board_key = board_keys.at(i).at(j).key;
				if (j < buttons_in_row)
					ImGui::SameLine();
			}
		}
		ImGui::PopStyleVar();
	}
	ImGui::End();
}

inline void s()
{
	ImGui::Spacing(); ImGui::SameLine();
}

void RenderEsp()
{
	ImGui::SetCursorPos(ImVec2(170 * dpi_scale, 15 * dpi_scale));
	ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.f);
	ImGui::BeginChild("esp_child", ImVec2(620 * dpi_scale, 540 * dpi_scale), false);
	{
		ImGui::SetCursorPos({ 20 * dpi_scale, 0 });
		ImGui::Text("ESP Players");
		ImGui::SetCursorPos({ 20 * dpi_scale, ImGui::CalcTextSize("ESP Players").y + 5 * dpi_scale });
		ImGui::BeginChild("esp_total", ImVec2(310 * dpi_scale, 220 * dpi_scale), true);
		{
			ImGui::Spacing();
			ImGui::Spacing();
			s(); ImGui::Checkbox("Enable", &settings::esp_enable);
			s(); ImGui::Checkbox("Only visible", &settings::esp_visible);
			s(); ImGui::Checkbox("ESP skeleton", &settings::esp_skeleton);
			s(); ImGui::Combo("Skeleton style", &settings::skeleton_color_type, "Common\0\rBased on health\0\0");
			s(); ImGui::Combo("Health style", &settings::health_color_type, "Common\0\rGradient\0\rBased on health\0\0");
			s(); ImGui::Combo("Box style", &settings::esp_box_type, "Box\0\rCorners\0\0");
			s(); ImGui::Checkbox("Box outline", &settings::box_outline);


		}
		ImGui::EndChild();
		ImGui::SetCursorPos({ 370 * dpi_scale, 0 });
		ImGui::Text("Sniper crosshair");
		ImGui::SetCursorPos({ 370 * dpi_scale, ImGui::CalcTextSize("Sniper crosshair").y + 5 * dpi_scale });
		ImGui::BeginChild("aim_settings", ImVec2(250 * dpi_scale, 160 * dpi_scale), true);
		{
			ImGui::Spacing();
			ImGui::Spacing();
			s(); ImGui::Checkbox("Enable", &settings::draw_crosshair);
			s(); ImGui::Checkbox("Outline", &settings::outline);
			s(); ImGui::SliderFloat("Padding", &settings::line_padding, 1, 20);
			s(); ImGui::SliderFloat("Length", &settings::line_length, 1, 20);
			s(); ImGui::SliderFloat("Width", &settings::line_width, 1, 5);
		}
		ImGui::EndChild();

		ImGui::SetCursorPos({ 20 * dpi_scale, 220 * dpi_scale + ImGui::CalcTextSize("ESP Players").y + 30 * dpi_scale });
		ImGui::Text("Windows");
		ImGui::SetCursorPos({ 20 * dpi_scale, 220 * dpi_scale + ImGui::CalcTextSize("ESP Players").y + 30 * dpi_scale + ImGui::CalcTextSize("Radar").y + 5 * dpi_scale });
		ImGui::BeginChild("radar", ImVec2(310 * dpi_scale, 210 * dpi_scale), true);
		{
			ImGui::Spacing();
			ImGui::Spacing();
			s(); ImGui::Checkbox("Radar", &settings::render_radar);
			s(); s(); s(); s(); ImGui::Checkbox("CrossLine", &settings::ShowRadarCrossLine);
			s(); s(); s(); s(); ImGui::SliderFloat("Scale", &settings::Proportion, 500.f, 3300.f, "%.1f");
			s(); s(); s(); s(); ImGui::SliderFloat("Range", &settings::RadarRange, 100.f, 300.f, "%.1f");
			s(); s(); s(); s(); ImGui::SliderFloat("Alpha##radar", &settings::radar_alpha, 0.f, 1.f, "%.1f");
			//s(); ImGui::Checkbox("Binds", &settings::binds_list);
			//s(); s(); s(); s(); ImGui::SliderFloat("Alpha##binds", &settings::binds_alpha, 0.f, 1.f, "%.1f");
		}
		ImGui::EndChild();

		ImGui::EndChild();
		
	}
	ImGui::EndChild();
	ImGui::PopStyleVar();
}

void RenderAim()
{
	
	ImGui::SetCursorPos(ImVec2(170 * dpi_scale, 15 * dpi_scale));
	ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.f);
	ImGui::BeginChild("aim_child", ImVec2(600 * dpi_scale, 540 * dpi_scale), false);
	{
		
	
			static int weapon_selection = 0;
			ImGui::SetCursorPos({ 20 * dpi_scale, 0 });
			ImGui::Text("Weapon selection");
			ImGui::SetCursorPos({ 20 * dpi_scale, ImGui::CalcTextSize("Weapon selection").y + 5 * dpi_scale });
			ImGui::BeginChild("aim_weapons", ImVec2(250 * dpi_scale, 500 * dpi_scale), true);
			{
				ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_ItemSpacing, ImVec2(10.f * dpi_scale, 10.f * dpi_scale));
				ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_FramePadding, ImVec2(10.f * dpi_scale, 0));
				ImGui::Spacing();
				for (int i = 0; i < weapon_names.size(); i++)
				{
					s();
					if (ImGui::Selectable(weapon_names.at(i).c_str(), i == weapon_selection))
						weapon_selection = i;
				}
				ImGui::PopStyleVar(2);
				ImGui::Spacing();
			}
			ImGui::EndChild();
			ImGui::SetCursorPos({ 310 * dpi_scale, 0 });
			ImGui::Text("Weapon settings");
			ImGui::SetCursorPos({ 310 * dpi_scale, ImGui::CalcTextSize("Weapon settings").y + 5 * dpi_scale });
			ImGui::BeginChild("aim_settings", ImVec2(300 * dpi_scale, 265 * dpi_scale), true);
			{
				ImGui::Spacing();
				ImGui::Spacing();
				if (weapon_selection > 0)
				{
					//s(); g_BindSystem->get_bind("aim_bot")->draw_keybind("Key", "##aim_key");
					s(); ImGui::Checkbox("Override global", &settings::aim_settings_bot[weapon_selection].override_global);
					s(); ImGui::Checkbox("Enable", &settings::aim_settings_bot[weapon_selection ].enable);
					s(); ImGui::SliderFloat("FOV", &settings::aim_settings_bot[weapon_selection ].aim_fov, 1, 100);
					//s(); ImGui::Checkbox("Use smooth", &settings::aim_settings_bot[weapon_selection ].smooth);
					s(); ImGui::SliderFloat("Smooth", &settings::aim_settings_bot[weapon_selection ].smooth_value, 0.1f, 1.f);
					s(); ImGui::Checkbox("Use RCS", &settings::aim_settings_bot[weapon_selection ].rcs);
					s(); ImGui::Combo("Bone", &settings::aim_settings_bot[weapon_selection - 1].bone_num, "Head\0\rNeck\0\rBody\0\rNearest\0\0");
					//s(); ImGui::SliderFloat("Switch delay", &settings::aim_settings_bot[weapon_selection].target_switch_delay, 0.1f, 3.f);
				}
				else
				{
					//s(); g_BindSystem->get_bind("aim_bot")->draw_keybind("Key", "##aim_key");
					s(); ImGui::Checkbox("Enable", &settings::global_aim.enable);
					s(); ImGui::SliderFloat("FOV", &settings::global_aim.aim_fov, 1, 100);
					//s(); ImGui::Checkbox("Use smooth", &settings::global_aim.smooth);
					s(); ImGui::SliderFloat("Smooth", &settings::global_aim.smooth_value, 0.1f, 1.f);
					s(); ImGui::Checkbox("Use RCS", &settings::global_aim.rcs);
					s(); ImGui::Combo("Bone", &settings::global_aim.bone_num, "Head\0\rNeck\0\rBody\0\rNearest\0\0");
					//s(); ImGui::SliderFloat("Switch delay", &settings::global_aim.target_switch_delay, 0.1f, 3.f);
				}
				s();  ImGui::Checkbox("Draw FOV", &settings::render_aim_fov);
				s();  ImGui::Checkbox("Draw Line", &settings::render_aim_line);
			}
			ImGui::EndChild();
			
			ImGui::SetCursorPos({ 310 * dpi_scale, 265 * dpi_scale + ImGui::CalcTextSize("Weapon settings").y + 20 * dpi_scale});
			ImGui::Text("Triggerbot settings");
			ImGui::SetCursorPos({ 310 * dpi_scale, ImGui::CalcTextSize("Triggerbot settings").y + 5 * dpi_scale + 265 * dpi_scale + ImGui::CalcTextSize("Weapon settings").y + 20 * dpi_scale });
			ImGui::BeginChild("trig_settings", ImVec2(250 * dpi_scale, 70 * dpi_scale), true);
			{
				ImGui::Spacing();
				ImGui::Spacing();
				if (weapon_selection > 0)
				{
					s(); ImGui::Checkbox("Override global", &settings::trigger_settings_bot[weapon_selection].override_global);
					s(); ImGui::Checkbox("Enable", &settings::trigger_settings_bot[weapon_selection].TriggerBot);
				}
				else
				{
					s(); ImGui::Checkbox("Enable", &settings::global_trigger.TriggerBot);
				}
			}
			ImGui::EndChild();

	}
	ImGui::EndChild();
	ImGui::PopStyleVar();
}

void RenderRadar()
{
	ImGui::SetCursorPos(ImVec2(170 * dpi_scale, 15 * dpi_scale));
	ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.f);
	ImGui::BeginChild("colors", ImVec2(600 * dpi_scale, 540 * dpi_scale), false);
	{
		static int color_selection = 0;
		ImGui::SetCursorPos({ 20 * dpi_scale, 0 });
		ImGui::Text("Color selection");
		ImGui::SetCursorPos({ 20 * dpi_scale, ImGui::CalcTextSize("Color selection").y + 5 * dpi_scale });
		ImGui::BeginChild("colors_list", ImVec2(250 * dpi_scale, 500 * dpi_scale), true);
		{
			ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_ItemSpacing, ImVec2(10.f * dpi_scale, 10.f * dpi_scale));
			ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_FramePadding, ImVec2(10.f * dpi_scale, 0));
			ImGui::Spacing();
			for (int i = 0; i < color_names.size(); i++)
			{
				s();
				if (ImGui::Selectable(color_names.at(i).c_str(), i == color_selection))
				{
					color_selection = i;
				}
			}
			ImGui::PopStyleVar(2);
			ImGui::Spacing();
		}
		ImGui::EndChild();
		ImGui::SetCursorPos({ 310 * dpi_scale, 0 });
		ImGui::Text("Color setting");
		ImGui::SetCursorPos({ 310 * dpi_scale, ImGui::CalcTextSize("Color setting").y + 5 * dpi_scale });
		ImGui::BeginChild("color_settings", ImVec2(250 * dpi_scale, 500 * dpi_scale), false);
		{
			
			ImGui::PushItemWidth(250 * dpi_scale);
			ImGui::ColorPicker3("##COLOR_PICKER", settings::colors_list[color_selection].value, ImGuiColorEditFlags_NoInputs);
			ImGui::PopItemWidth();
			auto draw = ImGui::GetWindowDrawList();
			ImVec4 clr = ImVec4(settings::colors_list[color_selection].value[0], settings::colors_list[color_selection].value[1], settings::colors_list[color_selection].value[2], settings::colors_list[color_selection].value[3]);
			draw->AddRectFilled(ImGui::GetWindowPos() + ImGui::GetCursorPos() + ImVec2(0, 10 * dpi_scale), ImGui::GetWindowPos() + ImGui::GetCursorPos() + ImVec2(250 * dpi_scale, 60 * dpi_scale), ImGui::GetColorU32(clr), 5.f);
			
		}
		ImGui::EndChild();
	}
	ImGui::EndChild();
	ImGui::PopStyleVar();
}
void RenderCfg(ImVec2 size, config_information info, int i)
{
	//add text content
	auto current_cursor_pos = ImGui::GetCursorPos();
	ImGui::SetCursorPos(current_cursor_pos + ImVec2(10 * dpi_scale, 6 * dpi_scale));

	std::string strName = cfg_manager->files[i];
	if (strName.length() > 16)
	{
		strName.erase(16, strName.length() - 16);
		strName.append(_S("..."));
	}

	ImGui::PushFont(menu_font);
	float text_height = ImGui::CalcTextSize("Title:").y;
	ImGui::Text("Title: "); ImGui::SameLine(); ImGui::Text(strName.c_str());
	ImGui::PopFont();

	std::string strName2 = info.author;
	if (strName2.length() > 16)
	{
		strName2.erase(16, strName2.length() - 16);
		strName2.append(_S("..."));
	}

	ImGui::SetCursorPos(current_cursor_pos + ImVec2(10 * dpi_scale, 8 * dpi_scale + text_height));
	ImGui::PushFont(small_esp_font);
	ImGui::Text("Author: "); ImGui::SameLine(); ImGui::Text(strName2.c_str());
	ImGui::PopFont();

	//add activity buttons
	ImVec2 load_button = ImVec2(100 * dpi_scale, 30 * dpi_scale);
	ImGui::SetCursorPos(current_cursor_pos + ImVec2(size.x - load_button.x - 10 * dpi_scale, 10 * dpi_scale));
	bool is_the_same = selected_config == i;
	if (is_the_same)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(140 / 255.f, 178 / 255.f, 127 / 255.f, 200 / 255.f));
		ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(140 / 255.f, 178 / 255.f, 127 / 255.f, 100 / 255.f));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);
	}
	else
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(140 / 255.f, 178 / 255.f, 127 / 255.f, 227 / 255.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(150 / 255.f, 188 / 255.f, 137 / 255.f, 237 / 255.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(160 / 255.f, 198 / 255.f, 147 / 255.f, 247 / 255.f));
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(140 / 255.f, 178 / 255.f, 127 / 255.f, 150 / 255.f));
		ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(140 / 255.f, 178 / 255.f, 127 / 255.f, 50 / 255.f));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);
	}
	if (ImGui::Button(selected_config == i? "Loaded" : "Load", load_button))
	{
		MessageBoxA(0, "called","", MB_OK);
		selected_config = i;
		std::thread(load_config).detach();
	}

	ImGui::PopStyleVar();
	ImGui::PopStyleColor(5);


	//adding border & item
	ImGui::SetCursorPos(current_cursor_pos);
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);
	if (ImGui::Button("", size)) {};
	ImGui::PopStyleVar();
	ImGui::PopStyleColor(3);

	//ImGui::PopFont();
	ImGui::SetCursorPos(current_cursor_pos + ImVec2(0, size.y + 5 * dpi_scale));	
}

void RenderConfigs()
{
	ImGui::SetCursorPos(ImVec2(200 * dpi_scale, 15 * dpi_scale));
	ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.f);
	ImGui::BeginChild("configs_child", ImVec2(620 * dpi_scale, 540 * dpi_scale), false);
	{
		if (need_update_cfg)
		{
			cfg_manager->config_files();
			need_update_cfg = false;
		}

		ImGui::Text("Configs list:");
		ImGui::BeginChild("Config List", { 620 * dpi_scale, 410.f * dpi_scale }, false);
		{
				int iter = 1;
				for (size_t i = 0; i < cfg_manager->files.size(); i++) 
				{
					bool is_the_same = selected_config == i;

					if (!is_the_same)
					{
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
						ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(140 / 255.f, 178 / 255.f, 127 / 255.f, 50 / 255.f));
						ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(140 / 255.f, 178 / 255.f, 127 / 255.f, 100 / 255.f));
						ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(140 / 255.f, 178 / 255.f, 127 / 255.f, 200 / 255.f));
						ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(140 / 255.f, 178 / 255.f, 127 / 255.f, 100 / 255.f));
						ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);
					}
					else
					{
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(140 / 255.f, 178 / 255.f, 127 / 255.f, 227 / 255.f));
						ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(150 / 255.f, 188 / 255.f, 137 / 255.f, 237 / 255.f));
						ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(160 / 255.f, 198 / 255.f, 147 / 255.f, 247 / 255.f));
						ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(140 / 255.f, 178 / 255.f, 127 / 255.f, 150 / 255.f));
						ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(140 / 255.f, 178 / 255.f, 127 / 255.f, 50 / 255.f));
						ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);
					}
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5.f * dpi_scale, 5.f * dpi_scale));
					std::string strName = cfg_manager->cfg_info[i].title;
					if (strName.length() > 10)
					{
						strName.erase(10, strName.length() - 10);
						strName.append(_S("..."));
					}

					if (ImGui::Button(strName.c_str(), { 140 * dpi_scale, 120.f * dpi_scale }))
					{
						selected_config = i;
						std::thread(load_config).detach();
					}
					if (iter % 4 != 0)
						ImGui::SameLine();
					iter++;

					ImGui::PopStyleVar(2);
					ImGui::PopStyleColor(5);

					//RenderCfg({ 600 * dpi_scale, 50.f * dpi_scale }, cfg_manager->cfg_info[i], i);
				}
			
		}
		ImGui::EndChild();

		
		ImGui::Dummy({ 0, 20 * dpi_scale });
		ImGui::Text("Create new: ");
		ImGui::SameLine();

		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.f);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);

		//ImGui::InputText(("##configname"), config_name, sizeof(config_name));
		CutomGUIConfig::control_string();

		ImGui::PopStyleVar(2);
		ImGui::SameLine();


		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(140 / 255.f, 178 / 255.f, 127 / 255.f, 227 / 255.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(150 / 255.f, 188 / 255.f, 137 / 255.f, 237 / 255.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(160 / 255.f, 198 / 255.f, 147 / 255.f, 247 / 255.f));
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(140 / 255.f, 178 / 255.f, 127 / 255.f, 150 / 255.f));
		ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(140 / 255.f, 178 / 255.f, 127 / 255.f, 50 / 255.f));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);

		if (ImGui::Button("+ Create"))
		{
			add_config();
		}
		ImGui::Dummy({ 0, 10 * dpi_scale });

		if (ImGui::Button("Open folder"))
		{
			std::string folder = "C:\\genesis_cs2_external\\configs\\";
			CreateDirectoryA(folder.c_str(), nullptr);
			ShellExecuteA(NULL, "open", folder.c_str(), NULL, NULL, SW_SHOWNORMAL);
		}
		ImGui::PopStyleVar();
		ImGui::PopStyleColor(5);
	}
	ImGui::EndChild();
	ImGui::PopStyleVar();
}

void RenderLua()
{
	std::string text = "Soon...";
	ImVec2 textsize = ImGui::CalcTextSize(text.c_str());
	ImGui::SetCursorPos(ImVec2(60 * dpi_scale, 0) + ImVec2(width / 2 - textsize.x / 2, height / 2 - textsize.y / 2));
	ImGui::Text(text.c_str());
}

void RenderMisc()
{
	std::string text = "Soon...";
	ImVec2 textsize = ImGui::CalcTextSize(text.c_str());
	ImGui::SetCursorPos(ImVec2(60 * dpi_scale, 0) + ImVec2(width / 2 - textsize.x / 2, height / 2 - textsize.y / 2));
	ImGui::Text(text.c_str());
}

inline void RadarSetting(Base_Radar& Radar)
{
	// Radar.SetPos({ Gui.Window.Size.x / 2,Gui.Window.Size.y / 2 });
	Radar.SetDrawList(ImGui::GetWindowDrawList());
	Radar.SetPos({ ImGui::GetWindowPos().x + settings::RadarRange, ImGui::GetWindowPos().y + settings::RadarRange  });
	Radar.SetProportion(settings::Proportion );
	Radar.SetRange(settings::RadarRange );
	Radar.SetSize(settings::RadarRange * 2 );
	//Radar.SetCrossColor(settings::RadarCrossLineColor );

	Radar.ShowCrossLine = settings::ShowRadarCrossLine;
}

void Render()
{
	if (GetAsyncKeyState(VK_INSERT) & 1)
	{
		menuShow = !menuShow;
	}

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	//ImGui::GetIO().MouseDrawCursor = menuShow;
	static bool setup_smth = false;
	static float float_smth = 10;

	logs_system::instance();

	// setup colors and some styles
	if (!menu_setupped)
		menu_setup(ImGui::GetStyle());

	if (settings::render_radar)
	{
		// Radar window
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4{ 29/255.f, 38 / 255.f, 53 / 255.f, settings::radar_alpha });
		ImGui::Begin("Radar", 0, ImGuiWindowFlags_NoDecoration);
		ImGui::SetWindowSize({ settings::RadarRange * 2, settings::RadarRange * 2 });
		{
			RadarSetting(g_Radar);
			g_Radar.Render();
		}
		ImGui::End();
		ImGui::PopStyleColor();
	}
	const auto min_binds_width = 250 * dpi_scale;
	if (settings::binds_list)
	{
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4{ 29 / 255.f, 38 / 255.f, 53 / 255.f, settings::binds_alpha/2 });

		std::vector<func> active_binds;
		for (auto item : g_BindSystem->get_binds_list()) {
			if (g_BindSystem->is_bind_active(item.name)) {
				active_binds.push_back(item);
			}
		}
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(9.f * dpi_scale, 9.f * dpi_scale));
		ImGui::Begin("binds_list", 0, ImGuiWindowFlags_NoDecoration);
		{
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(9.f * dpi_scale, 9.f * dpi_scale));
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));
			auto draw = ImGui::GetBackgroundDrawList();
			auto title_size = ImGui::CalcTextSize("Binds list");
			auto pos = ImGui::GetWindowPos();
			auto height = title_size.y * active_binds.size() * ImGui::GetStyle().ItemSpacing.y * (active_binds.size() + 1);
			ImGui::SetWindowSize({ min_binds_width , height });
			draw->AddRectFilled(pos, pos + ImVec2(min_binds_width, title_size.y + ImGui::GetStyle().ItemSpacing.y * 2), ImColor(29, 38, 53), 5.f);
			ImGui::PopStyleVar();
			ImGui::Text("Binds list");
			for (auto item : active_binds)
			{
				std::string type = item.type == hold_on ? "[ Hold ]" : "[ Toggle ]";
				std::string name = item.name;
				ImGui::Text(" "); ImGui::SameLine(); ImGui::Text(type.c_str()); ImGui::SameLine(); ImGui::Text(name.c_str());
			}
			ImGui::Text("Binds list");
			ImGui::Text("Binds list");
			ImGui::Text("Binds list");
			ImGui::Text("Binds list");
			ImGui::Text("Binds list");
			ImGui::Text("Binds list");
			ImGui::Text("Binds list");
			ImGui::Text("Binds list");
			
		}
		ImGui::End(); 
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor();
	}

	/////////////////////////////////////////////////////////////////////
	if (menuShow)
	{
		// default menu size
		const int x = 850, y = 560;

		// last active tab to switch effect & reverse alpha & preview alpha
		// IMPORTANT: DO TAB SWITCHING BY LAST_TAB!!!!!
		static int last_tab = active_tab;
		static bool preview_reverse = false;
		ImVec2 pos = ImVec2();
		InputHandler();

		ImGui::SetNextWindowSize(ImVec2(x, y));
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.f);
		ImGui::Begin("Genesis", nullptr, ImGuiWindowFlags_NoDecoration);
		{
			pos = ImGui::GetWindowPos();
			main_window = ImGui::GetCurrentWindow();
			if (update_dpi)
			{
				// prevent oversizing
				dpi_scale = min(1.8f, max(dpi_scale, 1.f));

				// font and window size setting
				ImGui::SetWindowFontScale(dpi_scale);
				ImGui::SetWindowSize(ImVec2(ImFloor(x * dpi_scale), ImFloor(y * dpi_scale)));

				// styles resizing
				dpi_resize(dpi_scale, ImGui::GetStyle());

				// setup new window sizes
				width = ImFloor(x * dpi_scale);
				height = ImFloor(y * dpi_scale);

				// end of dpi updating
				update_dpi = false;
			}
			    
				ImVec2 window_position = ImGui::GetWindowPos();
			  
				ImVec2 back_rect_pos = window_position + ImVec2(10 * dpi_scale, 188 * dpi_scale) + ImVec2(0, (7.f * dpi_scale + 30.f * dpi_scale) * last_tab);
				ImGui::GetWindowDrawList()->AddRectFilled(back_rect_pos, back_rect_pos + ImVec2(124 * dpi_scale, 30.f * dpi_scale), ImColor(69 / 255.f, 78 / 255.f, 93 / 255.f, (100 * (1.f - preview_alpha)) / 255.f), 6.f);
				ImGui::GetWindowDrawList()->AddCircle(back_rect_pos + ImVec2(20 * dpi_scale, (30.f * dpi_scale) / 2), 8.f * dpi_scale, ImColor(134 / 255.f, 167 / 255.f, 227 / 255.f, 255 / 255.f * (1.f - preview_alpha)), 6.f * dpi_scale * 3);
				ImGui::GetWindowDrawList()->AddCircleFilled(back_rect_pos + ImVec2(20 * dpi_scale, (30.f * dpi_scale) / 2), 3.f * dpi_scale, ImColor(134 / 255.f, 167 / 255.f, 227 / 255.f, 255 / 255.f * (1.f - preview_alpha)), 6.f * dpi_scale * 3);

				ImGui::SetCursorPos(ImVec2(10 * dpi_scale, 15 * dpi_scale));
				ImGui::BeginChild("##tabs_area", ImVec2(140 * dpi_scale, 550 * dpi_scale)); 
				{																			
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.f * dpi_scale, 7.f * dpi_scale));

					ImGui::Image(logo, ImVec2(140 * dpi_scale, 140 * dpi_scale));
					ImGui::Dummy(ImVec2(0, 20 * dpi_scale));
					//  we need some more checks to prevent over-switching tabs
					if (ImGui::MainTab("Aim", menu_font, "R", active_tab == 0, ImVec2(124 * dpi_scale, 30.f * dpi_scale)) && last_tab == active_tab && !preview_reverse) active_tab = 0;
					if (ImGui::MainTab("Visuals", menu_font, "L", active_tab == 1, ImVec2(124 * dpi_scale, 30.f * dpi_scale)) && last_tab == active_tab && !preview_reverse) active_tab = 1;
					if (ImGui::MainTab("Colors", menu_font, "L", active_tab == 2, ImVec2(124 * dpi_scale, 30.f * dpi_scale)) && last_tab == active_tab && !preview_reverse) active_tab = 2;
					if (ImGui::MainTab("Misc", menu_font, "V", active_tab == 3, ImVec2(124 * dpi_scale, 30.f * dpi_scale)) && last_tab == active_tab && !preview_reverse) active_tab = 3;
					if (ImGui::MainTab("Configs", menu_font, "P", active_tab == 4, ImVec2(124 * dpi_scale, 30.f * dpi_scale)) && last_tab == active_tab && !preview_reverse) active_tab = 4;
					if (ImGui::MainTab("Lua", menu_font, "M", active_tab == 5, ImVec2(124 * dpi_scale, 30.f * dpi_scale)) && last_tab == active_tab && !preview_reverse) active_tab = 5;
					ImGui::Dummy(ImVec2(0, 40 * dpi_scale));
					s(); s(); ImGui::Text("Username:");
					s(); s(); ImGui::Text(username.c_str());
					s(); s(); ImGui::Checkbox("Stream", &settings::use_anti_obs);
					ImGui::PopStyleVar();
				}
				ImGui::EndChild();

				ImGui::GetWindowDrawList()->AddLine(window_position + ImVec2(160 * dpi_scale, 0), window_position + ImVec2(160 * dpi_scale, ScreenHeight), ImColor(180, 180, 180, 30));
				if (last_tab != active_tab || (last_tab == active_tab && preview_reverse))
				{
					need_update_cfg = true;
					if (!preview_reverse)
					{
						if (preview_alpha == 1.f)
							preview_reverse = true;

						preview_alpha = std::clamp(preview_alpha + (6.f * ImGui::GetIO().DeltaTime), 0.01f, 1.f);
					}
					else
					{
						last_tab = active_tab;
						
						if (preview_alpha == 0.01f)
						{
							preview_reverse = false;
						}

						preview_alpha = std::clamp(preview_alpha - (6.f * ImGui::GetIO().DeltaTime), 0.01f, 1.f);
					}
				}
				else
					preview_alpha = std::clamp(preview_alpha - (6.f * ImGui::GetIO().DeltaTime), 0.0f, 1.f);


				
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, (1.f - preview_alpha) * 1.f);
					switch (last_tab)
					{
						case 0: RenderAim(); break;
						case 1: RenderEsp(); break;
						case 2: RenderRadar(); break;
						case 3: RenderMisc(); break;
						case 4: RenderConfigs(); break;
						case 5: RenderLua(); break;
							
						default: break;
					}
					ImGui::PopStyleVar();
				
			//if (ImGui::Button("notify"))
			//	logs_system::push_log("Welcome!", false);
		}
		ImGui::End();
		ImGui::PopStyleVar();
		
		if (active_tab == 1)
		{
			ImGui::PushStyleColor(ImGuiStyleVar_Alpha, (1.f - preview_alpha) * 1.f);
			ImGui::Begin(_S("Preview"), nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
			{
				auto draw = ImGui::GetWindowDrawList();
				ImGui::SetWindowPos(pos + ImVec2(x+10.f * dpi_scale, 0));
				ImGui::SetWindowSize({ 375 * dpi_scale, y });
				auto pos = ImGui::GetWindowPos();
				//draw->AddImage(model_preview, pos + ImVec2{ 105 * dpi_scale, 120 * dpi_scale }, pos + ImVec2{ 275 * dpi_scale, 440 * dpi_scale });
				draw->AddRectFilled(pos + ImVec2{ 105 * dpi_scale, 120 * dpi_scale }, pos + ImVec2{ 275 * dpi_scale, 440 * dpi_scale }, ImColor(19, 28, 43), 5.f);
				RenderPreview(x,y);
				
			}
			ImGui::End();
			ImGui::PopStyleColor();
		}
		need_to_draw_board = use_focus || use_bind;

		if (need_to_draw_board && 0)
			RenderScreenBoard();
	}
	g_Renderer->Instance();
	/////////////////////////////////////////////////////////////////////

	ImGui::EndFrame();

	DirectX9Interface::pDevice->SetRenderState(D3DRS_ZENABLE, false);
	DirectX9Interface::pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	DirectX9Interface::pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, false);

	DirectX9Interface::pDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
	if (DirectX9Interface::pDevice->BeginScene() >= 0) {
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		DirectX9Interface::pDevice->EndScene();
	}

	HRESULT result = DirectX9Interface::pDevice->Present(NULL, NULL, NULL, NULL);
	if (result == D3DERR_DEVICELOST && DirectX9Interface::pDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
		ImGui_ImplDX9_InvalidateDeviceObjects();
		DirectX9Interface::pDevice->Reset(&DirectX9Interface::pParams);
		ImGui_ImplDX9_CreateDeviceObjects();
	}
}


void MainLoop() {
	static RECT OldRect;
	ZeroMemory(&DirectX9Interface::Message, sizeof(MSG));

	while (DirectX9Interface::Message.message != WM_QUIT) 
	{
		HWND tst_HWND = FindWindowA(NULL, _S("Counter-Strike 2"));
		if (!tst_HWND)
			g_Utils->SelfDelete();



		if (PeekMessage(&DirectX9Interface::Message, OverlayWindow::Hwnd, 0, 0, PM_REMOVE)) {
			TranslateMessage(&DirectX9Interface::Message);
			DispatchMessage(&DirectX9Interface::Message);
		}
		HWND ForegroundWindow = GetForegroundWindow();
		if (ForegroundWindow == _HWND) 
		{
			HWND TempProcessHwnd = GetWindow(ForegroundWindow, GW_HWNDPREV);
			SetWindowPos(OverlayWindow::Hwnd, TempProcessHwnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
		//OverlayWindow::Hwnd = FindWindowA("", "overlay");
		RECT TempRect;
		POINT TempPoint;
		ZeroMemory(&TempRect, sizeof(RECT));
		ZeroMemory(&TempPoint, sizeof(POINT));

		GetClientRect(_HWND, &TempRect);
		ClientToScreen(_HWND, &TempPoint);

		TempRect.left = TempPoint.x;
		TempRect.top = TempPoint.y;
		ImGuiIO& io = ImGui::GetIO();
		io.ImeWindowHandle = _HWND;

		POINT TempPoint2;
		GetCursorPos(&TempPoint2);
		io.MousePos.x = TempPoint2.x - TempPoint.x;
		io.MousePos.y = TempPoint2.y - TempPoint.y;
		
		if (GetAsyncKeyState(0x1)) {
			io.MouseDown[0] = true;
			io.MouseClicked[0] = true;
			io.MouseClickedPos[0].x = io.MousePos.x;
			io.MouseClickedPos[0].x = io.MousePos.y;
		}
		else {
			io.MouseDown[0] = false;
		}

		//if (TempRect.left != OldRect.left || TempRect.right != OldRect.right || TempRect.top != OldRect.top || TempRect.bottom != OldRect.bottom) {
			OldRect = TempRect;
			ScreenWidth = TempRect.right;
			ScreenHeight = TempRect.bottom;
			DirectX9Interface::pParams.BackBufferWidth = ScreenWidth;
			DirectX9Interface::pParams.BackBufferHeight = ScreenHeight;
			SetWindowPos(OverlayWindow::Hwnd, (HWND)0, TempPoint.x, TempPoint.y, ScreenWidth, ScreenHeight, SWP_NOREDRAW);
			DirectX9Interface::pDevice->Reset(&DirectX9Interface::pParams);
		//}

		

		if (settings::use_anti_obs)
		{
			SetWindowDisplayAffinity(OverlayWindow::Hwnd, WDA_EXCLUDEFROMCAPTURE);
			SetWindowDisplayAffinity(_HWND, WDA_EXCLUDEFROMCAPTURE);
		}
		else
		{
			SetWindowDisplayAffinity(OverlayWindow::Hwnd, WDA_NONE);
			SetWindowDisplayAffinity(_HWND, WDA_NONE);
		}

		g_Renderer->ClearDrawData();

		g_BindSystem->manage_binds();
		hack::loop();

#ifndef clean_logs
		printf("loop entered...\n");
#endif

		Render(); 

#ifndef clean_logs
		printf("render entered...\n");
#endif

		g_Renderer->SwapDrawData();
	}
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	if (DirectX9Interface::pDevice != NULL) {
		DirectX9Interface::pDevice->EndScene();
		DirectX9Interface::pDevice->Release();
	}
	if (DirectX9Interface::Direct3D9 != NULL) {
		DirectX9Interface::Direct3D9->Release();
	}
	DestroyWindow(OverlayWindow::Hwnd);
	UnregisterClass(OverlayWindow::WindowClass.lpszClassName, OverlayWindow::WindowClass.hInstance);
}

bool DirectXInit() {
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &DirectX9Interface::Direct3D9))) {
		return false;
	}

	D3DPRESENT_PARAMETERS Params = { 0 };
	Params.Windowed = TRUE;
	Params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	Params.hDeviceWindow = OverlayWindow::Hwnd;
	Params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	Params.BackBufferFormat = D3DFMT_A8R8G8B8;
	Params.BackBufferWidth = ScreenWidth;
	Params.BackBufferHeight =ScreenHeight;
	Params.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	Params.EnableAutoDepthStencil = TRUE;
	Params.AutoDepthStencilFormat = D3DFMT_D16;
	Params.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	Params.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

	if (FAILED(DirectX9Interface::Direct3D9->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, OverlayWindow::Hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &Params, 0, &DirectX9Interface::pDevice))) {
		DirectX9Interface::Direct3D9->Release();
		return false;
	}

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantTextInput || ImGui::GetIO().WantCaptureKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.IniFilename = NULL;
	io.LogFilename = NULL;

	ImGui_ImplWin32_Init(OverlayWindow::Hwnd);
	ImGui_ImplDX9_Init(DirectX9Interface::pDevice);

	D3DXCreateTextureFromFileInMemory(DirectX9Interface::pDevice, &cheat_logo, sizeof(cheat_logo), &logo);
	D3DXCreateTextureFromFileInMemory(DirectX9Interface::pDevice, &espmodel, sizeof(espmodel), &model_preview);

	g_Renderer->SetupData();
	DirectX9Interface::Direct3D9->Release();
	
	return true;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, Message, wParam, lParam))
		return true;

	switch (Message) {
	case WM_DESTROY:
		if (DirectX9Interface::pDevice != NULL) {
			DirectX9Interface::pDevice->EndScene();
			DirectX9Interface::pDevice->Release();
		}
		if (DirectX9Interface::Direct3D9 != NULL) {
			DirectX9Interface::Direct3D9->Release();
		}
		PostQuitMessage(0);
		exit(4);
		break;
	case WM_SIZE:
		if (DirectX9Interface::pDevice != NULL && wParam != SIZE_MINIMIZED) {
			ImGui_ImplDX9_InvalidateDeviceObjects();
			DirectX9Interface::pParams.BackBufferWidth = LOWORD(lParam);
			DirectX9Interface::pParams.BackBufferHeight = HIWORD(lParam);
			HRESULT hr = DirectX9Interface::pDevice->Reset(&DirectX9Interface::pParams);
			if (hr == D3DERR_INVALIDCALL)
				IM_ASSERT(0);
			ImGui_ImplDX9_CreateDeviceObjects();
		}
		break;
	default:
		return DefWindowProc(hWnd, Message, wParam, lParam);
		break;
	}
	return 0;
}

void SetupWindow() {
	OverlayWindow::WindowClass = {
		sizeof(WNDCLASSEX), CS_CLASSDC, WinProc, 0, 0, nullptr, LoadIcon(nullptr, IDI_APPLICATION), LoadCursor(nullptr, IDC_ARROW), nullptr, nullptr, OverlayWindow::Name, LoadIcon(nullptr, IDI_APPLICATION)
	};

	RegisterClassEx(&OverlayWindow::WindowClass);
	if (_HWND) 
	{
		static RECT TempRect = { NULL };
		static POINT TempPoint;
		GetClientRect(_HWND, &TempRect);
		ClientToScreen(_HWND, &TempPoint);
		TempRect.left = TempPoint.x;
		TempRect.top = TempPoint.y;
		ScreenWidth = TempRect.right;
		ScreenHeight = TempRect.bottom;
	}
	
	OverlayWindow::Hwnd = CreateWindowEx(NULL, OverlayWindow::Name, OverlayWindow::Name, WS_POPUP | WS_VISIBLE, ScreenLeft, ScreenTop, ScreenWidth, ScreenHeight, NULL, NULL, 0, NULL);
	DwmExtendFrameIntoClientArea(OverlayWindow::Hwnd, &DirectX9Interface::Margin);
	SetWindowLong(OverlayWindow::Hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
	ShowWindow(OverlayWindow::Hwnd, SW_SHOW);
	UpdateWindow(OverlayWindow::Hwnd);
}

int writer(char* data, size_t size, size_t nmemb, std::string* buffer)
{
	int result = 0;
	if (buffer != NULL)
	{
		buffer->append(data, size * nmemb);
		result = size * nmemb;
	}
	return result;
}

inline BOOL set_registry(char* keyPath, char* keyName, const char* keyData)
{
	HKEY  key;
	long  error;
	DWORD disposition;

	error = RegCreateKeyExA(
		HKEY_CURRENT_USER, keyPath, 0, NULL, REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS, NULL, &key, &disposition
	);
	if (error) return FALSE;

	error = RegSetValueExA(
		key, keyName, 0, REG_SZ, (BYTE*)keyData, lstrlenA(keyData) + 1
	);
	RegCloseKey(key);
	if (error) return FALSE;

	return TRUE;
}

inline BOOL get_registry(const char* keyPath, const char* keyName, char* keyData)
{
	HKEY  key;
	long  error;
	char  content[1024];
	DWORD type = REG_SZ;
	DWORD size = 1024;

	error = RegOpenKeyExA(
		HKEY_CURRENT_USER, keyPath, 0, KEY_ALL_ACCESS, &key
	);
	if (error) return FALSE;

	error = RegQueryValueExA(
		key, keyName, NULL, &type, (BYTE*)content, &size
	);
	RegCloseKey(key);
	if (error) return FALSE;

	lstrcpyA(keyData, content);
	return TRUE;
}

#include <type_traits>

inline unsigned long DecToHex(DWORD src)
{
	return static_cast<std::make_unsigned<decltype(src)>::type>(src);
}


inline void UpdateOffsetsAndSigs()
{
	//DecToHex2("0x32C");
	//get token
	char szToken[1024] = { 0 };
	get_registry(_S("Software\\genesis\\"), _S("Token"), szToken);
	
	std::string tok = szToken;

	//MessageBoxA(0, tok.c_str(), "gen", MB_OK);

	CURL* pCurl = curl_easy_init();
	if (!pCurl)
		return;

	std::string szPostFields = _S("a=") + tok + " " + g_Utils->GetHWID() + " " + _S("c3Vja215ZGljaw==");
	std::string szServerResponse;

	curl_easy_setopt(pCurl, CURLOPT_URL, _S("https://genesistest.ru/php/cheat_login.php?"));
	curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, writer);
	curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, &szServerResponse);
	curl_easy_setopt(pCurl, CURLOPT_POSTFIELDS, szPostFields.c_str());

	CURLcode uResult = curl_easy_perform(pCurl);
	if (uResult == CURLE_OK)
	{
		//MessageBoxA(0, szServerResponse.c_str(), "gen", MB_OK);
		if (nlohmann::json::accept(szServerResponse))
		{
			nlohmann::json jReceivedMessage = nlohmann::json::parse(szServerResponse);

			username = jReceivedMessage[_S("username")].get<std::string>();
			/*
			Offset::Entity.Health = DecToHex(jReceivedMessage[_S("Health")].get<DWORD>());
			Offset::Entity.IsAlive = DecToHex(jReceivedMessage[_S("IsAlive")].get<DWORD>());
			Offset::Entity.iszPlayerName = DecToHex(jReceivedMessage[_S("iszPlayerName")].get<DWORD>());
			Offset::Entity.PlayerPawn = DecToHex(jReceivedMessage[_S("PlayerPawn")].get<DWORD>());
			Offset::Entity.TeamID = DecToHex(jReceivedMessage[_S("TeamID")].get<DWORD>());

			Offset::Pawn.IsDefusing = DecToHex(jReceivedMessage[_S("IsDefusing")].get<DWORD>());
			Offset::Pawn.IsScoped = DecToHex(jReceivedMessage[_S("IsScoped")].get<DWORD>());
			Offset::Pawn.Pos = DecToHex(jReceivedMessage[_S("Pos")].get<DWORD>());
			Offset::Pawn.MaxHealth = DecToHex(jReceivedMessage[_S("MaxHealth")].get<DWORD>());
			Offset::Pawn.CurrentHealth = DecToHex(jReceivedMessage[_S("CurrentHealth")].get<DWORD>());
			Offset::Pawn.GameSceneNode = DecToHex(jReceivedMessage[_S("GameSceneNode")].get<DWORD>());
			Offset::Pawn.BoneArray = DecToHex(jReceivedMessage[_S("BoneArray")].get<DWORD>());
			Offset::Pawn.angEyeAngles = DecToHex(jReceivedMessage[_S("angEyeAngles")].get<DWORD>());
			Offset::Pawn.vecLastClipCameraPos = DecToHex(jReceivedMessage[_S("vecLastClipCameraPos")].get<DWORD>());
			Offset::Pawn.pClippingWeapon = DecToHex(jReceivedMessage[_S("pClippingWeapon")].get<DWORD>());
			Offset::Pawn.iShotsFired = DecToHex(jReceivedMessage[_S("iShotsFired")].get<DWORD>());
			Offset::Pawn.aimPunchAngle = DecToHex(jReceivedMessage[_S("aimPunchAngle")].get<DWORD>());
			Offset::Pawn.iIDEntIndex = DecToHex(jReceivedMessage[_S("iIDEntIndex")].get<DWORD>());
			Offset::Pawn.iTeamNum = DecToHex(jReceivedMessage[_S("iTeamNum")].get<DWORD>());
			Offset::Pawn.CameraServices = DecToHex(jReceivedMessage[_S("CameraServices")].get<DWORD>());
			Offset::Pawn.iFov = DecToHex(jReceivedMessage[_S("iFov")].get<DWORD>());
			Offset::Pawn.bSpottedByMask = DecToHex(jReceivedMessage[_S("bSpottedByMask")].get<DWORD>());

			Offset::Signatures::EntityList = jReceivedMessage[_S("EntityList")].get<std::string>();
			Offset::Signatures::LocalPlayerController = jReceivedMessage[_S("LocalPlayerController")].get<std::string>();
			Offset::Signatures::ViewAngles = jReceivedMessage[_S("ViewAngles")].get<std::string>();
			Offset::Signatures::ViewMatrix = jReceivedMessage[_S("ViewMatrix")].get<std::string>();
			Offset::Signatures::LocalPlayerPawn = jReceivedMessage[_S("LocalPlayerPawn")].get<std::string>();
			*/
		}
		else
		{
			MessageBoxA(0, _S("Cheat log in error"), _S("Genesis"), MB_ICONERROR | MB_OK);
			curl_easy_cleanup(pCurl);
			g_Utils->SelfDelete();
		}
	}
	else
	{
		MessageBoxA(0, _S("Can't get data"), _S("Genesis"), MB_ICONERROR | MB_OK);
		curl_easy_cleanup(pCurl);
		g_Utils->SelfDelete();
	}
	curl_easy_cleanup(pCurl);
}

int main()
{
	::ShowWindow(::GetConsoleWindow(), SW_HIDE);
	MessageBoxA(0, _S("To activate cheat you have to click in CS2 window.\nIf the game is not opened - open it and click inside the window\nThe game should be in windowed or windowed fullscreen mode\nInsert - Open/Close menu"), _S("Genesis"), MB_OK);
#ifndef TEST_WINDOW 
	_HWND = FindWindowA(NULL, _S("Counter-Strike 2"));
#else
	_HWND = FindWindow(NULL, L"Калькулятор");
#endif

	
#ifndef TEST_WINDOW 
	UpdateOffsetsAndSigs();
	//std::cout << "Connecting to server..." << std::endl;
	//g_SystemCore->Instance();
#endif
#ifndef clean_logs
	std::cout << "[Render] Searching for window ..." << std::endl;
#endif

	while (_HWND!= GetForegroundWindow())
	{
		_HWND = FindWindowA(NULL, _S("Counter-Strike 2"));
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	RECT TempRect;
	GetWindowRect(_HWND, &TempRect);
	ScreenWidth = TempRect.right - TempRect.left;
	ScreenHeight = TempRect.bottom - TempRect.top;
	ScreenLeft = TempRect.left;
	ScreenRight = TempRect.right;
	ScreenTop = TempRect.top;
	ScreenBottom = TempRect.bottom;

#ifndef clean_logs
	std::cout << "[Render] Setupping overlay ..." << std::endl;
#endif

	OverlayWindow::Name = L"overlay";
	SetupWindow();
	DirectXInit();

#ifndef clean_logs
	std::cout << "[Render] Overlay has been set up" << std::endl;
	std::cout << "Creating directories ..." << std::endl;
#endif

	CreateDirectoryA(_S("C:/genesis_cs2_external/"), NULL);
	CreateDirectoryA(_S("C:/genesis_cs2_external/configs"), NULL);

#ifndef clean_logs
	std::cout << "Directories created" << std::endl;
	std::cout << "Setupping configs ..." << std::endl;
#endif

	cfg_manager->setup();

#ifndef clean_logs
	std::cout << "[HACK] Starting main loop..." << std::endl;
#endif

	auto ProcessStatus = ProcessMgr.Attach(_S("cs2.exe"));
	if (ProcessStatus != StatusCode::SUCCEED)
	{
		MessageBoxA(0, _S("Kernel start error 0x1\nSend screenshot to support"), _S("Genesis"), MB_ICONERROR | MB_OK);
		//std::cout << "[ERROR] Failed to attach process, StatusCode:" << ProcessStatus << std::endl;

		return -100;
	}

	if (!Offset::UpdateOffsets())
	{
		MessageBoxA(0, _S("Kernel start error 0x2\nSend screenshot to support"), _S("Genesis"), MB_ICONERROR | MB_OK);
		//std::cout << "[ERROR] Failed to update offsets." << std::endl;

		return -100;
	}

	if (!gGame.InitAddress())
	{
		MessageBoxA(0, _S("Kernel start error 0x3\nSend screenshot to support"), _S("Genesis"), MB_ICONERROR | MB_OK);
		//std::cout << "[ERROR] Failed to call InitAddress()." << std::endl;

		return -100;
	}
#ifndef clean_logs
	std::cout << "[Game] Pid:" << ProcessMgr.ProcessID << std::endl;
	std::cout << "[Game] Client:" << gGame.GetClientDLLAddress() << std::endl;

	std::cout << "Offset:" << std::endl;
	std::cout << "--EntityList:" << std::setiosflags(std::ios::uppercase) << std::hex << Offset::EntityList << std::endl;
	std::cout << "--Matrix:" << std::setiosflags(std::ios::uppercase) << std::hex << Offset::Matrix << std::endl;
	std::cout << "--LocalPlayerController:" << std::setiosflags(std::ios::uppercase) << std::hex << Offset::LocalPlayerController << std::endl;
	std::cout << "--ViewAngles:" << std::setiosflags(std::ios::uppercase) << std::hex << Offset::ViewAngle << std::endl;
	std::cout << "--LocalPlayerPawn:" << std::setiosflags(std::ios::uppercase) << std::hex << Offset::LocalPlayerPawn << std::endl;

	std::cout << "Runing..." << std::endl;
#endif
	g_BindSystem->register_bind({ "trigger_bot", {"", 0}, hold_on});
	g_BindSystem->register_bind({ "aim_bot", {"", 0}, hold_on });

	logs_system::push_log("Welcome!", false);

	while (TRUE)
		MainLoop();
}
