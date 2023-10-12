#pragma once
#include "../libs/imgui/imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "../libs/imgui/imgui_internal.h"
#include "../libs/xorstr.hpp"
#include "../libs/json.hpp"
#include "../render/render.hpp"
#include "../configs/cheat_cfg.hpp"

#include <string>
#include <vector>
#include <algorithm>

enum DraggableItemCondiction : int {
	LEFT_COND = 0,
	RIGHT_COND = 1,
	TOP_COND = 2,
	BOT_COND = 3,
	CENTER_COND = 4,
	POOL_COND = 5,
	IN_MOVE_COND = 6,
};

struct MovableItems {
	std::string	ItemName;
	ImVec2 TemporaryPos = ImVec2();
	ImVec2 BasicPositions = ImVec2();
	ImVec2 WidgetSize = ImVec2();

	int Draw;
	int VectorCond = 4;

	int Type = 0; // 0 - text, 1 - bar, 2 - box

	MovableItems(
		int drw,
		std::string name = _S("Default"),
		int cond = 4,
		ImVec2 temp1 = ImVec2{ },
		ImVec2 temp2 = ImVec2{ },
		int type = 0)
	{
		Draw = drw;
		ItemName = name;
		VectorCond = cond;
		TemporaryPos = temp1;
		BasicPositions = temp2;
		Type = type;
	}
};

inline bool isMouseInAction = false;
inline float RecalculateAnimation = 0.f;
inline bool RecalculateAnimationFlag = false;

extern void RenderPreview(float x, float y);
extern std::vector<std::vector<MovableItems>> Pool;

struct pool_item
{
	std::string name;
	int array_num;
};

inline int box_pool_cond = 0;
inline int name_pool_cond = 0;
inline int health_pool_cond = 0;
inline int weapon_pool_cond = 0;

inline bool switch_to_save = false;

inline MovableItems& get_item_by_name(std::string name)
{
	for (int i = 0; i < Pool.size(); i++)
	{
		for (int j = 0; j < Pool[i].size(); j++)
		{
			if (Pool[i][j].ItemName == name)
				return Pool[i][j];
		}
	}
}



inline void load_pool(std::string response)
{
	nlohmann::json my_json = nlohmann::json::parse(response);
	get_item_by_name("Box").VectorCond = my_json["box_pool"].get<int>();
	get_item_by_name("Username").VectorCond = my_json["name_pool"].get<int>();
	get_item_by_name("Health").VectorCond = my_json["health_pool"].get<int>();
	get_item_by_name("Weapon").VectorCond = my_json["weapon_pool"].get<int>();
}


