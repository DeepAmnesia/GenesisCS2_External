#pragma once
#include <unordered_map>
#include <algorithm>
#include "../configs/cheat_cfg.hpp"
#include "../draggable_esp/items.h"
#include "../classes/vector.hpp"
#include "../libs/imgui/imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "../libs/imgui/imgui_internal.h"
#include "../render/render.hpp"

constexpr float SPEED_FREQ = 255 / 4.0f;

struct ESPPlayerData_t
{
	int m_iDownOffset = 0;
	int m_iUpOffset = 0;

	int m_iLeftDownOffset = 0;
	int m_iLeftOffset = 0;

	int m_iRightDownOffset = 0;
	int m_iRightOffset = 0;

};
class C_PlayerESP
{
public:
	virtual void AddBar(DraggableItemCondiction pos, float& percentage, float max, ImColor color, ImColor color1, ImColor color2, Vector3 screen_pos, ImVec2 size, ESPPlayerData_t data, int color_mode, int value = 0);
	virtual void AddText(std::string text, int font_size, DraggableItemCondiction pos, ImColor color, Vector3 screen_pos, ImVec2 size, ESPPlayerData_t data, int transform, int outline);
	virtual void RenderBox(Vector3 pos, ImVec2 size, ESPPlayerData_t data, ImColor color, int style, bool outline);
};

inline C_PlayerESP* g_PlayerESP = new C_PlayerESP();