#include "cheat_cfg.hpp"
#include <sstream>
#include <unordered_map>
#include "../draggable_esp/items.h"

std::unordered_map <std::string, float[4]> colors;
C_ConfigManager* cfg_manager = new C_ConfigManager();

inline std::string add_line(std::string src)
{
	std::string result = src;
	for (int i = 0; i < result.length(); i++)
		if (result[i] == ' ')
			result[i] = '_';

	return result;
}

inline std::string erase_line(std::string src)
{
	std::string result = src;
	for (int i = 0; i < result.length(); i++)
		if (result[i] == '_')
			result[i] = ' ';

	return result;
}

void C_ConfigManager::setup()
{
	//cfg info
	{
		setup_item(&settings::my_username, "Genesis", ("info.my_username"));
	}

	//bools
	{
		setup_item(&settings::esp_enable, false, ("esp.esp_enable"));
		setup_item(&settings::esp_skeleton, false, ("esp.esp_skeleton"));
		setup_item(&settings::esp_visible, false, ("esp.esp_visible"));

		setup_item(&settings::skeleton_color_type, 0, ("skeleton_color_type"));
		setup_item(&settings::esp_box_type, 0, ("esp_box_type"));
		setup_item(&settings::box_outline, false, ("box_outline"));
		setup_item(&settings::esp_name_flag, 0, ("esp_name_flag"));
		setup_item(&settings::esp_name_transform, 0, ("esp_name_transform"));
		setup_item(&settings::esp_weapon_flag, 0, ("esp_weapon_flag"));
		setup_item(&settings::esp_weapon_transform, 0, ("esp_weapon_transform"));
		setup_item(&settings::health_color_type, 0, ("health_color_type"));

	}

	//font sizes
	{
		setup_item(&settings::username_fontsize, 14.f, ("esp.username_fontsize"));
		setup_item(&settings::weapon_fontsize, 14.f, ("esp.weapon_fontsize"));
		setup_item(&settings::flashed_fontsize, 14.f, ("esp.flashed_fontsize"));
		setup_item(&settings::defusing_fontsize, 14.f, ("esp.defusing_fontsize"));
		setup_item(&settings::scope_fontsize, 14.f, ("esp.scope_fontsize"));
	}

	//colors
	{
		for (int i = 0; i < settings::colors_list.size(); i++)
		{
			for (int j=0; j<4; j++)
				setup_item(&settings::colors_list[i].value[j], 1.f, add_line(settings::colors_list[i].name) + "_" + std::to_string(j));
		}
	}

	//aimbot
	{
		for (int i = 0; i < 36; i++)
		{
			setup_item(&settings::aim_settings_bot[i].override_global, false, "aim_override_"  + std::to_string(i));
			setup_item(&settings::aim_settings_bot[i].enable, false, "aim_enable_" + std::to_string(i));
			setup_item(&settings::aim_settings_bot[i].smooth_value, 0.5f,  "aim_smooth_" + std::to_string(i));
			//setup_item(&settings::aim_settings_bot[i].smooth, true, "aim_smooth_enable_" + std::to_string(i));
			setup_item(&settings::aim_settings_bot[i].aim_fov, 30.f, "aim_fov_" + std::to_string(i));
			setup_item(&settings::aim_settings_bot[i].bone_num, 0, "aim_bone_" + std::to_string(i));
		}

		setup_item(&settings::global_aim.override_global, false, "aim_override_global");
		setup_item(&settings::global_aim.enable, false, "aim_enable_global");
		setup_item(&settings::global_aim.smooth_value, 0.5f, "aim_smooth_global");
		//setup_item(&settings::global_aim.smooth, true, "aim_smooth_enable_global");
		setup_item(&settings::global_aim.aim_fov, 30.f, "aim_fov_global");
		setup_item(&settings::global_aim.bone_num, 0, "aim_bone_global");

		setup_item(&settings::render_aim_fov, false, "render_aim_fov");

		//setup_item(&box_pool_cond, 0, "box_pool_cond");
		//setup_item(&health_pool_cond, 0, "health_pool_cond");
		//setup_item(&name_pool_cond, 0, "name_pool_cond");
		//setup_item(&weapon_pool_cond, 0, "weapon_pool_cond");

	}

	//trigger
	{
		for (int i = 0; i < 36; i++)
		{
			setup_item(&settings::trigger_settings_bot[i].override_global, false, "trigger_override_" + std::to_string(i));
			setup_item(&settings::trigger_settings_bot[i].TriggerBot, false, "trigger_enable_" + std::to_string(i));
			setup_item(&settings::trigger_settings_bot[i].TeamCheck, false, "trigger_team_check_" + std::to_string(i));
		}

		setup_item(&settings::global_trigger.override_global, false, "trigger_override_global");
		setup_item(&settings::global_trigger.TriggerBot, false, "trigger_enable_global");
		setup_item(&settings::global_trigger.TeamCheck, false, "trigger_team_check_global");

	}

	//radar
	{
		setup_item(&settings::render_radar, false, "render_radar");
		setup_item(&settings::ShowRadarCrossLine, false, "ShowRadarCrossLine");
		setup_item(&settings::RadarRange, 150, "RadarRange");
		setup_item(&settings::Proportion, 2230, "Proportion");
		setup_item(&settings::radar_alpha, 0.5f, "radar_alpha");
	}

	//binds
	{
		setup_item(&settings::binds_list, false, "binds_list");
		setup_item(&settings::binds_alpha, 0.5f, "binds_alpha");
	}

	//crosshair
	{
		setup_item(&settings::draw_crosshair, false, "draw_crosshair");
		setup_item(&settings::outline, false, "outline");
		setup_item(&settings::line_width, 1, "line_width");
		setup_item(&settings::line_padding, 3, "line_padding");
		setup_item(&settings::line_length, 2, "line_length");
	}

	//keys
	{
	//	for (int i = 0; i < g_BindSystem->get_binds_list().size(); i++)
	//	{
	//		setup_item(&settings::line_width, 1, "line_width");
	//	}
		
	}
}

void C_ConfigManager::add_item(void* pointer, const char* name, const std::string& type)
{
	items.push_back(new C_ConfigItem(std::string(name), pointer, type));
}

void C_ConfigManager::setup_item(int* pointer, int value, const std::string& name)
{
	add_item(pointer, name.c_str(), ("int"));
	*pointer = value;
}

void C_ConfigManager::setup_item(bool* pointer, bool value, const std::string& name)
{
	add_item(pointer, name.c_str(), ("bool"));
	*pointer = value;
}

void C_ConfigManager::setup_item(float* pointer, float value, const std::string& name)
{
	add_item(pointer, name.c_str(), ("float"));
	*pointer = value;
}


void C_ConfigManager::setup_item(ImColor* pointer, ImColor value, const std::string& name)
{
	colors[name][0] = (float)value.Value.x / 255.f;
	colors[name][1] = (float)value.Value.y / 255.f;
	colors[name][2] = (float)value.Value.z / 255.f;
	colors[name][3] = (float)value.Value.w / 255.f;

	add_item(pointer, name.c_str(), ("Color"));
	*pointer = value;
}

void C_ConfigManager::setup_item(std::vector< int >* pointer, int size, const std::string& name)
{
	add_item(pointer, name.c_str(), ("vector<int>"));
	pointer->clear();

	for (int i = 0; i < size; i++)
		pointer->push_back(FALSE);
}

void C_ConfigManager::setup_item(std::vector< std::string >* pointer, const std::string& name)
{
	add_item(pointer, name.c_str(), ("vector<string>"));
}

void C_ConfigManager::setup_item(std::string* pointer, const std::string& value, const std::string& name)
{
	add_item(pointer, name.c_str(), ("string"));
	*pointer = value; 
}

void C_ConfigManager::save(std::string config)
{
	std::string folder, file;

	auto get_dir = [&folder, &file, &config]() -> void
	{
		folder = "C:\\genesis_cs2_external\\configs\\";
		file = "C:\\genesis_cs2_external\\configs\\" + config;

		CreateDirectoryA(folder.c_str(), nullptr);
	};

	get_dir();
	nlohmann::json allJson;

	for (auto it : items)
	{
		nlohmann::json j;

		j[("name")] = it->name;
		j[("type")] = it->type;

		if (!it->type.compare(("int")))
			j[("value")] = (int)*(int*)it->pointer; 
		else if (!it->type.compare(("float")))
			j[("value")] = (float)*(float*)it->pointer;
		else if (!it->type.compare(("bool")))
			j[("value")] = (bool)*(bool*)it->pointer;
		else if (!it->type.compare(("ImColor")))
		{
			auto c = *(ImColor*)(it->pointer);

			std::vector<float> a = { c.Value.x, c.Value.y, c.Value.z, c.Value.w };
			nlohmann::json ja;

			for (auto& i : a)
				ja.push_back(i);

			j[("value")] = ja.dump();
		}
		else if (!it->type.compare(("vector<int>")))
		{
			auto& ptr = *(std::vector<int>*)(it->pointer);
			nlohmann::json ja;

			for (auto& i : ptr)
				ja.push_back(i);

			j[("value")] = ja.dump();
		}
		else if (!it->type.compare(("vector<string>")))
		{
			auto& ptr = *(std::vector<std::string>*)(it->pointer);
			nlohmann::json ja;

			for (auto& i : ptr)
				ja.push_back(i);

			j[("value")] = ja.dump();
		}
		else if (!it->type.compare(("string")))
			j[("value")] = (std::string) * (std::string*)it->pointer;

		allJson.push_back(j);
	}

	/*auto get_type = [](menu_item_type type)
	{
		switch (type)
		{
		case CHECK_BOX:
			return "bool";
		case COMBO_BOX:
		case SLIDER_INT:
			return "int";
		case SLIDER_FLOAT:
			return "float";
		case COLOR_PICKER:
			return "Color";
		}
	};
	
	for (auto i = 0; i < c_lua::get().scripts.size(); ++i)
	{
		auto& script = c_lua::get().scripts.at(i);

		for (auto& item : c_lua::get().items.at(i))
		{
			if (item.second.type == NEXT_LINE)
				continue;

			nlohmann::json j;
			auto type = (std::string)get_type(item.second.type);

			j[("name")] = item.first;
			j[("type")] = type;

			if (!type.compare(("bool")))
				j[("value")] = item.second.check_box_value;
			else if (!type.compare(("int")))
				j[("value")] = item.second.type == COMBO_BOX ? item.second.combo_box_value : item.second.slider_int_value;
			else if (!type.compare(("float")))
				j[("value")] = item.second.slider_float_value;
			else if (!type.compare(("Color")))
			{
				std::vector <float> color =
				{
					item.second.color_picker_value.Value.x,
					item.second.color_picker_value.Value.y,
					item.second.color_picker_value.Value.z,
					item.second.color_picker_value.Value.w
				};

				nlohmann::json j_color;

				for (auto& i : color)
					j_color.push_back(i);

				j[("value")] = j_color.dump();
			}

			allJson.push_back(j);
		}
	}
	*/



	auto str = allJson.dump();
	std::vector<uint8_t> vec_enc = std::vector<uint8_t>(str.begin(), str.end());
	(vec_enc);
	auto str_enc = std::string(vec_enc.begin(), vec_enc.end());

	std::ofstream file_out(file, std::ios::binary | std::ios::trunc);
	if (file_out.good())
		file_out.write(str_enc.data(), str_enc.size());

	file_out.close();

	/*
	std::string data;

	Base64 base64;
	base64.encode(allJson.dump(), &data);

	std::ofstream ofs;
	ofs.open(file + '\0', std::ios::out | std::ios::trunc);

	ofs << std::setw(4) << data << std::endl;
	ofs.close();*/
}

std::vector<uint8_t> ReadAllBytes2(char const* filename)
{
	std::vector<uint8_t> result;

	std::ifstream ifs(filename, std::ios::binary | std::ios::ate);
	if (ifs)
	{
		std::ifstream::pos_type pos = ifs.tellg();
		result.resize(pos);

		ifs.seekg(0, std::ios::beg);
		ifs.read((char*)&result[0], pos);
	}

	return result;
}

void C_ConfigManager::load(std::string config, bool load_script_items)
{
	switch_to_save = true;
	static auto find_item = [](std::vector< C_ConfigItem* > items, std::string name) -> C_ConfigItem*
	{
		for (int i = 0; i < (int)items.size(); i++) //-V202
			if (!items[i]->name.compare(name))
				return items[i];

		return nullptr;
	};

	std::string folder, file;

	auto get_dir = [&folder, &file, &config]() ->void
	{
		static TCHAR path[MAX_PATH];

		folder = "C:\\genesis_cs2_external\\configs\\";
		file = "C:\\genesis_cs2_external\\configs\\" + config + ".gn";

		CreateDirectoryA(folder.c_str(), nullptr);
	};
	get_dir();

	std::vector<uint8_t> vec_enc;
	nlohmann::json allJson;

	vec_enc = ReadAllBytes2(file.c_str());

	std::vector<uint8_t> vec_dec = vec_enc;
	(vec_dec);
	std::stringstream stream_dec;
	auto str_dec = std::string(vec_dec.begin(), vec_dec.end());
	stream_dec << str_dec;
	stream_dec >> allJson;

	/*
	std::string data;

	std::ifstream ifs;
	ifs.open(file + '\0');

	ifs >> data;
	ifs.close();

	if (data.empty())
		return;

	Base64 base64;

	std::string decoded_data;
	base64.decode(data, &decoded_data);

	std::ofstream ofs;
	ofs.open(file + '\0', std::ios::out | std::ios::trunc);

	ofs << decoded_data;
	ofs.close();



	std::ifstream ifs_final;
	ifs_final.open(file + '\0');

	ifs_final >> allJson;
	ifs_final.close();

	base64.encode(allJson.dump(), &data);

	std::ofstream final_ofs;
	final_ofs.open(file + '\0', std::ios::out | std::ios::trunc);

	final_ofs << data;
	final_ofs.close();
	*/
	for (auto it = allJson.begin(); it != allJson.end(); ++it)
	{
		nlohmann::json j = *it;

		std::string name = j[("name")];
		std::string type = j[("type")];

		auto script_item = std::count_if(name.begin(), name.end(),
			[](char& c)
			{
				return c == '.';
			}
		) >= 2;

		/*
		if (load_script_items && script_item)
		{
			std::string script_name;
			auto first_point = false;

			for (auto& c : name)
			{
				if (c == '.')
				{
					if (first_point)
						break;
					else
						first_point = true;
				}

				script_name.push_back(c);
			}

			auto script_id = c_lua::get().get_script_id(script_name);

			if (script_id == -1)
				continue;

			for (auto& current_item : c_lua::get().items.at(script_id))
			{
				if (current_item.first == name)
				{
					if (!type.compare(("bool")))
					{
						current_item.second.type = CHECK_BOX;
						current_item.second.check_box_value = j[("value")].get<bool>();
					}
					else if (!type.compare(("int")))
					{
						if (current_item.second.type == COMBO_BOX)
							current_item.second.combo_box_value = j[("value")].get<int>();
						else
							current_item.second.slider_int_value = j[("value")].get<int>();
					}
					else if (!type.compare(("float")))
						current_item.second.slider_float_value = j[("value")].get<float>();
					else if (!type.compare(("Color")))
					{
						std::vector<int> a;
						nlohmann::json ja = nlohmann::json::parse(j[("value")].get<std::string>().c_str());

						for (nlohmann::json::iterator it = ja.begin(); it != ja.end(); ++it)
							a.push_back(*it);

						colors.erase(name);
						current_item.second.color_picker_value = ImColor(a[0], a[1], a[2], a[3]);
					}
				}
			}
		}
		else */if (!load_script_items && !script_item)
		{
			auto item = find_item(items, name);

			if (item)
			{
				if (!type.compare(("int")))
					*(int*)item->pointer = j[("value")].get<int>(); //-V206
				else if (!type.compare(("float")))
					*(float*)item->pointer = j[("value")].get<float>();
				else if (!type.compare(("bool")))
					*(bool*)item->pointer = j[("value")].get<bool>();
				else if (!type.compare(("ImColor")))
				{
					std::vector<int> a;
					nlohmann::json ja = nlohmann::json::parse(j[("value")].get<std::string>().c_str());

					for (nlohmann::json::iterator it = ja.begin(); it != ja.end(); ++it)
						a.push_back(*it);

					colors.erase(item->name);
					*(ImColor*)item->pointer = ImColor(a[0], a[1], a[2], a[3]);
				}
				else if (!type.compare(("vector<int>")))
				{
					auto ptr = static_cast<std::vector <int>*> (item->pointer);
					ptr->clear();

					nlohmann::json ja = nlohmann::json::parse(j[("value")].get<std::string>().c_str());

					for (nlohmann::json::iterator it = ja.begin(); it != ja.end(); ++it)
						ptr->push_back(*it);
				}
				else if (!type.compare(("vector<string>")))
				{
					auto ptr = static_cast<std::vector <std::string>*> (item->pointer);
					ptr->clear();

					nlohmann::json ja = nlohmann::json::parse(j[("value")].get<std::string>().c_str());

					for (nlohmann::json::iterator it = ja.begin(); it != ja.end(); ++it)
						ptr->push_back(*it);
				}
				else if (!type.compare(("string")))
					*(std::string*)item->pointer = j[("value")].get<std::string>();
			}
		}
		
		//std::string item_name = j[("box_pool")];
		//int pos = j[("type")];

	}

	//load_pool(allJson.dump());
}

config_information C_ConfigManager::get_cfg_info(std::string config)
{
	std::string title = config;
	std::string author = "";

	std::string folder, file;

	auto get_dir = [&folder, &file, &config]() ->void
	{
		static TCHAR path[MAX_PATH];

		folder = "C:\\genesis_cs2_external\\configs\\";
		file = "C:\\genesis_cs2_external\\configs\\" + config;

		CreateDirectoryA(folder.c_str(), nullptr);
	};

	get_dir();

	std::vector<uint8_t> vec_enc;
	nlohmann::json allJson;

	vec_enc = ReadAllBytes2(file.c_str());

	std::vector<uint8_t> vec_dec = vec_enc;
	(vec_dec);
	std::stringstream stream_dec;
	auto str_dec = std::string(vec_dec.begin(), vec_dec.end());
	stream_dec << str_dec;
	stream_dec >> allJson;

	for (auto it = allJson.begin(); it != allJson.end(); ++it)
	{
		nlohmann::json j = *it;

		std::string name = j[("name")];
		std::string type = j[("type")];

		if (name == "info.my_username")
		{
			author = j[("value")].get<std::string>();
			break;
		}
	}

	return config_information{ title, author };
}

void C_ConfigManager::remove(std::string config)
{
	std::string folder, file;

	auto get_dir = [&folder, &file, &config]() -> void
	{
		static TCHAR path[MAX_PATH];

		folder = "C:\\genesis_cs2_external\\configs\\";
		file = "C:\\genesis_cs2_external\\configs\\" + config;

		CreateDirectoryA(folder.c_str(), nullptr);
	};

	get_dir();

	std::string path = file + '\0';
	std::remove(path.c_str());
}

void C_ConfigManager::config_files()
{
	std::string folder;

	auto get_dir = [&folder]() -> void
	{
		folder = "C:\\genesis_cs2_external\\configs\\";
		CreateDirectoryA(folder.c_str(), nullptr);
	};

	get_dir();

	files.clear();
	cfg_info.clear();

	std::string path = folder + ("/*.gn");
	WIN32_FIND_DATA fd;

	size_t size = strlen(path.c_str());
	wchar_t* wArr = new wchar_t[size];
	for (size_t i = 0; i < size; ++i)
		wArr[i] = path[i];

	HANDLE hFind = FindFirstFile(wArr, &fd);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				std::wstring ws(fd.cFileName);
				std::string test(ws.begin(), ws.end());

				if (test.substr(test.find_last_of(".") + 1) == "gn")
				{
					std::string result_string = test.erase(test.length() - 3, 3);
					files.push_back(result_string);
					cfg_info.push_back(get_cfg_info(result_string + ".gn"));
				}
			}
				
		} while (FindNextFile(hFind, &fd));

		FindClose(hFind);
	}
}