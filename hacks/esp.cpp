#include "esp.hpp"

inline std::vector<int> get_color_for_esp(int iHealth)
{
	if (iHealth <= 10) return { 51, 14, 0 };
	if (iHealth <= 30) return { 255, 72, 0 };
	if (iHealth <= 60) return { 255, 136, 0 };
	if (iHealth <= 80) return { 255, 234, 0 };
	if (iHealth <= 100) return { 0, 255, 0 };
}

ImVec2 GetPosition(DraggableItemCondiction pos, Vector3 screen_pos, ImVec2 size, ESPPlayerData_t m_Data) {
	if (pos == RIGHT_COND)
		return ImVec2(screen_pos.x + size.x + 2 + m_Data.m_iRightOffset, screen_pos.y + m_Data.m_iRightDownOffset);
	if (pos == LEFT_COND)
		return ImVec2(screen_pos.x - 2 - m_Data.m_iLeftOffset, screen_pos.y + m_Data.m_iLeftDownOffset);
	if (pos == BOT_COND)
		return ImVec2(screen_pos.x + ((screen_pos.x + size.x - screen_pos.x) * 0.5f), screen_pos.y + size.y + 2 + m_Data.m_iDownOffset);
	if (pos == TOP_COND)
		return ImVec2(screen_pos.x + ((screen_pos.x + size.x - screen_pos.x) * 0.5f), screen_pos.y - m_Data.m_iUpOffset);
}

ImVec2 GetPositionOffsetless(DraggableItemCondiction pos, Vector3 screen_pos, ImVec2 size, ESPPlayerData_t m_Data) {
	if (pos == RIGHT_COND)
		return ImVec2(screen_pos.x + size.x + 3 + m_Data.m_iRightOffset, screen_pos.y);
	if (pos == LEFT_COND)
		return ImVec2(screen_pos.x - 3 - m_Data.m_iLeftOffset, screen_pos.y);
	if (pos == BOT_COND)
		return ImVec2(screen_pos.x, screen_pos.y + size.y + 2);
	if (pos == TOP_COND)
		return ImVec2(screen_pos.x, screen_pos.y - 4);
}

void C_PlayerESP::AddBar(DraggableItemCondiction pos, float& percentage, float max, ImColor color, ImColor color1, ImColor color2, Vector3 screen_pos, ImVec2 size, ESPPlayerData_t m_Data, int color_mode, int value) {

	ImVec2 Position = GetPositionOffsetless(pos, screen_pos, size, m_Data);
	int XOffset, X2Offset;
	int YOffset, Y2Offset;

	auto widthSides = abs(size.y) - (abs(size.y) * percentage) / max;
	auto widthUpDown = abs(size.x) - (abs(size.x) * percentage) / max;

	if (pos == RIGHT_COND) {
		XOffset = 1 + m_Data.m_iRightOffset;
		X2Offset = 3 + m_Data.m_iRightOffset;
		YOffset = 1;
		Y2Offset = -1 + size.y;
	}

	if (pos == LEFT_COND) {
		XOffset = 1 - m_Data.m_iLeftOffset - 4;
		X2Offset = 3 - m_Data.m_iLeftOffset - 4;
		YOffset = 1;
		Y2Offset = -1 + size.y;
	}

	if (pos == BOT_COND || pos == TOP_COND) {
		XOffset = 1;
		X2Offset = size.x - 1;
		YOffset = m_Data.m_iDownOffset;
		Y2Offset = 2 + m_Data.m_iDownOffset;
	}

	g_Renderer->AddRectFilled
	(
		{ (float)Position.x + XOffset - 1, (float)Position.y + YOffset -1},
		{ (float)Position.x + X2Offset + 1, (float)Position.y + Y2Offset  + 1},
		ImColor(0,0,0)
	);

	ImColor val_color = ImColor(get_color_for_esp(value)[0], get_color_for_esp(value)[1], get_color_for_esp(value)[2]);

	if (color_mode == 0)
	{
		g_Renderer->AddRectFilled
		(
			{ (float)Position.x + XOffset, (float)Position.y + YOffset + (pos == LEFT_COND || pos == RIGHT_COND ? widthSides : 0) },
			{ (float)Position.x + X2Offset - (pos == BOT_COND || pos == TOP_COND ? widthUpDown : 0), (float)Position.y + Y2Offset },
			ImColor(color.Value.x, color.Value.y, color.Value.z, color.Value.w)
		);
	}
	if (color_mode == 1)
	{
		if (pos == LEFT_COND || pos == RIGHT_COND)
		{
			g_Renderer->AddRectFilledMultiColor
			(
				{ (float)Position.x + XOffset, (float)Position.y + YOffset + (pos == LEFT_COND || pos == RIGHT_COND ? widthSides : 0) },
				{ (float)Position.x + X2Offset - (pos == BOT_COND || pos == TOP_COND ? widthUpDown : 0), (float)Position.y + Y2Offset },
				ImColor(color1.Value.x, color1.Value.y, color1.Value.z, color1.Value.w),
				ImColor(color1.Value.x, color1.Value.y, color1.Value.z, color1.Value.w),
				ImColor(color2.Value.x, color2.Value.y, color2.Value.z, color2.Value.w),
				ImColor(color2.Value.x, color2.Value.y, color2.Value.z, color2.Value.w)
			);
		}
		if (pos == TOP_COND || pos == BOT_COND)
		{
			g_Renderer->AddRectFilledMultiColor
			(
				{ (float)Position.x + XOffset, (float)Position.y + YOffset + (pos == LEFT_COND || pos == RIGHT_COND ? widthSides : 0) },
				{ (float)Position.x + X2Offset - (pos == BOT_COND || pos == TOP_COND ? widthUpDown : 0), (float)Position.y + Y2Offset },
				ImColor(color1.Value.x, color1.Value.y, color1.Value.z, color1.Value.w),
				ImColor(color2.Value.x, color2.Value.y, color2.Value.z, color2.Value.w),
				ImColor(color2.Value.x, color2.Value.y, color2.Value.z, color2.Value.w),
				ImColor(color1.Value.x, color1.Value.y, color1.Value.z, color1.Value.w)
			);
		}
	}
	if (color_mode == 2)
	{
		g_Renderer->AddRectFilled
		(
			{ (float)Position.x + XOffset, (float)Position.y + YOffset + (pos == LEFT_COND || pos == RIGHT_COND ? widthSides : 0) },
			{ (float)Position.x + X2Offset - (pos == BOT_COND || pos == TOP_COND ? widthUpDown : 0), (float)Position.y + Y2Offset },
			ImColor(val_color.Value.x, val_color.Value.y, val_color.Value.z, val_color.Value.w)
		);
	}

	if (pos == RIGHT_COND)
		m_Data.m_iRightOffset = m_Data.m_iRightOffset + 3;
	if (pos == LEFT_COND)
		m_Data.m_iLeftOffset = m_Data.m_iLeftOffset + 3;
	if (pos == BOT_COND)
		m_Data.m_iDownOffset = m_Data.m_iDownOffset + 5;
	if (pos == TOP_COND)
		m_Data.m_iUpOffset = m_Data.m_iUpOffset + 5;
}

void C_PlayerESP::AddText(std::string text, int font_size, DraggableItemCondiction pos, ImColor color, Vector3 screen_pos, ImVec2 size, ESPPlayerData_t m_Data, int transform, int outline) {
	ImFont* m_Font = esp_font;
	if (!m_Font)
		return;

	ImVec2 ImTextSize = m_Font->CalcTextSizeA(font_size, FLT_MAX, 0.0f, text.c_str());
	std::string tr_str = text;
	std::transform(tr_str.begin(), tr_str.end(), tr_str.begin(), ::toupper);
	std::string tr_str2 = text;
	std::transform(tr_str2.begin(), tr_str2.end(), tr_str2.begin(), ::tolower);
	std::string res_text = text;

	if (transform == 1)
	{
		ImTextSize = m_Font->CalcTextSizeA(font_size, FLT_MAX, 0.0f, tr_str.c_str());
		res_text = tr_str;
	}
	if (transform == 2)
	{
		ImTextSize = m_Font->CalcTextSizeA(font_size, FLT_MAX, 0.0f, tr_str2.c_str());
		res_text = tr_str2;
	}

	ImVec2 Position = GetPosition(pos, screen_pos, size, m_Data);

	if (pos == LEFT_COND)
		Position = Position - ImVec2(ImTextSize.x + 4, 0);
	if (pos == RIGHT_COND)
		Position = Position + ImVec2(4, 0);
	if (pos == BOT_COND)
		Position = Position - ImVec2(ImTextSize.x / 2, 0);
	if (pos == TOP_COND)
		Position = Position - ImVec2(ImTextSize.x / 2, ImTextSize.y);

	if (outline == 0)
	g_Renderer->AddText(
		m_Font, font_size,
		Position, res_text,
		ImColor(color.Value.x, color.Value.y, color.Value.z, color.Value.w), false, false
	);
	if (outline == 1)
		g_Renderer->AddText(
			m_Font, font_size,
			Position, res_text,
			ImColor(color.Value.x, color.Value.y, color.Value.z, color.Value.w), false, true
		);
	if (outline == 2)
		g_Renderer->AddText(
			m_Font, font_size,
			Position, res_text,
			ImColor(color.Value.x, color.Value.y, color.Value.z, color.Value.w), true, false
		);

	if (pos == RIGHT_COND)
		m_Data.m_iRightDownOffset = m_Data.m_iRightDownOffset + ImTextSize.y;
	if (pos == LEFT_COND)
		m_Data.m_iLeftDownOffset = m_Data.m_iLeftDownOffset + ImTextSize.y;
	if (pos == BOT_COND)
		m_Data.m_iDownOffset = m_Data.m_iDownOffset + ImTextSize.y;
	if (pos == TOP_COND)
		m_Data.m_iUpOffset = m_Data.m_iUpOffset + ImTextSize.y;
}


void C_PlayerESP::RenderBox(Vector3 screen_pos, ImVec2 size, ESPPlayerData_t m_Data, ImColor color, int style, bool outline)
{
	if (style == 0 && outline)
	{
		g_Renderer->AddRect(
			{ (float)screen_pos.x, (float)screen_pos.y },
			{ (float)screen_pos.x + size.x, (float)screen_pos.y + size.y },
			ImColor(0, 0, 0), 2);

		g_Renderer->AddRect(
			{ (float)screen_pos.x, (float)screen_pos.y },
			{ (float)screen_pos.x + size.x, (float)screen_pos.y + size.y },
			color);
	}
	if (style == 0 && !outline)
	{
		g_Renderer->AddRect(
			{ (float)screen_pos.x, (float)screen_pos.y },
			{ (float)screen_pos.x + size.x, (float)screen_pos.y + size.y },
			color);
	}
	if (style == 1 && outline)
	{
		g_Renderer->AddRect(
			{ (float)screen_pos.x, (float)screen_pos.y },
			{ (float)screen_pos.x + size.x, (float)screen_pos.y + size.y },
			ImColor(0, 0, 0), 2, 0, 0, true);

		g_Renderer->AddRect(
			{ (float)screen_pos.x, (float)screen_pos.y },
			{ (float)screen_pos.x + size.x, (float)screen_pos.y + size.y },
			color, 1, 0, 0, true);
	}
	if (style == 1 && !outline)
	{
		g_Renderer->AddRect(
			{ (float)screen_pos.x, (float)screen_pos.y },
			{ (float)screen_pos.x + size.x, (float)screen_pos.y + size.y },
			color, 1, 0, 0, true);
	}
}
