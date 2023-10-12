#include "items.h"
#include "../menu.h"

inline std::vector<int> get_color_for_item(int iHealth) 
{
	if (iHealth <= 10) return { 51, 14, 0 };
	if (iHealth <= 30) return { 255, 72, 0 };
	if (iHealth <= 60) return { 255, 136, 0 };
	if (iHealth <= 80) return { 255, 234, 0 };
	if (iHealth <= 100) return { 0, 255, 0 };
}

ImColor upd(ImColor color)
{
	return ImColor(color.Value.x, color.Value.y, color.Value.z, color.Value.w * (1.f - preview_alpha));
}

void Username(bool pressed, ImDrawList* draw, ImVec2 pos, ImVec2& size, int cond)
{
	ImFont* m_Font = esp_font;
	if (!m_Font)
		return;

	int FontSize = settings::username_fontsize;
	ImVec2 ImTextSize = m_Font->CalcTextSizeA(FontSize, FLT_MAX, 0.0f, _S("GenesiS"));
	ImColor clr = ImColor(settings::get_color(_S("Username"))[0], settings::get_color(_S("Username"))[1], settings::get_color(_S("Username"))[2]);
	if (cond == LEFT_COND) {
		size = ImVec2(ImTextSize.x + 10, FontSize + 6);
		if (settings::esp_name_transform == 0)
			draw->AddText(m_Font, FontSize, pos + ImVec2(1, size.y / 2 - ImTextSize.y / 2), upd(clr), _S("GenesiS"));
		if (settings::esp_name_transform == 1)
			draw->AddText(m_Font, FontSize, pos + ImVec2(1, size.y / 2 - ImTextSize.y / 2), upd(clr), _S("GENESIS"));
		if (settings::esp_name_transform == 2)
			draw->AddText(m_Font, FontSize, pos + ImVec2(1, size.y / 2 - ImTextSize.y / 2), upd(clr), _S("genesis"));
	}

	if (cond == RIGHT_COND) {
		size = ImVec2(ImTextSize.x + 10, FontSize + 6);
		if (settings::esp_name_transform == 0)
			draw->AddText(m_Font, FontSize, pos + ImVec2(9, size.y / 2 - ImTextSize.y / 2), upd(clr), _S("GenesiS"));
		if (settings::esp_name_transform == 1)
			draw->AddText(m_Font, FontSize, pos + ImVec2(9, size.y / 2 - ImTextSize.y / 2), upd(clr), _S("GENESIS"));
		if (settings::esp_name_transform == 2)
			draw->AddText(m_Font, FontSize, pos + ImVec2(9, size.y / 2 - ImTextSize.y / 2), upd(clr), _S("genesis"));
	}

	if (cond == TOP_COND || cond == BOT_COND) {
		size = ImVec2(180, FontSize + 6);
		if (settings::esp_name_transform == 0)
			draw->AddText(m_Font, FontSize, pos + ImVec2(90 - ImTextSize.x / 2, size.y / 2 - ImTextSize.y / 2), upd(clr), _S("GenesiS"));
		if (settings::esp_name_transform == 1)
			draw->AddText(m_Font, FontSize, pos + ImVec2(90 - ImTextSize.x / 2, size.y / 2 - ImTextSize.y / 2), upd(clr), _S("GENESIS"));
		if (settings::esp_name_transform == 2)
			draw->AddText(m_Font, FontSize, pos + ImVec2(90 - ImTextSize.x / 2, size.y / 2 - ImTextSize.y / 2), upd(clr), _S("genesis"));
	}

	if (cond == IN_MOVE_COND || cond == POOL_COND) {
		size = ImVec2(70, 20);

		draw->AddRectFilled(pos, pos + size, ImColor(117/255.f, 117/255.f, 117/255.f, (255 * (1.f - preview_alpha)/255.f)), 3);
		draw->AddText(pos + ImVec2(2 * dpi_scale, 0), ImColor(255/255.f, 255/255.f, 255/255.f, (255 * (1.f - preview_alpha) / 255.f)), _S("Nickname"));
	}

	if (ImGui::BeginPopup(_S("UsernamePopup")))
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{1,1,1,1});
		ImGui::SetCursorPos({ 10, 10 });
		ImGui::BeginGroup();
		{
			ImGui::SliderInt(_S("Font Size"), &settings::username_fontsize, 9, 22); ImGui::SameLine(); ImGui::Spacing();
			ImGui::Combo("Transform", &settings::esp_name_transform, "Common\0\rUppercase\0\rLowercase\0\0");
			ImGui::Combo("Outline", &settings::esp_name_flag, "Disable\0\rShadow\0\rOutline\0\0");
			ImGui::Dummy({ 150, 10 });
		}
		ImGui::EndGroup();
		ImGui::PopStyleColor();
		ImGui::EndPopup();
	}

	if (pressed)
		ImGui::OpenPopup(_S("UsernamePopup"));
}

void Weapon(bool pressed, ImDrawList* draw, ImVec2 pos, ImVec2& size, int cond)
{
	ImFont* m_Font = esp_font;
	if (!m_Font)
		return;

	int FontSize = settings::weapon_fontsize;
	ImVec2 ImTextSize = m_Font->CalcTextSizeA(FontSize, FLT_MAX, 0.0f, settings::ammo_amount ? _S("Dual Elites [12]") : _S("Dual Elites"));
	ImColor clr = ImColor(settings::get_color(_S("Weapon"))[0], settings::get_color(_S("Weapon"))[1], settings::get_color(_S("Weapon"))[2]);
	std::string text = settings::ammo_amount ? _S("Dual Elites [12]") : _S("Dual Elites");

	if (cond == LEFT_COND) {
		size = ImVec2(ImTextSize.x + 10, ImTextSize.y + 6);

		std::string tr_str = text;
		std::transform(tr_str.begin(), tr_str.end(), tr_str.begin(), ::toupper);
		std::string tr_str2 = text;
		std::transform(tr_str2.begin(), tr_str2.end(), tr_str2.begin(), ::tolower);

		if (settings::esp_weapon_transform == 0)
			draw->AddText(m_Font, FontSize, pos + ImVec2(1, 3), upd(clr), text.c_str());
		if (settings::esp_weapon_transform == 1)
			draw->AddText(m_Font, FontSize, pos + ImVec2(1, 3), upd(clr), tr_str.c_str());
		if (settings::esp_weapon_transform == 2)
			draw->AddText(m_Font, FontSize, pos + ImVec2(1, 3), upd(clr), tr_str2.c_str());
	}

	if (cond == RIGHT_COND) {
		size = ImVec2(ImTextSize.x + 10, ImTextSize.y + 6);
		std::string tr_str = text;
		std::transform(tr_str.begin(), tr_str.end(), tr_str.begin(), ::toupper);
		std::string tr_str2 = text;
		std::transform(tr_str2.begin(), tr_str2.end(), tr_str2.begin(), ::tolower);

		if (settings::esp_weapon_transform == 0)
			draw->AddText(m_Font, FontSize, pos + ImVec2(9, 3), upd(clr), text.c_str());
		if (settings::esp_weapon_transform == 1)
			draw->AddText(m_Font, FontSize, pos + ImVec2(9, 3), upd(clr), tr_str.c_str());
		if (settings::esp_weapon_transform == 2)
			draw->AddText(m_Font, FontSize, pos + ImVec2(9, 3), upd(clr), tr_str2.c_str());
	}

	if (cond == TOP_COND || cond == BOT_COND) {
		size = ImVec2(180, ImTextSize.y + 6);
		std::string tr_str = text;
		std::transform(tr_str.begin(), tr_str.end(), tr_str.begin(), ::toupper);
		std::string tr_str2 = text;
		std::transform(tr_str2.begin(), tr_str2.end(), tr_str2.begin(), ::tolower);

		if (settings::esp_weapon_transform == 0)
		draw->AddText(m_Font, FontSize, pos + ImVec2(90 - ImGui::CalcTextSize(text.c_str()).x / 2, 3), upd(clr), text.c_str());
		if (settings::esp_weapon_transform == 1)
			draw->AddText(m_Font, FontSize, pos + ImVec2(90 - ImGui::CalcTextSize(tr_str.c_str()).x / 2, 3), upd(clr), tr_str.c_str());
		if (settings::esp_weapon_transform == 2)
			draw->AddText(m_Font, FontSize, pos + ImVec2(90 - ImGui::CalcTextSize(tr_str2.c_str()).x / 2, 3), upd(clr), tr_str2.c_str());
	}

	if (cond == IN_MOVE_COND || cond == POOL_COND) {
		size = ImVec2(60, 20);

		draw->AddRectFilled(pos, pos + size, ImColor(117/255.f, 117/255.f, 117/255.f, (255 * (1.f - preview_alpha)/255.f)), 3);
		draw->AddText(pos + ImVec2(2 * dpi_scale, 0), ImColor(255/255.f, 255/255.f, 255/255.f, (255 * (1.f - preview_alpha) / 255.f)), _S("Weapon"));
	}

	if (ImGui::BeginPopup(_S("WeaponPopup")))
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 1,1,1,1 });
		ImGui::SetCursorPos({ 10, 10 });
		ImGui::BeginGroup();
		{
			//ImGui::Checkbox(_S("Ammo Amount"), &settings::ammo_amount); ImGui::SameLine(); ImGui::Spacing();
			ImGui::SliderInt(_S("Font Size"), &settings::weapon_fontsize, 9, 22); ImGui::SameLine(); ImGui::Spacing();
			ImGui::Combo("Transform", &settings::esp_weapon_transform, "Common\0\rUppercase\0\rLowercase\0\0");
			ImGui::Combo("Outline", &settings::esp_weapon_flag, "Disable\0\rShadow\0\rOutline\0\0");
			ImGui::Dummy({ 150, 10 });
		}
		ImGui::EndGroup();
		ImGui::PopStyleColor();
		ImGui::EndPopup();
	}

	if (pressed)
		ImGui::OpenPopup(_S("WeaponPopup"));
}

void HealthBar(bool pressed, ImDrawList* draw, ImVec2 pos, ImVec2& size, int cond)
{
	ImColor clr = ImColor(settings::get_color(_S("Health Bar"))[0], settings::get_color(_S("Health Bar"))[1], settings::get_color(_S("Health Bar"))[2]);
	ImColor grad_top = ImColor(settings::get_color(_S("Health Gradient Top"))[0], settings::get_color(_S("Health Gradient Top"))[1], settings::get_color(_S("Health Gradient Top"))[2]);
	ImColor grad_bot = ImColor(settings::get_color(_S("Health Gradient Bottom"))[0], settings::get_color(_S("Health Gradient Bottom"))[1], settings::get_color(_S("Health Gradient Bottom"))[2]);
	ImColor halth_col = ImColor(get_color_for_item(80)[0], get_color_for_item(80)[1], get_color_for_item(80)[2]);

	if (cond == LEFT_COND || cond == RIGHT_COND) {
		size = ImVec2(6, 320);
		draw->AddRectFilled(pos + ImVec2(4, 0), pos + size - ImVec2(4, 0), upd(ImColor(0, 0, 0)));
		draw->AddRectFilled(pos + ImVec2(5, 1), pos + size - ImVec2(5, 1), upd(ImColor(0, 0, 0)));
		if (settings::health_color_type == 0)
			draw->AddRectFilled(pos + ImVec2(5, 1), pos + size - ImVec2(5, 60), clr);
		if (settings::health_color_type == 1)
			draw->AddRectFilledMultiColor(pos + ImVec2(5, 1), pos + size - ImVec2(5, 60), grad_top, grad_top, grad_bot, grad_bot);
		if (settings::health_color_type == 2)
			draw->AddRectFilled(pos + ImVec2(5, 1), pos + size - ImVec2(5, 60), halth_col);
	}

	if (cond == TOP_COND || cond == BOT_COND) {
		size = ImVec2(180, 6);
		if (settings::health_color_type == 0)
		{
			draw->AddRectFilled(pos + ImVec2(4, 0), pos + size - ImVec2(4, 0), clr);
			draw->AddRectFilled(pos + ImVec2(5, 3), pos + size - ImVec2(5, 3), clr);
		}
		if (settings::health_color_type == 1)
		{
			draw->AddRectFilledMultiColor(pos + ImVec2(4, 0), pos + size - ImVec2(4, 0), grad_top, grad_bot, grad_bot, grad_top);
			draw->AddRectFilledMultiColor(pos + ImVec2(5, 3), pos + size - ImVec2(5, 3), grad_top, grad_bot, grad_bot, grad_top);
		}
		if (settings::health_color_type == 2)
		{
			halth_col = ImColor(get_color_for_item(100)[0], get_color_for_item(100)[1], get_color_for_item(100)[2]);
			draw->AddRectFilled(pos + ImVec2(4, 0), pos + size - ImVec2(4, 0), halth_col);
			draw->AddRectFilled(pos + ImVec2(5, 3), pos + size - ImVec2(5, 3), halth_col);
		}
		//draw->AddRectFilled(pos + ImVec2(5, 3), pos + size - ImVec2(5, 3), clr);
	}

	if (cond == IN_MOVE_COND || cond == POOL_COND) {
		size = ImVec2(55, 20);

		draw->AddRectFilled(pos, pos + size, ImColor(117/255.f, 117/255.f, 117/255.f, (255 * (1.f - preview_alpha)/255.f)), 3);
		draw->AddText(pos + ImVec2(2 * dpi_scale, 0), ImColor(255/255.f, 255/255.f, 255/255.f, (255 * (1.f - preview_alpha) / 255.f)), _S("Health"));
	}
}

void ArmorBar(bool pressed, ImDrawList* draw, ImVec2 pos, ImVec2& size, int cond)
{
	ImColor clr = ImColor(settings::get_color(_S("Armor Bar"))[0], settings::get_color(_S("Armor Bar"))[1], settings::get_color(_S("Armor Bar"))[2]);
	if (cond == LEFT_COND || cond == RIGHT_COND) {
		size = ImVec2(6, 320);
		draw->AddRectFilled(pos + ImVec2(4, 0), pos + size - ImVec2(4, 0), upd(ImColor(0,0,0)));
		draw->AddRectFilled(pos + ImVec2(5, 1), pos + size - ImVec2(5, 1), upd(ImColor(0, 0, 0)));

		draw->AddRectFilled(pos + ImVec2(5, 1), pos + size - ImVec2(5, 98), upd(clr));
	}

	if (cond == TOP_COND || cond == BOT_COND) {
		size = ImVec2(180, 6);
		draw->AddRectFilled(pos + ImVec2(4, 0), pos + size - ImVec2(4, 0), upd(ImColor(0, 0, 0)));
		draw->AddRectFilled(pos + ImVec2(5, 3), pos + size - ImVec2(5, 3), upd(ImColor(0, 0, 0)));

		draw->AddRectFilled(pos + ImVec2(5, 3), pos + size - ImVec2(56, 3), upd(clr));
	}

	if (cond == IN_MOVE_COND || cond == POOL_COND) {
		size = ImVec2(45, 20);

		draw->AddRectFilled(pos, pos + size, ImColor(117/255.f, 117/255.f, 117/255.f, (255 * (1.f - preview_alpha)/255.f)), 3);
		draw->AddText(pos + ImVec2(2 * dpi_scale, 0), ImColor(255/255.f, 255/255.f, 255/255.f, (255 * (1.f - preview_alpha) / 255.f)), _S("Armor"));
	}
}

void Box(bool pressed, ImDrawList* draw, ImVec2 pos, ImVec2& size, int cond)
{
	ImColor clr = ImColor(settings::get_color(_S("Box Visible"))[0], settings::get_color(_S("Box Visible"))[1], settings::get_color(_S("Box Visible"))[2]);
	if (cond == CENTER_COND) {
		size = ImVec2(180, 320);
		if (settings::box_outline && settings::esp_box_type == 0)
		{
			draw->AddRect(pos , pos + size, upd(ImColor(0, 0, 0)), 0, 0, 2.f);
			draw->AddRect(pos , pos + size, upd(clr));
		}
		if (!settings::box_outline && settings::esp_box_type == 0)
		{
			//draw->AddRect(pos + ImVec2(4, 0), pos + size - ImVec2(4, 0), upd(ImColor(0, 0, 0)), 0, 0, 2.f);
			draw->AddRect(pos , pos + size, upd(clr));
		}
		if (settings::box_outline && settings::esp_box_type == 1)
		{
			draw->AddLine(pos, pos + ImVec2(size.x / 3, 0), upd(ImColor(0, 0, 0)), 2.f); // upper left
			draw->AddLine(pos, pos + ImVec2(0, size.y / 3), upd(ImColor(0, 0, 0)), 2.f); // upper left

			draw->AddLine(pos + ImVec2(0, size.y), pos + ImVec2(size.x / 3, size.y), upd(ImColor(0, 0, 0)), 2.f); // down left
			draw->AddLine(pos + ImVec2(0, size.y), pos + ImVec2(0, size.y - size.y / 3), upd(ImColor(0, 0, 0)), 2.f); // down left

			draw->AddLine(pos + size, pos + size - ImVec2(size.x / 3, 0), upd(ImColor(0, 0, 0)), 2.f); // down right
			draw->AddLine(pos + size, pos + size - ImVec2(0, size.y / 3), upd(ImColor(0, 0, 0)), 2.f); // down right

			draw->AddLine(pos + ImVec2(size.x, 0), pos + ImVec2(size.x, 0) - ImVec2(size.x / 3, 0), upd(ImColor(0, 0, 0)), 2.f); // upper right
			draw->AddLine(pos + ImVec2(size.x, 0), pos + ImVec2(size.x, 0) + ImVec2(0, size.y / 3), upd(ImColor(0, 0, 0)), 2.f); // upper right

			//rect
			draw->AddLine(pos , pos + ImVec2(size.x / 3, 0) , upd(clr)); // upper left
			draw->AddLine(pos , pos + ImVec2(0, size.y / 3) , upd(clr)); // upper left

			draw->AddLine(pos + ImVec2(0, size.y), pos + ImVec2(size.x / 3, size.y) , upd(clr)); // down left
			draw->AddLine(pos + ImVec2(0, size.y) , pos + ImVec2(0, size.y - size.y / 3) , upd(clr)); // down left

			draw->AddLine(pos + size , pos + size - ImVec2(size.x / 3, 0) , upd(clr)); // down right
			draw->AddLine(pos + size , pos + size - ImVec2(0, size.y / 3) , upd(clr)); // down right

			draw->AddLine(pos + ImVec2(size.x, 0) , pos + ImVec2(size.x, 0) - ImVec2(size.x / 3, 0) , upd(clr)); // upper right
			draw->AddLine(pos + ImVec2(size.x, 0),  pos + ImVec2(size.x, 0) + ImVec2(0, size.y / 3) , upd(clr)); // upper right
		}
		if (!settings::box_outline && settings::esp_box_type == 1)
		{
			//rect
			draw->AddLine(pos, pos + ImVec2(size.x / 3, 0), upd(clr)); // upper left
			draw->AddLine(pos, pos + ImVec2(0, size.y / 3), upd(clr)); // upper left

			draw->AddLine(pos + ImVec2(0, size.y), pos + ImVec2(size.x / 3, size.y), upd(clr)); // down left
			draw->AddLine(pos + ImVec2(0, size.y), pos + ImVec2(0, size.y - size.y / 3), upd(clr)); // down left

			draw->AddLine(pos + size, pos + size - ImVec2(size.x / 3, 0), upd(clr)); // down right
			draw->AddLine(pos + size, pos + size - ImVec2(0, size.y / 3), upd(clr)); // down right

			draw->AddLine(pos + ImVec2(size.x, 0), pos + ImVec2(size.x, 0) - ImVec2(size.x / 3, 0), upd(clr)); // upper right
			draw->AddLine(pos + ImVec2(size.x, 0), pos + ImVec2(size.x, 0) + ImVec2(0, size.y / 3), upd(clr)); // upper right
		}
	}

	if (cond == IN_MOVE_COND || cond == POOL_COND) {
		size = ImVec2(35, 20);

		draw->AddRectFilled(pos, pos + size, ImColor(117/255.f, 117/255.f, 117/255.f, (255 * (1.f - preview_alpha)/255.f)), 3);
		draw->AddText(pos + ImVec2(2 * dpi_scale , 0), ImColor(255 / 255.f, 255 / 255.f, 255 / 255.f, (255 * (1.f - preview_alpha) / 255.f)), _S("Box"));
	}
}

void Ammo(bool pressed, ImDrawList* draw, ImVec2 pos, ImVec2& size, int cond)
{
	ImColor clr = ImColor(settings::get_color(_S("Ammo Bar"))[0], settings::get_color(_S("Ammo Bar"))[1], settings::get_color(_S("Ammo Bar"))[2]);
	if (cond == LEFT_COND || cond == RIGHT_COND) {
		size = ImVec2(6, 320);
		draw->AddRectFilled(pos + ImVec2(4, 0), pos + size - ImVec2(4, 0), upd(ImColor(0, 0, 0)));
		draw->AddRectFilled(pos + ImVec2(5, 1), pos + size - ImVec2(5, 1), upd(ImColor(0, 0, 0)));
		draw->AddRectFilled(pos + ImVec2(5, 1), pos + size - ImVec2(5, 60), upd(clr));
	}

	if (cond == TOP_COND || cond == BOT_COND) {
		size = ImVec2(180, 6);
		draw->AddRectFilled(pos + ImVec2(4, 0), pos + size - ImVec2(4, 0), upd(ImColor(0, 0, 0)));
		draw->AddRectFilled(pos + ImVec2(5, 3), pos + size - ImVec2(5, 3), upd(ImColor(0, 0, 0)));
		draw->AddRectFilled(pos + ImVec2(5, 3), pos + size - ImVec2(56, 3), upd(clr));
	}

	if (cond == IN_MOVE_COND || cond == POOL_COND) {
		size = ImVec2(45, 20);

		draw->AddRectFilled(pos, pos + size, ImColor(117/255.f, 117/255.f, 117/255.f, (255 * (1.f - preview_alpha)/255.f)), 3);
		draw->AddText(pos + ImVec2(5, 3), ImColor(255/255.f, 255/255.f, 255/255.f, (255 * (1.f - preview_alpha) / 255.f)), _S("Ammo"));
	}
}

void Scope(bool pressed, ImDrawList* draw, ImVec2 pos, ImVec2& size, int cond)
{
	ImFont* m_Font =esp_font;
	if (!m_Font)
		return;
	ImColor clr = ImColor(settings::get_color(_S("Scoped"))[0], settings::get_color(_S("Scoped"))[1], settings::get_color(_S("Scoped"))[2]);

	int FontSize = settings::scope_fontsize;
	ImVec2 ImTextSize = m_Font->CalcTextSizeA(FontSize, FLT_MAX, 0.0f, _S("Scoped"));

	if (cond == LEFT_COND) {
		size = ImVec2(ImTextSize.x + 10, ImTextSize.y + 6);
		draw->AddText(m_Font, FontSize, pos + ImVec2(1, 3), upd(clr), _S("Scoped"));
	}

	if (cond == RIGHT_COND) {
		size = ImVec2(ImTextSize.x + 10, ImTextSize.y + 6);
		draw->AddText(m_Font, FontSize, pos + ImVec2(9, 3), upd(clr), _S("Scoped"));
	}

	if (cond == TOP_COND || cond == BOT_COND) {
		size = ImVec2(180, ImTextSize.y + 6);
		draw->AddText(m_Font, FontSize, pos + ImVec2(90 - ImTextSize.x / 2, 3), upd(clr), _S("Scoped"));
	}

	if (cond == IN_MOVE_COND || cond == POOL_COND) {
		size = ImVec2(45, 20);

		draw->AddRectFilled(pos, pos + size, ImColor(117/255.f, 117/255.f, 117/255.f, (255 * (1.f - preview_alpha)/255.f)), 3);
		draw->AddText(pos + ImVec2(5, 3), ImColor(255/255.f, 255/255.f, 255/255.f, (255 * (1.f - preview_alpha) / 255.f)), _S("Scope"));
	}

	if (ImGui::BeginPopup(_S("ScopePopup")))
	{
		ImGui::SetCursorPos({ 10, 10 });
		ImGui::BeginGroup();
		{
			ImGui::SliderInt(_S("Font Size"), &settings::scope_fontsize, 9, 22);
			ImGui::Dummy({ 150, 10 });
		}
		ImGui::EndGroup();

		ImGui::EndPopup();
	}

	if (pressed)
		ImGui::OpenPopup(_S("ScopePopup"));
}

void Defusing(bool pressed, ImDrawList* draw, ImVec2 pos, ImVec2& size, int cond)
{
	ImFont* m_Font =esp_font;
	if (!m_Font)
		return;
	ImColor clr = ImColor(settings::get_color(_S("Defusing"))[0], settings::get_color(_S("Defusing"))[1], settings::get_color(_S("Defusing"))[2]);
	int FontSize = settings::defusing_fontsize;
	ImVec2 ImTextSize = m_Font->CalcTextSizeA(FontSize, FLT_MAX, 0.0f, _S("Defusing"));

	if (cond == LEFT_COND) {
		size = ImVec2(ImTextSize.x + 10, ImTextSize.y + 6);
		draw->AddText(m_Font, FontSize, pos + ImVec2(1, 3), upd(clr), _S("Defusing"));
	}

	if (cond == RIGHT_COND) {
		size = ImVec2(ImTextSize.x + 10, ImTextSize.y + 6);
		draw->AddText(m_Font, FontSize, pos + ImVec2(9, 3), upd(clr), _S("Defusing"));
	}

	if (cond == TOP_COND || cond == BOT_COND) {
		size = ImVec2(180, ImTextSize.y + 6);
		draw->AddText(m_Font, FontSize, pos + ImVec2(90 - ImTextSize.x / 2, 3), upd(clr), _S("Defusing"));
	}

	if (cond == IN_MOVE_COND || cond == POOL_COND) {
		size = ImVec2(70, 20);

		draw->AddRectFilled(pos, pos + size, ImColor(117/255.f, 117/255.f, 117/255.f, (255 * (1.f - preview_alpha)/255.f)), 3);
		draw->AddText(pos + ImVec2(5, 3), ImColor(255/255.f, 255/255.f, 255/255.f, (255 * (1.f - preview_alpha) / 255.f)), _S("Defusing"));
	}

	if (ImGui::BeginPopup(_S("DefusingPopup")))
	{
		ImGui::SetCursorPos({ 10, 10 });
		ImGui::BeginGroup();
		{
			ImGui::SliderInt(_S("Font Size"), &settings::defusing_fontsize, 9, 22);
			ImGui::Dummy({ 150, 10 });
		}
		ImGui::EndGroup();

		ImGui::EndPopup();
	}

	if (pressed)
		ImGui::OpenPopup(_S("DefusingPopup"));
}

void Flashed(bool pressed, ImDrawList* draw, ImVec2 pos, ImVec2& size, int cond)
{
	ImFont* m_Font =esp_font;
	if (!m_Font)
		return;
	ImColor clr = ImColor(settings::get_color(_S("Flashed"))[0], settings::get_color(_S("Flashed"))[1], settings::get_color(_S("Flashed"))[2]);
	int FontSize = settings::flashed_fontsize;
	ImVec2 ImTextSize = m_Font->CalcTextSizeA(FontSize, FLT_MAX, 0.0f, _S("Flashed"));

	if (cond == LEFT_COND) {
		size = ImVec2(ImTextSize.x + 10, ImTextSize.y + 6);
		draw->AddText(m_Font, FontSize, pos + ImVec2(1, 3), upd(clr), _S("Flashed"));
	}

	if (cond == RIGHT_COND) {
		size = ImVec2(ImTextSize.x + 10, ImTextSize.y + 6);
		draw->AddText(m_Font, FontSize, pos + ImVec2(9, 3), upd(clr), _S("Flashed"));
	}

	if (cond == TOP_COND || cond == BOT_COND) {
		size = ImVec2(180, ImTextSize.y + 6);
		draw->AddText(m_Font, FontSize, pos + ImVec2(90 - ImTextSize.x / 2, 3), upd(clr), _S("Flashed"));
	}

	if (cond == IN_MOVE_COND || cond == POOL_COND) {
		size = ImVec2(55, 20);

		draw->AddRectFilled(pos, pos + size, ImColor(117/255.f, 117/255.f, 117/255.f, (255 * (1.f - preview_alpha)/255.f)), 3);
		draw->AddText(pos + ImVec2(5, 3), ImColor(255/255.f, 255/255.f, 255/255.f, (255 * (1.f - preview_alpha) / 255.f)), _S("Flashed"));
	}

	if (ImGui::BeginPopup(_S("FlashedPopup")))
	{
		ImGui::SetCursorPos({ 10, 10 });
		ImGui::BeginGroup();
		{
			ImGui::SliderInt(_S("Font Size"), &settings::flashed_fontsize, 9, 22);
			ImGui::Dummy({ 150, 10 });
		}
		ImGui::EndGroup();

		ImGui::EndPopup();
	}

	if (pressed)
		ImGui::OpenPopup(_S("FlashedPopup"));
}
/*
void Taser(bool pressed, ImDrawList* draw, ImVec2 pos, ImVec2& size, int cond)
{
	ImFont* m_Font =esp_font;
	if (!m_Font)
		return;
	ImColor clr = ImColor(settings::get_color("Taser")[0], settings::get_color("Taser")[1], settings::get_color("Taser")[2]);
	int FontSize = settings::taser_fontsize;
	ImVec2 ImTextSize = m_Font->CalcTextSizeA(FontSize, FLT_MAX, 0.0f, _S("Taser"));

	if (cond == LEFT_COND) {
		size = ImVec2(ImTextSize.x + 10, ImTextSize.y + 6);
		draw->AddText(m_Font, FontSize, pos + ImVec2(1, 3), upd(clr), _S("Taser"));
	}

	if (cond == RIGHT_COND) {
		size = ImVec2(ImTextSize.x + 10, ImTextSize.y + 6);
		draw->AddText(m_Font, FontSize, pos + ImVec2(9, 3), upd(clr), _S("Taser"));
	}

	if (cond == TOP_COND || cond == BOT_COND) {
		size = ImVec2(180, ImTextSize.y + 6);
		draw->AddText(m_Font, FontSize, pos + ImVec2(90 - ImTextSize.x / 2, 3), upd(clr), _S("Taser"));
	}

	if (cond == IN_MOVE_COND || cond == POOL_COND) {
		size = ImVec2(47, 20);

		draw->AddRectFilled(pos, pos + size, ImColor(117/255.f, 117/255.f, 117/255.f, (255 * (1.f - preview_alpha)/255.f)), 3);
		draw->AddText(pos + ImVec2(5, 3), ImColor(255/255.f, 255/255.f, 255/255.f, (255 * (1.f - preview_alpha) / 255.f)), _S("Taser"));
	}

	if (ImGui::BeginPopup(_S("TaserPopup")))
	{
		ImGui::SetCursorPos({ 10, 10 });
		ImGui::BeginGroup();
		{
			ImGui::SliderInt(_S("Font Size"), &settings::taser_fontsize, 9, 22);
			ImGui::Dummy({ 150, 10 });
		}
		ImGui::EndGroup();

		ImGui::EndPopup();
	}

	if (pressed)
		ImGui::OpenPopup(_S("TaserPopup"));
}

void Duck(bool pressed, ImDrawList* draw, ImVec2 pos, ImVec2& size, int cond)
{
	ImFont* m_Font =esp_font;
	if (!m_Font)
		return;
	ImColor clr = ImColor(settings::get_color("Duck")[0], settings::get_color("Duck")[1], settings::get_color("Duck")[2]);
	int FontSize = settings::duck_fontsize;
	ImVec2 ImTextSize = m_Font->CalcTextSizeA(FontSize, FLT_MAX, 0.0f, _S("FD"));

	if (cond == LEFT_COND) {
		size = ImVec2(ImTextSize.x + 10, ImTextSize.y + 6);
		draw->AddText(m_Font, FontSize, pos + ImVec2(1, 3), upd(clr), _S("FD"));
	}

	if (cond == RIGHT_COND) {
		size = ImVec2(ImTextSize.x + 10, ImTextSize.y + 6);
		draw->AddText(m_Font, FontSize, pos + ImVec2(9, 3), upd(clr), _S("FD"));
	}

	if (cond == TOP_COND || cond == BOT_COND) {
		size = ImVec2(180, ImTextSize.y + 6);
		draw->AddText(m_Font, FontSize, pos + ImVec2(90 - ImGui::CalcTextSize(_S("FD")).x / 2, 3), upd(clr), _S("FD"));
	}

	if (cond == IN_MOVE_COND || cond == POOL_COND) {
		size = ImVec2(55, 20);

		draw->AddRectFilled(pos, pos + size, ImColor(117/255.f, 117/255.f, 117/255.f, (255 * (1.f - preview_alpha)/255.f)), 3);
		draw->AddText(pos + ImVec2(5, 3), ImColor(255/255.f, 255/255.f, 255/255.f, (255 * (1.f - preview_alpha) / 255.f)), _S("FD"));
	}

	if (ImGui::BeginPopup(_S("duckingPopup")))
	{
		ImGui::SetCursorPos({ 10, 10 });
		ImGui::BeginGroup();
		{
			ImGui::SliderInt(_S("Font Size"), &settings::duck_fontsize, 9, 22);
			ImGui::Dummy({ 150, 10 });
		}
		ImGui::EndGroup();

		ImGui::EndPopup();
	}

	if (pressed)
		ImGui::OpenPopup(_S("duckingPopup"));
}
*/
void GetDrawableFunc(bool pressed, ImDrawList* draw, ImVec2 pos, ImVec2& size, int cond, int var) {
	switch (var) {
	case 0: return Weapon(pressed, draw, pos, size, cond); break;
	case 1: return Username(pressed, draw, pos, size, cond); break;
	case 2: return HealthBar(pressed, draw, pos, size, cond); break;
	//case 3: return ArmorBar(pressed, draw, pos, size, cond); break;
	case 3: return Box(pressed, draw, pos, size, cond); break;
	//case 5: return Ammo(pressed, draw, pos, size, cond); break;
	//case 6: return Scope(pressed, draw, pos, size, cond); break;
	//case 7: return Defusing(pressed, draw, pos, size, cond); break;
	//case 8: return Flashed(pressed, draw, pos, size, cond); break;
	//case 9: return Taser(pressed, draw, pos, size, cond); break;
	//case 10: return Duck(pressed, draw, pos, size, cond); break;
	}
}

std::vector<std::vector<MovableItems>> Pool = {
	std::vector<MovableItems>{}, // Left 0 
	std::vector<MovableItems>{}, // Right 1
	std::vector<MovableItems>{}, // Top 2
	std::vector<MovableItems>{}, // Bot 3
	std::vector<MovableItems>{}, // Center 4
	std::vector<MovableItems>{
		MovableItems(0, _S("Weapon"), POOL_COND, { 10 * dpi_scale, 490 * dpi_scale }, { 10 * dpi_scale, 490 * dpi_scale }, 0),
		MovableItems(1, _S("Username"), POOL_COND, { 75 * dpi_scale, 490 * dpi_scale }, { 75 * dpi_scale, 490 * dpi_scale}, 0),
		MovableItems(2, _S("Health"), POOL_COND, { 150 * dpi_scale, 490 * dpi_scale }, { 150 * dpi_scale, 490 * dpi_scale }, 1),
	//	MovableItems(3, _S("Armor"), POOL_COND, { 210, 540 }, { 210, 540 }, 1),
		MovableItems(3, _S("Box"), POOL_COND, { 210/*260*/ * dpi_scale, 490 * dpi_scale }, { 210/*260*/ * dpi_scale, 490 * dpi_scale }, 2),
		//MovableItems(5, _S("Ammo"), POOL_COND, { 300, 540 }, { 300, 540 }, 1),
	//	MovableItems(5, _S("Scope"), POOL_COND, { 10, 570 }, { 10, 570 }, 0),
	//	MovableItems(7, _S("Defusing"), POOL_COND, { 60, 570 }, { 60, 570 }, 0),
	//	MovableItems(6, _S("Flashed"), POOL_COND, { 135, 570 }, { 135, 570 }, 0),
	//	MovableItems(9, _S("Taser"), POOL_COND, { 195, 570 }, { 195, 570 }, 0),
	//	MovableItems(11, _S("FD"), POOL_COND, { 275, 570 }, { 275, 570 }, 0),
}, // Pool 5
std::vector<MovableItems>{}, // InMove 6
};

bool ItemInMove(MovableItems Item) {
	for (auto a = 0; a < Pool[IN_MOVE_COND].size(); a++) {
		if (Pool[IN_MOVE_COND][a].ItemName == Item.ItemName)
			return true;
	}
	return false;
}

std::tuple<int, int> GetMovableItemPosition(MovableItems Item)
{
	for (auto a = 0; a < 7; a++) {
		for (auto b = 0; b < Pool[a].size(); b++) {
			if (Pool[a][b].ItemName == Item.ItemName)
				return std::make_tuple(a, b);
		}
	}
}

std::tuple<int, int> GetMovableItemPosition(std::string name)
{
	for (auto a = 0; a < 7; a++) {
		for (auto b = 0; b < Pool[a].size(); b++) {
			if (Pool[a][b].ItemName == name)
				return std::make_tuple(a, b);
		}
	}
}

void Vector2Vector(MovableItems Item, int Destination)
{
	auto Position = GetMovableItemPosition(Item);

	Pool[std::get<0>(Position)].erase(Pool[std::get<0>(Position)].begin() + std::get<1>(Position)); // Erase From Position
	Pool[Destination].emplace_back(MovableItems(Item.Draw, Item.ItemName, Destination, Item.TemporaryPos, Item.BasicPositions, Item.Type)); // Append to Destination
}

void Vector2VectorIndexed(MovableItems Item, int Destination, int Index)
{
	auto Position = GetMovableItemPosition(Item);

	Pool[std::get<0>(Position)].erase(Pool[std::get<0>(Position)].begin() + std::get<1>(Position)); // Erase From Position
	Pool[Destination].emplace(Pool[Destination].cbegin() + Index, MovableItems(Item.Draw, Item.ItemName, Destination, Item.TemporaryPos, Item.BasicPositions, Item.Type)); // Append to Destination
}

static bool Handle(MovableItems& Item)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	ImGuiID id = window->GetID(Item.ItemName.c_str()); // use the pointer address as identifier
	ImGuiButtonFlags flags = 0;

	ImGuiContext& g = *GImGui;
	ImGuiStyle& style = g.Style;

	ImRect rect = ImRect(ImGui::GetWindowPos().x + Item.TemporaryPos.x,
		ImGui::GetWindowPos().y + Item.TemporaryPos.y,
		ImGui::GetWindowPos().x + Item.TemporaryPos.x + Item.WidgetSize.x,
		ImGui::GetWindowPos().y + Item.TemporaryPos.y + Item.WidgetSize.y);

	ImGui::ItemSize(rect, style.FramePadding.y);
	if (!ImGui::ItemAdd(rect, id))
		return false;

	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(rect, id, &hovered, &held, flags);
	bool is_active = ImGui::IsItemActive();

	ImGuiCol handle_color = ImColor(ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered]);
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	GetDrawableFunc(ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsMouseHoveringRect(rect.Min, rect.Max), draw_list, ImGui::GetWindowPos() + Item.TemporaryPos, Item.WidgetSize, Item.VectorCond, Item.Draw);

	if (is_active && ImGui::IsMouseDragging(0)) 
	{
		Item.TemporaryPos[0] += ImGui::GetIO().MouseDelta.x;
		Item.TemporaryPos[1] += ImGui::GetIO().MouseDelta.y;
	}

	return held;
}

bool MouseIntersectRect(ImVec2 pos1, ImVec2 pos2)
{
	if (ImGui::GetIO().MousePos.x >= ImGui::GetWindowPos().x + pos1.x &&
		ImGui::GetIO().MousePos.y >= ImGui::GetWindowPos().y + pos1.y &&
		ImGui::GetIO().MousePos.x <= ImGui::GetWindowPos().x + pos2.x &&
		ImGui::GetIO().MousePos.y <= ImGui::GetWindowPos().y + pos2.y)
		return true;

	return false;
}

void RecalculatePool(float animation)
{
	for (auto a = 0; a <= IN_MOVE_COND; a++)
	{
		for (auto b = 0; b < Pool[a].size(); b++)
		{
			if (a == IN_MOVE_COND || a == POOL_COND) {
				Pool[a][b].TemporaryPos = ImLerp(Pool[a][b].TemporaryPos, Pool[a][b].BasicPositions, animation);

				if (a == IN_MOVE_COND)
					Vector2Vector(Pool[a][b], POOL_COND);
			}
		}
	}
}

void RecalculateLeftRight(float animation)
{
	float PositionLeft = 0;
	float xModifier = 0;
	for (auto b = 0; b < Pool[LEFT_COND].size(); b++)
	{
		Pool[LEFT_COND][b].TemporaryPos = ImLerp(Pool[LEFT_COND][b].TemporaryPos, ImVec2(100 - Pool[LEFT_COND][b].WidgetSize.x - xModifier
			, 120 + PositionLeft), animation);

		if (b < 1)
		{
			if (Pool[LEFT_COND][0].ItemName == _S("Health") || Pool[LEFT_COND][0].ItemName == _S("Armor") ||
				Pool[LEFT_COND][1].ItemName == _S("Health") || Pool[LEFT_COND][1].ItemName == _S("Armor"))
				xModifier += 10;
		}

		if (Pool[LEFT_COND][b].ItemName != _S("Health") && Pool[LEFT_COND][b].ItemName != _S("Armor"))
			PositionLeft += Pool[LEFT_COND][b].WidgetSize.y;
	}


	float PositionRight = 0;
	float yModifier = 0;
	for (auto b = 0; b < Pool[RIGHT_COND].size(); b++)
	{
		Pool[RIGHT_COND][b].TemporaryPos = ImLerp(Pool[RIGHT_COND][b].TemporaryPos, ImVec2(280 + yModifier
			, 120 + PositionRight), animation);

		if (b < 1)
		{
			if (Pool[RIGHT_COND][0].ItemName == _S("Health") || Pool[RIGHT_COND][0].ItemName == _S("Armor") ||
				Pool[RIGHT_COND][1].ItemName == _S("Health") || Pool[RIGHT_COND][1].ItemName == _S("Armor"))
				yModifier += 10;
		}

		if (Pool[RIGHT_COND][b].ItemName != _S("Health") && Pool[RIGHT_COND][b].ItemName != _S("Armor"))
			PositionRight += Pool[RIGHT_COND][b].WidgetSize.y;
	}
}

void RecalculateTopBot(float animation)
{
	float PositionTop = 0;
	for (auto b = 0; b < Pool[TOP_COND].size(); b++)
	{
		PositionTop += Pool[TOP_COND][b].WidgetSize.y;
		Pool[TOP_COND][b].TemporaryPos = ImLerp(Pool[TOP_COND][b].TemporaryPos, ImVec2(100, 120 - PositionTop), animation);
	}

	float PositionBot = 0;
	for (auto b = 0; b < Pool[BOT_COND].size(); b++)
	{
		Pool[BOT_COND][b].TemporaryPos = ImLerp(Pool[BOT_COND][b].TemporaryPos, ImVec2(100, 440 + PositionBot), animation);
		PositionBot += Pool[BOT_COND][b].WidgetSize.y;
	}

	for (auto b = 0; b < Pool[CENTER_COND].size(); b++)
	{
		Pool[CENTER_COND][b].TemporaryPos = ImLerp(Pool[CENTER_COND][b].TemporaryPos, ImVec2(100, 120), animation);
	}
}

int emptyPos = 0;
int emptyPosPrediction = 0;
bool dragginAnyItem = false;

void IsHoveringStandingItem(ImDrawList* draw) {
	for (auto a = 0; a < CENTER_COND; a++)
	{
		for (auto b = 0; b < Pool[a].size(); b++)
		{
			auto pos = ImGui::GetWindowPos();
			if (ImGui::IsMouseHoveringRect(pos + Pool[a][b].TemporaryPos, pos + Pool[a][b].TemporaryPos + ImVec2(Pool[a][b].WidgetSize.x, Pool[a][b].WidgetSize.y / 2)))
			{
				//draw->AddRectFilled(pos + Pool[a][b].TemporaryPos, pos + Pool[a][b].TemporaryPos + Pool[a][b].WidgetSize, ImColor(50, 50, 50, 50), 8, 15);
				if (b == 0) {
					emptyPosPrediction = 0;
				}
				else {
					emptyPosPrediction = b - 1;
				}
			}
			else if (ImGui::IsMouseHoveringRect(pos + Pool[a][b].TemporaryPos + ImVec2(0, Pool[a][b].WidgetSize.y / 2), pos + Pool[a][b].TemporaryPos + Pool[a][b].WidgetSize)) {
				emptyPosPrediction = b + 1;
			}
		}
	}
}

void IsDragginAnyItem(ImDrawList* draw) {
	for (auto a = 0; a < CENTER_COND; a++)
	{
		for (auto b = 0; b < Pool[a].size(); b++)
		{
			auto pos = ImGui::GetWindowPos();
			if (ImGui::IsMouseHoveringRect(pos + Pool[a][b].TemporaryPos, pos + Pool[a][b].TemporaryPos + Pool[a][b].WidgetSize)
				&& ImGui::IsMouseDragging(0))
			{
				//draw->AddRectFilled(pos + Pool[a][b].TemporaryPos, pos + Pool[a][b].TemporaryPos + Pool[a][b].WidgetSize, ImColor(50, 50, 50, 50), 8, 15);
				dragginAnyItem = true;
			}
			else {
				dragginAnyItem = false;
			}
		}
	}
}

void RenderPreview(float x, float y)
{
	auto draw = ImGui::GetWindowDrawList();
	auto pos = ImGui::GetWindowPos();

	//draw pool rect
	draw->AddRectFilled(pos + ImVec2(0 * dpi_scale, 480 * dpi_scale), pos + ImVec2(380 * dpi_scale, y), ImColor(100, 100, 100, 50), 8, 15);

	//draw moving rect
	//draw->AddRectFilled(pos + ImVec2(410 * dpi_scale, 70 * dpi_scale), pos + ImVec2(590 * dpi_scale, 370 * dpi_scale), ImColor(50 / 255.f, 50 / 255.f, 50 / 255.f, (50 * (1.f - preview_alpha))/255.f), 8, 15);

	for (auto a = 0; a < 7; a++)
	{
		for (auto b = 0; b < Pool[a].size(); b++)
		{
			if (Handle(Pool[a][b]))
			{
				isMouseInAction = true;
				RecalculateAnimationFlag = true;

				if (MouseIntersectRect(ImVec2(0 * dpi_scale, 480 * dpi_scale), ImVec2(380 * dpi_scale, y))) {
					if (a == IN_MOVE_COND) {
						Pool[a][b].VectorCond = POOL_COND;
						Vector2Vector(Pool[a][b], POOL_COND);
					}
				}
				else if (MouseIntersectRect(ImVec2(0 * dpi_scale, 120 * dpi_scale), ImVec2(100 * dpi_scale, 400 * dpi_scale)) && Pool[a][b].ItemName != _S("Box")) {
					if (a == IN_MOVE_COND) {
						if (Pool[a][b].ItemName != _S("Health") && Pool[a][b].ItemName != _S("Armor")) {
							Pool[a][b].VectorCond = LEFT_COND;
							Vector2Vector(Pool[a][b], LEFT_COND);
						}
						else if (Pool[a][b].ItemName == _S("Health")) {
							Pool[a][b].VectorCond = LEFT_COND;
							Vector2VectorIndexed(Pool[a][b], LEFT_COND, 0);
						}
						else if (Pool[a][b].ItemName == _S("Armor")) {
							Pool[a][b].VectorCond = LEFT_COND;
							Vector2VectorIndexed(Pool[a][b], LEFT_COND, 0);
						}
					}
				}
				else if (MouseIntersectRect(ImVec2(280 * dpi_scale, 120 * dpi_scale), ImVec2(380 * dpi_scale, 440 * dpi_scale)) && Pool[a][b].ItemName != _S("Box")) {
					if (a == IN_MOVE_COND) {
						if (Pool[a][b].ItemName != _S("Health") && Pool[a][b].ItemName != _S("Armor")) {
							Pool[a][b].VectorCond = RIGHT_COND;
							Vector2Vector(Pool[a][b], RIGHT_COND);
						}
						else if (Pool[a][b].ItemName == _S("Health")) {
							Pool[a][b].VectorCond = RIGHT_COND;
							Vector2VectorIndexed(Pool[a][b], RIGHT_COND, 0);
						}
						else if (Pool[a][b].ItemName == _S("Armor")) {
							Pool[a][b].VectorCond = RIGHT_COND;
							Vector2VectorIndexed(Pool[a][b], RIGHT_COND, 0);
						}
					}
				}
				else if (MouseIntersectRect(ImVec2(100 * dpi_scale, 40 * dpi_scale), ImVec2(280 * dpi_scale, 120 * dpi_scale)) && Pool[a][b].ItemName != _S("Box")) {
					if (a == IN_MOVE_COND) {
						Pool[a][b].VectorCond = TOP_COND;
						Vector2Vector(Pool[a][b], TOP_COND);
					}
				}
				else if (MouseIntersectRect(ImVec2(100 * dpi_scale, 440 * dpi_scale), ImVec2(280 * dpi_scale, 520 * dpi_scale)) && Pool[a][b].ItemName != _S("Box")) {
					if (a == IN_MOVE_COND) {
						Pool[a][b].VectorCond = BOT_COND;
						Vector2Vector(Pool[a][b], BOT_COND);
					}
				}
				else if (MouseIntersectRect(ImVec2(100 * dpi_scale, 120 * dpi_scale), ImVec2(280 * dpi_scale, 440 * dpi_scale)) && Pool[a][b].ItemName == _S("Box")) {
					if (a == IN_MOVE_COND) {
						Pool[a][b].VectorCond = CENTER_COND;
						Vector2Vector(Pool[a][b], CENTER_COND);
					}
				}
				else {
					if (ImGui::GetIO().MouseDownDuration[0] > 0.0f) {
						Vector2Vector(Pool[a][b], IN_MOVE_COND);
					}
				}
			}

			if (!ImGui::GetIO().MouseDown[0])
				isMouseInAction = false;
		}
	}

	if (isMouseInAction == false)
	{
		if (RecalculateAnimationFlag == true) {
			RecalculateAnimation = 0.f;
			RecalculateAnimationFlag = false;
		}

		if (RecalculateAnimation < 1.f)
			RecalculateAnimation += 0.005f * (60 / ImGui::GetIO().Framerate);

		RecalculateAnimation = std::clamp(RecalculateAnimation, 0.f, 1.f);

		RecalculatePool(RecalculateAnimation);
		RecalculateLeftRight(RecalculateAnimation);
		RecalculateTopBot(RecalculateAnimation);
	}

	IsHoveringStandingItem(draw);
	IsDragginAnyItem(draw);
}
