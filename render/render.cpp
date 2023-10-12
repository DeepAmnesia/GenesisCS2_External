#include "render.hpp"
#include "../bytes/font_awesome.h"
#include "../bytes/icons_font_awesome5.hpp"
#include "../bytes/weapon_icon.h"
#include "../hacks/hack.hpp"

ImFont* esp_font;
ImFont* menu_font;
ImFont* small_esp_font;
ImFont* font_awesome_font;
ImFont* icons_font;

void C_Renderer::SetupData()
{
	ImFontConfig m_config;
	m_config.OversampleH = m_config.OversampleV = 3;
	m_config.PixelSnapH = false;

	static const ImWchar ranges[] =
	{
		0x0020, 0x00FF,
		0x0400, 0x044F,
		0
	}; 
	
	char windows_directory[64];
	GetWindowsDirectoryA(windows_directory, 64);
	auto tahoma_directory = (std::string)windows_directory + _S("\\Fonts\\Tahoma.ttf");

	ImGui::GetIO().Fonts->AddFontFromMemoryTTF((void*)OpenSansBold, sizeof(OpenSansBold), 18.f, &m_config, ranges);

	esp_font = ImGui::GetIO().Fonts->AddFontFromFileTTF(tahoma_directory.c_str(), 18.f, &m_config, ranges);
	small_esp_font = ImGui::GetIO().Fonts->AddFontFromMemoryTTF((void*)OpenSansBold, sizeof(OpenSansBold), 15.f, &m_config, ranges);
	icons_font = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedBase85TTF(WeaponIcons_compressed_data_base85, 20);

	ImFontConfig icon_config;
	icon_config.MergeMode = true;
	icon_config.GlyphMinAdvanceX = 13.0f;
	static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };

	font_awesome_font = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(font_awesome_data, font_awesome_size, 13.0f, &icon_config, icon_ranges);
}

void C_Renderer::SwapDrawData()
{
	std::unique_lock<std::shared_mutex> lock(m_DrawMutex);
	m_DrawListQueue.swap(m_DrawListQueueSafe);
}

void C_Renderer::ClearDrawData()
{
	if (!m_DrawListQueue.empty())
		m_DrawListQueue.clear();
}

void C_Renderer::Instance()
{
	std::unique_lock<std::shared_mutex> lock(m_DrawMutex);

	if (m_DrawListQueueSafe.empty())
		return;

	for (auto draw : m_DrawListQueueSafe)
	{
		if (!draw.m_Object.has_value())
			return;

		if (draw.m_Type == DrawObject_NONE) {}

		if (draw.m_Type == DrawObject_LINE)
		{
			const auto& pObject = std::any_cast<LineObject_t>(draw.m_Object);
			ImGui::GetBackgroundDrawList()->AddLine(pObject.m_Min, pObject.m_Max, pObject.m_Color, pObject.m_fThickness);
		}

		if (draw.m_Type == DrawObject_RECT)
		{
			const auto& pObject = std::any_cast<RectObject_t>(draw.m_Object);
			if (pObject.m_RectType == RectType_Clear)
				ImGui::GetBackgroundDrawList()->AddRect(pObject.m_Min, pObject.m_Max, pObject.m_Color, pObject.m_fRounding, pObject.m_tCorners, pObject.m_fThickness);
			if (pObject.m_RectType == RectType_Filled)
				ImGui::GetBackgroundDrawList()->AddRectFilled(pObject.m_Min, pObject.m_Max, pObject.m_Color, pObject.m_fRounding, pObject.m_tCorners);
			if (pObject.m_RectType == RectType_Corners)
			{
				ImVec2 size = pObject.m_Max - pObject.m_Min;
				ImVec2 pos = pObject.m_Min;
				ImGui::GetBackgroundDrawList()->AddLine(pos, pos + ImVec2(size.x / 3, 0), pObject.m_Color, pObject.m_fThickness); // upper left
				ImGui::GetBackgroundDrawList()->AddLine(pos, pos + ImVec2(size.x / 3, 0), pObject.m_Color, pObject.m_fThickness); // upper left
				ImGui::GetBackgroundDrawList()->AddLine(pos, pos + ImVec2(0, size.y / 3), pObject.m_Color, pObject.m_fThickness); // upper left

				ImGui::GetBackgroundDrawList()->AddLine(pos + ImVec2(0, size.y), pos + ImVec2(size.x / 3, size.y), pObject.m_Color, pObject.m_fThickness); // down left
				ImGui::GetBackgroundDrawList()->AddLine(pos + ImVec2(0, size.y), pos + ImVec2(0, size.y - size.y / 3), pObject.m_Color, pObject.m_fThickness); // down left

				ImGui::GetBackgroundDrawList()->AddLine(pos + size, pos + size - ImVec2(size.x / 3, 0), pObject.m_Color, pObject.m_fThickness); // down right
				ImGui::GetBackgroundDrawList()->AddLine(pos + size, pos + size - ImVec2(0, size.y / 3), pObject.m_Color, pObject.m_fThickness); // down right

				ImGui::GetBackgroundDrawList()->AddLine(pos + ImVec2(size.x, 0), pos + ImVec2(size.x, 0) - ImVec2(size.x / 3, 0), pObject.m_Color, pObject.m_fThickness); // upper right
				ImGui::GetBackgroundDrawList()->AddLine(pos + ImVec2(size.x, 0), pos + ImVec2(size.x, 0) + ImVec2(0, size.y / 3), pObject.m_Color, pObject.m_fThickness); // upper right
					
			}
		}

		if (draw.m_Type == DrawObject_RECTMULTI)
		{
			const auto& pObject = std::any_cast<RectMultiObject_t>(draw.m_Object);
			ImGui::GetBackgroundDrawList()->AddRectFilledMultiColor(pObject.m_Min, pObject.m_Max, pObject.m_Color1, pObject.m_Color2, pObject.m_Color3, pObject.m_Color4);
		}

		

		if (draw.m_Type == DrawObject_CIRCLE2D)
		{
			const auto& pObject = std::any_cast<Circle3dObject_t>(draw.m_Object);
			Vector3 vecScreenPosition;

			if (pObject.Pos == Vector3(0, 0, 0))
				vecScreenPosition = Vector3(ScreenCenterX, ScreenCenterY, 0);
			else
				vecScreenPosition = pObject.Pos;

			ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(vecScreenPosition.x, vecScreenPosition.y), pObject.Radius, pObject.Color, pObject.Points);
		}


		if (draw.m_Type == DrawObject_CIRCLE2DFiled)
		{
			const auto& pObject = std::any_cast<Circle3dObject_t>(draw.m_Object);

			Vector3 vecScreenPosition = Vector3(0, 0, 0);
			
			ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2(vecScreenPosition.x, vecScreenPosition.y), pObject.Radius, pObject.Color, pObject.Points);
		}

		if (draw.m_Type == DrawObject_Arc)
		{
			const auto& pObject = std::any_cast<ArcObject_t>(draw.m_Object);

			ImGui::GetBackgroundDrawList()->PathArcTo(pObject.Pos, pObject.Radius, DEG2RAD(pObject.MinAngle), DEG2RAD(pObject.MaxAngle), 32);
			ImGui::GetBackgroundDrawList()->PathStroke(pObject.Color, false, pObject.Thikness);
		}

		if (draw.m_Type == DrawObject_TEXT)
		{
			const auto& pObject = std::any_cast<TextObject_t>(draw.m_Object);

			ImGui::GetBackgroundDrawList()->PushTextureID(pObject.m_Font->ContainerAtlas->TexID);

			if (pObject.Outline)
			{
				
				ImGui::GetBackgroundDrawList()->AddText(pObject.m_Font, pObject.m_fFontSize, ImVec2(pObject.m_Pos.x + 1, pObject.m_Pos.y + 1), ImColor(0.0f, 0.0f, 0.0f, pObject.m_Color.Value.w), pObject.m_Text.c_str());
				ImGui::GetBackgroundDrawList()->AddText(pObject.m_Font, pObject.m_fFontSize, ImVec2(pObject.m_Pos.x - 1, pObject.m_Pos.y - 1), ImColor(0.0f, 0.0f, 0.0f, pObject.m_Color.Value.w), pObject.m_Text.c_str());
				ImGui::GetBackgroundDrawList()->AddText(pObject.m_Font, pObject.m_fFontSize, ImVec2(pObject.m_Pos.x + 1, pObject.m_Pos.y - 1), ImColor(0.0f, 0.0f, 0.0f, pObject.m_Color.Value.w), pObject.m_Text.c_str());
				ImGui::GetBackgroundDrawList()->AddText(pObject.m_Font, pObject.m_fFontSize, ImVec2(pObject.m_Pos.x - 1, pObject.m_Pos.y + 1), ImColor(0.0f, 0.0f, 0.0f, pObject.m_Color.Value.w), pObject.m_Text.c_str());
				ImGui::GetBackgroundDrawList()->AddText(pObject.m_Font, pObject.m_fFontSize, ImVec2(pObject.m_Pos.x, pObject.m_Pos.y + 1), ImColor(0.0f, 0.0f, 0.0f, pObject.m_Color.Value.w), pObject.m_Text.c_str());
				ImGui::GetBackgroundDrawList()->AddText(pObject.m_Font, pObject.m_fFontSize, ImVec2(pObject.m_Pos.x, pObject.m_Pos.y - 1), ImColor(0.0f, 0.0f, 0.0f, pObject.m_Color.Value.w), pObject.m_Text.c_str());
				ImGui::GetBackgroundDrawList()->AddText(pObject.m_Font, pObject.m_fFontSize, ImVec2(pObject.m_Pos.x + 1, pObject.m_Pos.y), ImColor(0.0f, 0.0f, 0.0f, pObject.m_Color.Value.w), pObject.m_Text.c_str());
				ImGui::GetBackgroundDrawList()->AddText(pObject.m_Font, pObject.m_fFontSize, ImVec2(pObject.m_Pos.x - 1, pObject.m_Pos.y), ImColor(0.0f, 0.0f, 0.0f, pObject.m_Color.Value.w), pObject.m_Text.c_str());
			}
			else
			{
				ImGui::GetBackgroundDrawList()->AddText(pObject.m_Font, pObject.m_fFontSize, ImVec2(pObject.m_Pos.x + 1, pObject.m_Pos.y + 1), ImColor(0.0f, 0.0f, 0.0f, pObject.m_Color.Value.w), pObject.m_Text.c_str());
			}

			ImGui::GetBackgroundDrawList()->AddText(pObject.m_Font, pObject.m_fFontSize, pObject.m_Pos, pObject.m_Color, pObject.m_Text.c_str());
			ImGui::GetBackgroundDrawList()->PopTextureID();
		}
	}
}

void C_Renderer::AddLine(ImVec2 min, ImVec2 max, ImColor col, float thickness)
{
	m_DrawListQueue.emplace_back(DrawQueueObject_t(DrawObject_LINE, LineObject_t{ min, max, col, thickness }));
}

void C_Renderer::AddRect(ImVec2 min, ImVec2 max, ImColor col, float thickness, float rounding, ImDrawCornerFlags corners, bool corner)
{
	m_DrawListQueue.emplace_back(DrawQueueObject_t(DrawObject_RECT, RectObject_t{ min, max, col, thickness, rounding, corners, corner? RectType_Corners : RectType_Clear }));
}

void C_Renderer::AddRectFilled(ImVec2 min, ImVec2 max, ImColor col, float rounding, ImDrawCornerFlags corners)
{
	m_DrawListQueue.emplace_back(DrawQueueObject_t(DrawObject_RECT, RectObject_t{ min, max, col, 0, rounding, corners, RectType_Filled }));
}

void C_Renderer::AddRectFilledMultiColor(ImVec2 min, ImVec2 max, ImColor col, ImColor col2, ImColor col3, ImColor col4)
{
	m_DrawListQueue.emplace_back(DrawQueueObject_t(DrawObject_RECTMULTI, RectMultiObject_t{ min, max, col, col2, col3, col4 }));
}

void C_Renderer::AddText(ImFont* font, float font_size, ImVec2 pos, std::string text, ImColor color, bool outline, bool shadow)
{
	m_DrawListQueue.emplace_back(DrawQueueObject_t(DrawObject_TEXT, TextObject_t{ font, font_size, pos, text, color, outline, shadow }));
}

void C_Renderer::AddCircle3D(Vector3 position, float points, float radius, ImColor color)
{
	m_DrawListQueue.emplace_back(DrawQueueObject_t(DrawObject_CIRCLE3D, Circle3dObject_t{ position, points, color, radius }));
}

void C_Renderer::AddCircle2D(Vector3 position, float points, float radius, ImColor color)
{
	m_DrawListQueue.emplace_back(DrawQueueObject_t(DrawObject_CIRCLE2D, Circle3dObject_t{ position, points, color, radius }));
}

void C_Renderer::AddCircle2DFilled(Vector3 position, float points, float radius, ImColor color)
{
	m_DrawListQueue.emplace_back(DrawQueueObject_t(DrawObject_CIRCLE2DFiled, Circle3dObject_t{ position, points, color, radius }));
}

void C_Renderer::AddArc(ImVec2 position, float radius, float min_angle, float max_angle, ImColor col, float thickness)
{
	m_DrawListQueue.emplace_back(DrawQueueObject_t(DrawObject_Arc, ArcObject_t{ position, radius, min_angle, max_angle, col, thickness }));
}
