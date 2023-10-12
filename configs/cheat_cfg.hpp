#pragma once
#include <windows.h>
#include "../libs/json.hpp"
#include "../libs/LuaAPI/menu_item.h"
#include "../libs/xorstr.hpp"
#include "../libs/imgui/imgui.h"
#include "../web_client/Helpers/Base64.hpp"
#include <iomanip>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <limits>
#include <array>

extern std::unordered_map <std::string, float[4]> colors;
using json = nlohmann::json;

struct config_information
{
	std::string title;
	std::string author;
};

class C_ConfigManager
{
public:
	class C_ConfigItem
	{
	public:
		Base64 base64;
		std::string name;
		void* pointer;
		std::string type;

		C_ConfigItem(std::string name, void* pointer, std::string type)  //-V818
		{
			this->name = name; //-V820
			this->pointer = pointer;
			this->type = type; //-V820
		}
	};

	void add_item(void* pointer, const char* name, const std::string& type);
	void setup_item(int*, int, const std::string&);
	void setup_item(bool*, bool, const std::string&);
	void setup_item(float*, float, const std::string&);
	void setup_item(ImColor*, ImColor, const std::string&);
	void setup_item(std::vector< int >*, int, const std::string&);
	void setup_item(std::vector< std::string >*, const std::string&);
	void setup_item(std::string*, const std::string&, const std::string&);

	std::vector <C_ConfigItem*> items;

	C_ConfigManager()
	{
		setup();
	};

	void setup();
	void save(std::string config);
	void load(std::string config, bool load_script_items);
	void remove(std::string config);
	config_information get_cfg_info(std::string config);

	std::vector<std::string> files;
	std::vector<config_information> cfg_info;

	void config_files();
};

extern C_ConfigManager* cfg_manager;

struct aim_settings
{
	bool override_global;
	bool enable;
	float smooth_value = 0.5f;
	bool smooth = true;
	bool rcs;
	float aim_fov = 30.f;
	int bone_num = 0; // head, neck, body, nearest
	float target_switch_delay = 1.f;
};

struct trigger_settings
{
	bool override_global = false;
	bool TriggerBot = true;
	int TriggerHotKey = 0;
	bool TeamCheck = true;
};

inline std::vector<std::string> color_names =
{ 
	_S("Aim line"),
	_S("Aim FOV"),
	_S("Skeleton"),
	_S("Crosshair"),
	_S("Radar crossline"),
	_S("Username"),
	_S("Weapon"),
	_S("Health Bar"),
	_S("Health Gradient Top"),
	_S("Health Gradient Bottom"),
	_S("Box Visisble"),
	_S("Box Invisisble"),
};

struct set_color
{
	std::string name;
	float value[4];
};
namespace settings
{
	inline std::vector<set_color> colors_list =
	{ 
		{_S("Aim line"), {1.f, 1.f, 1.f, 1.f}},
		{_S("Aim FOV"), {1.f, 1.f, 1.f, 1.f}},
		{_S("Skeleton"), {1.f, 1.f, 1.f, 1.f}},
		{_S("Crosshair"), {1.f, 1.f, 1.f, 1.f}},
		{_S("Radar crossline"), {1.f, 1.f, 1.f, 1.f}},
		{_S("Username"), {1.f, 1.f, 1.f, 1.f}},
		{_S("Weapon"), {1.f, 1.f, 1.f, 1.f}},
		{_S("Health Bar"), {1.f, 1.f, 1.f, 1.f}},
		{_S("Health Gradient Top"), {1.f, 1.f, 1.f, 1.f}},
		{_S("Health Gradient Bottom"), {1.f, 1.f, 1.f, 1.f}},
		{_S("Box Visible"), {1.f, 1.f, 1.f, 1.f}},
		{_S("Box Invisible"), {1.f, 1.f, 1.f, 1.f}},
	};

	inline float* get_color(std::string name)
	{
		for (auto item : colors_list)
			if (item.name == name)
				return item.value;
	}

	inline std::string my_username;
	inline bool use_anti_obs;

	//aimbot
	inline aim_settings aim_settings_bot[36];
	inline aim_settings global_aim;
	inline bool render_aim_fov = false;
	inline bool render_aim_line = false;

	//trigger
	inline trigger_settings trigger_settings_bot[36];
	inline trigger_settings global_trigger;

	//esp
	inline bool esp_visible;
	inline bool esp_enable;

	inline bool esp_skeleton;
	inline int skeleton_color_type = 0; // 0 - common 1 - health based

	inline int esp_box_type = 0; //0 - box 1 - corners
	inline bool box_outline = false;

	inline int esp_name_flag = 1; //0 - text, 1 - shadow 2 - outline
	inline int esp_name_transform = 0; // 0 - common 1 - uppercase 2 - lowercase

	inline int esp_weapon_flag = 1; //0 - text, 1 - shadow 2 - outline
	inline int esp_weapon_transform = 0; // 0 - common 1 - uppercase 2 - lowercase

	inline int health_style = 0; // 0 - common 1 - from bottom to upper 2 - from upper to bottom
	inline int health_color_type = 0; // 0 - color 1 - gradient 2 - based on health

	//crosshair
	inline bool draw_crosshair = false;
	inline bool outline = false;
	inline float line_width = 1.f;
	inline float line_length = 5.f;
	inline float line_padding = 1.f;

	//radar
	inline bool render_radar;
	inline float RadarRange = 150;
	inline bool ShowRadarCrossLine;
	inline float Proportion = 2230;
	inline float radar_alpha = 1.f;

	//username
	inline int username_fontsize;

	//weapon
	inline int weapon_fontsize;
	inline bool ammo_amount;

	//defusing
	inline int defusing_fontsize;

	//flashed
	inline int flashed_fontsize;

	//scope
	inline int scope_fontsize;

	inline bool binds_list = false;
	inline float binds_alpha = 1.f;
	inline std::vector<std::string> files;
}
