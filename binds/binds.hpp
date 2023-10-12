#pragma once

#include <windows.h>
#include <string>
#include <vector>

enum bind_type : int
{
	hold_on = 0,
	toggle = 1
};

struct key_bind
{
	std::string name;
	DWORD key = 0;
};

inline bool LabelClick(const char* label, bool* v, const char* unique_id)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	// The concatoff/on thingies were for my weapon config system so if we're going to make that, we still need this aids.
	char Buf[64];
	_snprintf(Buf, 64, _S("%s"), label);

	char getid[128];
	sprintf_s(getid, 128, _S("%s%s"), label, unique_id);


	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(getid);
	const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

	const ImRect check_bb(window->DC.CursorPos, ImVec2(label_size.y + style.FramePadding.y * 2 + window->DC.CursorPos.x, window->DC.CursorPos.y + label_size.y + style.FramePadding.y * 2));
	ImGui::ItemSize(check_bb, style.FramePadding.y);

	ImRect total_bb = check_bb;

	if (label_size.x > 0)
	{
		ImGui::SameLine(0, style.ItemInnerSpacing.x);
		const ImRect text_bb(ImVec2(window->DC.CursorPos.x, window->DC.CursorPos.y + style.FramePadding.y), ImVec2(window->DC.CursorPos.x + label_size.x, window->DC.CursorPos.y + style.FramePadding.y + label_size.y));

		ImGui::ItemSize(ImVec2(text_bb.GetWidth(), check_bb.GetHeight()), style.FramePadding.y);
		total_bb = ImRect(ImMin(check_bb.Min, text_bb.Min), ImMax(check_bb.Max, text_bb.Max));
	}

	if (!ImGui::ItemAdd(total_bb, id))
		return false;

	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(total_bb, id, &hovered, &held);
	if (pressed)
		*v = !(*v);

	if (*v)
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(126 / 255.f, 131 / 255.f, 219 / 255.f, 1.f));
	if (label_size.x > 0.0f)
		ImGui::RenderText(ImVec2(check_bb.GetTL().x + 12, check_bb.GetTL().y), Buf);
	if (*v)
		ImGui::PopStyleColor();

	return pressed;

}

inline bool input_shouldListen = false;


struct func
{
	std::string name;
	key_bind bind;
	bind_type type;
	bool active;
	bool prepare_to_change;

	void draw_keybind(const char* label, const char* unique_id);

};

inline std::vector<std::vector<key_bind>> binds_global_list =
{
	std::vector<key_bind>{
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

	std::vector<key_bind>{
		{"~", VK_OEM_3},
		{ "1", 0x31 },
		{ "2", 0x32 },
		{ "3", 0x33 },
		{ "4", 0x34 },
		{ "5", 0x35 },
		{ "6", 0x36 },
		{ "7", 0x37 },
		{ "8", 0x38 },
		{ "9", 0x39 },
		{ "0", 0x30 },
		{ "<-", VK_BACK },
		{ "Insert", VK_INSERT },
		{ "Home", VK_HOME },
		{ "Num Lc", VK_NUMLOCK },
	},

	std::vector<key_bind>{
		{"TAB", VK_TAB},
		{ "Q", 0x51 },
		{ "W", 0x57 },
		{ "E", 0x45 },
		{ "R", 0x52 },
		{ "T", 0x54 },
		{ "Y", 0x59 },
		{ "U", 0x55 },
		{ "I", 0x49 },
		{ "O", 0x4F },
		{ "P", 0x50 },
		{ "Enter", VK_RETURN },
		{ "Delete", VK_DELETE },
		{ "End", VK_END },
		},

			std::vector<key_bind>{
				{"CAPS", VK_TAB},
				{ "A", 0x41 },
				{ "S", 0x53 },
				{ "D", 0x44 },
				{ "F", 0x46 },
				{ "G", 0x47 },
				{ "H", 0x48 },
				{ "J", 0x4A },
				{ "K", 0x4B },
				{ "L", 0x4C },
		},

			std::vector<key_bind>{
				{"L Shift", VK_LSHIFT},
				{ "Z", 0x5A },
				{ "X", 0x58 },
				{ "C", 0x43 },
				{ "V", 0x56 },
				{ "B", 0x42 },
				{ "N", 0x4E },
				{ "M", 0x4D },
				{ "R Shift", VK_RSHIFT },
				{ "Up Arrow", VK_UP },
				},

					std::vector<key_bind>{
						{"L Ctrl", VK_LCONTROL},
						{ "L Win", VK_LWIN },
						{ "L Alt", VK_LMENU },
						{ "Space", VK_SPACE },
						{ "R Alt", VK_RMENU },
						{ "R Win", VK_RWIN },
						{ "R Ctrl", VK_RCONTROL },
						{ "L Arrow", VK_UP },
						{ "D Arrow", VK_DOWN },
						{ "R Arrow", VK_RIGHT },
				},

};


class C_BindSystem
{
private:
	std::vector<func> list;

public:
	std::vector<func> get_binds_list() {
		return list;
	}

	void register_bind(func function) {
		list.push_back(function);
	}

	func* get_bind(std::string func_name) {
		for (auto item : list)
			if (item.name == func_name) {
				return &item;
				
			}
	}

	void manage_binds() {
		for (auto item : list)
			if (item.type == toggle) {
				//if bind down and not going to change then prepare to cange on next loop
				if ((::GetKeyState(item.bind.key) & 0x8000) != 0 && !item.prepare_to_change) 
					item.prepare_to_change = true;

				//if on this loop key is not active but is going to change it seems that user up the key and we have to change is's value
				else if ((GetKeyState(item.bind.key) & 0x8000 == 0) && item.prepare_to_change) {
					item.active = !item.active;
					item.prepare_to_change = false;
				}
			}
	}

	bool is_bind_active(std::string func_name) {
		for (auto item : list)
			if (item.name == func_name) {
				if (item.type == hold_on)
					return (::GetKeyState(item.bind.key) & 0x8000) != 0;
				else if (item.type == toggle)
					return item.active;
			}		
	}

	std::string get_active_button() {
		std::string result = "";
		for (auto item : binds_global_list)
			for (auto cur_list : item)
			if (::GetKeyState(cur_list.key) & 0x8000) {
				result = cur_list.name;
				//MessageBoxA(0, result.c_str(), "2", MB_OK);
				break;
			}
		return result;
	}

	DWORD get_val_from_name(std::string name)
	{
		DWORD result = 0;
		for (auto item : binds_global_list)
			for (auto cur_list : item)
				if (cur_list.name == name) {
					result = cur_list.key;
					break;
				}
		return result;
	}

	std::string get_name_from_val(DWORD name)
	{
		std::string result = "";
		for (auto item : binds_global_list)
			for (auto cur_list : item)
				if (cur_list.key == name) {
					result = cur_list.name;
					MessageBoxA(0, result.c_str(), "", MB_OK);
					break;
				}
		return result;
	}
};

inline C_BindSystem* g_BindSystem = new C_BindSystem();

void func::draw_keybind(const char* label, const char* unique_id)
{
	auto clicked = false;
	auto text = g_BindSystem->get_active_button();
	// reset bind if we re pressing esc
	if (g_BindSystem->get_val_from_name(g_BindSystem->get_active_button()) == VK_ESCAPE)
	{
		input_shouldListen = false;
		clicked = false;
	}

	//if (text !="")
	//	MessageBoxA(0, std::to_string(bind.key).c_str(), "", MB_OK);
	if (bind.key <= 0 || bind.key > 112)
		text = _S("None");
	//else
	//	text = g_BindSystem->get_name_from_val(bind.key);
	//else
	//	text = bind.name;

	// if we clicked on keybind
	if (input_shouldListen )
	{
		clicked = true;
		text = _S("...");
	}

	if (input_shouldListen)
	{
		DWORD key;
		std::string res = g_BindSystem->get_active_button();
		if (res != "")
		{
			key = g_BindSystem->get_val_from_name(res);
			//text = res;
			bind.key = key;
			bind.name = res;

			input_shouldListen = false;
			clicked = false;
		}
	}

	auto textsize = ImGui::CalcTextSize(text.c_str()).x + 8 * dpi_scale;
	auto labelsize = ImGui::CalcTextSize(label);

	ImGui::Text(label);
	ImGui::SameLine();

	ImGui::SetCursorPosX(ImGui::GetWindowSize().x - (ImGui::GetWindowSize().x - ImGui::CalcItemWidth()) - 40 * dpi_scale);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3 * dpi_scale);

	if (ImGui::KeybindButton(text.c_str(), unique_id, ImVec2(max(50 * dpi_scale, textsize), 23 * dpi_scale), clicked))
		clicked = true;


	if (clicked)
	{
		input_shouldListen = true;
	}

	

	static auto hold = false, toggle = false;

	switch (type)
	{
	case bind_type::hold_on:
		hold = true;
		toggle = false;
		break;
	case bind_type::toggle:
		toggle = true;
		hold = false;
		break;
	}

	if (ImGui::BeginPopup(unique_id))
	{
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 6 * dpi_scale);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ((ImGui::GetCurrentWindow()->Size.x / 2) - (ImGui::CalcTextSize("Hold").x / 2)));
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 11);

		if (LabelClick(_S("Hold"), &hold, unique_id))
		{
			if (hold)
			{
				toggle = false;
				type = bind_type::hold_on;
			}
			else if (toggle)
			{
				hold = false;
				type = bind_type::toggle;
			}
			else
			{
				toggle = false;
				type = bind_type::hold_on;
			}

			ImGui::CloseCurrentPopup();
		}

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ((ImGui::GetCurrentWindow()->Size.x / 2) - (ImGui::CalcTextSize("Toggle").x / 2)));
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 11);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 9 * dpi_scale);

		if (LabelClick(_S("Toggle"), &toggle, unique_id))
		{
			if (toggle)
			{
				hold = false;
				type = bind_type::toggle;
			}
			else if (hold)
			{
				toggle = false;
				type = bind_type::hold_on;
			}
			else
			{
				hold = false;
				type = bind_type::toggle;
			}

			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}