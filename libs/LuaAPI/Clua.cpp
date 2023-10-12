// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "HTTPRequest.hpp"
#include "Clua.h"	
#include <ShlObj_core.h>
#include <Windows.h>
#include <any>
#include "../../menu.h"
#include "../xorstr.hpp"
#include "../../logs/logs.h"

void lua_panic(sol::optional <std::string> message)
{
	if (!message)
		return;

	auto log = _S("Lua error: ") + message.value_or("unknown");
	logs_system::push_log(log, false);
}

std::string get_current_script(sol::this_state s)
{
	sol::state_view lua_state(s);
	sol::table rs = lua_state["debug"]["getinfo"](2, ("S"));
	std::string source = rs["source"];
	std::string filename = std::filesystem::path(source.substr(1)).filename().string();

	return filename;
}

int get_current_script_id(sol::this_state s)
{
	return c_lua::get().get_script_id(get_current_script(s));
}

namespace ns_client
{
	void add_callback(sol::this_state s, std::string eventname, sol::protected_function func)
	{
		if (eventname != _S("on_paint") && eventname != _S("on_createmove") && eventname != _S("on_shot") && eventname != _S("pre_resolve"))
		{
			logs_system::push_log(_S("Lua error: invalid callback \"") + eventname + '\"', false);
			c_lua::get().unload_script(get_current_script_id(s));
			return;
		}

		if (c_lua::get().loaded.at(get_current_script_id(s)))
			c_lua::get().hooks.registerHook(eventname, get_current_script_id(s), func);
	}

	void load_script(std::string name)
	{
		c_lua::get().refresh_scripts();
		c_lua::get().load_script(c_lua::get().get_script_id(name));
	}

	void unload_script(std::string name)
	{
		c_lua::get().refresh_scripts();
		c_lua::get().unload_script(c_lua::get().get_script_id(name));
	}

	void log(std::string text)
	{
		//L::Print("LUA DEBUG: " + text);
		logs_system::push_log(text, false);
	}
}

std::vector <std::pair <std::string, menu_item>>::iterator find_item(std::vector <std::pair <std::string, menu_item>>& items, const std::string& name)
{
	for (auto it = items.begin(); it != items.end(); ++it)
		if (it->first == name)
			return it;

	return items.end();
}

menu_item find_item(std::vector <std::vector <std::pair <std::string, menu_item>>>& scripts, const std::string& name)
{
	for (auto& script : scripts)
	{
		for (auto& item : script)
		{
			std::string item_name;

			auto first_point = false;
			auto second_point = false;

			for (auto& c : item.first)
			{
				if (c == '.')
				{
					if (first_point)
					{
						second_point = true;
						continue;
					}
					else
					{
						first_point = true;
						continue;
					}
				}

				if (!second_point)
					continue;

				item_name.push_back(c);
			}

			if (item_name == name)
				return item.second;
		}
	}

	return menu_item();
}

namespace ns_menu
{
	bool get_visible()
	{
		return menuShow;
	}

	void set_visible(bool visible)
	{
		menuShow = visible;
	}

	auto next_line_counter = 0;

	void next_line(sol::this_state s)
	{
		c_lua::get().items.at(get_current_script_id(s)).emplace_back(std::make_pair(_S("next_line_") + std::to_string(next_line_counter), menu_item()));
		++next_line_counter;
	}

	void add_check_box(sol::this_state s, const std::string& name)
	{
		auto script = get_current_script(s);
		auto script_id = c_lua::get().get_script_id(script);

		auto& items = c_lua::get().items.at(script_id);
		auto full_name = script + '.' + name;

		if (find_item(items, full_name) != items.end())
			return;

		items.emplace_back(std::make_pair(full_name, menu_item(false)));
	}

	void add_combo_box(sol::this_state s, std::string name, std::vector <std::string> labels) //-V813
	{
		if (labels.empty())
			return;

		auto script = get_current_script(s);
		auto script_id = c_lua::get().get_script_id(script);

		auto& items = c_lua::get().items.at(script_id);
		auto full_name = script + '.' + name;

		if (find_item(items, full_name) != items.end())
			return;

		items.emplace_back(std::make_pair(full_name, menu_item(labels, 0)));
	}

	void add_slider_int(sol::this_state s, const std::string& name, int min, int max)
	{
		auto script = get_current_script(s);
		auto script_id = c_lua::get().get_script_id(script);

		auto& items = c_lua::get().items.at(script_id);
		auto full_name = script + '.' + name;

		if (find_item(items, full_name) != items.end())
			return;

		items.emplace_back(std::make_pair(full_name, menu_item(min, max, min)));
	}

	void add_slider_float(sol::this_state s, const std::string& name, float min, float max)
	{
		auto script = get_current_script(s);
		auto script_id = c_lua::get().get_script_id(script);

		auto& items = c_lua::get().items.at(script_id);
		auto full_name = script + '.' + name;

		if (find_item(items, full_name) != items.end())
			return;

		items.emplace_back(std::make_pair(full_name, menu_item(min, max, min)));
	}

	void add_color_picker(sol::this_state s, const std::string& name)
	{
		auto script = get_current_script(s);
		auto script_id = c_lua::get().get_script_id(script);

		auto& items = c_lua::get().items.at(script_id);
		auto full_name = script + '.' + name;

		if (find_item(items, full_name) != items.end())
			return;

		items.emplace_back(std::make_pair(full_name, menu_item(ImColor(255,255,255))));
	}

	std::unordered_map <std::string, bool> first_update;
	std::unordered_map <std::string, menu_item> stored_values;
	std::unordered_map <std::string, void*> config_items;

	bool find_config_item(std::string name, std::string type)
	{

		if (config_items.find(name) == config_items.end())
		{
			auto found = false;

			for (auto item : cfg_manager->items)
			{
				if (item->name == name)
				{
					if (item->type != type)
					{
						logs_system::push_log(_S("Lua error: invalid config item type, must be ") + type);
						return false;
					}

					found = true;
					config_items[name] = item->pointer;
					break;
				}
			}

			if (!found)
			{
				logs_system::push_log(_S("Lua error: cannot find menu variable (find_config_item) \"") + name + '\"');
				return false;
			}
		}

		return true;
	}

	bool get_bool(std::string name)
	{
		if (first_update.find(name) == first_update.end())
			first_update[name] = false;

		if (!menuShow && first_update[name])
		{
			if (stored_values.find(name) != stored_values.end())
				return stored_values[name].check_box_value;
			else if (config_items.find(name) != config_items.end())
				return *(bool*)config_items[name];
			else
				return false;
		}

		auto& it = find_item(c_lua::get().items, name);

		if (it.type == NEXT_LINE)
		{
			if (find_config_item(name, _S("bool")))
				return *(bool*)config_items[name];

			logs_system::push_log(_S("Lua error: cannot find menu variable (bool) \"") + name + '\"');
			return false;
		}

		first_update[name] = true;
		stored_values[name] = it;

		return it.check_box_value;
	}

	int get_int(std::string name)
	{
		if (first_update.find(name) == first_update.end())
			first_update[name] = false;

		if (!menuShow && first_update[name])
		{
			if (stored_values.find(name) != stored_values.end())
				return stored_values[name].type == COMBO_BOX ? stored_values[name].combo_box_value : stored_values[name].slider_int_value;
			else if (config_items.find(name) != config_items.end())
				return *(int*)config_items[name]; //-V206
			else
				return 0;
		}

		auto& it = find_item(c_lua::get().items, name);

		if (it.type == NEXT_LINE)
		{
			if (find_config_item(name, _S("int")))
				return *(int*)config_items[name]; //-V206

			logs_system::push_log(_S("Lua error: cannot find menu variable (int) \"") + name + '\"');
			return 0;
		}

		first_update[name] = true;
		stored_values[name] = it;

		return it.type == COMBO_BOX ? it.combo_box_value : it.slider_int_value;
	}

	float get_float(std::string name)
	{
		if (first_update.find(name) == first_update.end())
			first_update[name] = false;

		if (!menuShow && first_update[name])
		{
			if (stored_values.find(name) != stored_values.end())
				return stored_values[name].slider_float_value;
			else if (config_items.find(name) != config_items.end())
				return *(float*)config_items[name];
			else
				return 0.0f;
		}

		auto& it = find_item(c_lua::get().items, name);

		if (it.type == NEXT_LINE)
		{
			if (find_config_item(name, _S("float")))
				return *(float*)config_items[name];

			logs_system::push_log(_S("Lua error: cannot find menu variable (float) \"") + name + '\"');
			return 0.0f;
		}

		first_update[name] = true;
		stored_values[name] = it;

		return it.slider_float_value;
	}

	ImColor get_color(std::string name)
	{
		if (first_update.find(name) == first_update.end())
			first_update[name] = false;

		if (!menuShow && first_update[name])
		{
			if (stored_values.find(name) != stored_values.end())
				return stored_values[name].color_picker_value;
			else if (config_items.find(name) != config_items.end())
				return *(ImColor*)config_items[name];
			else
				return ImColor(255,255,255);
		}

		auto& it = find_item(c_lua::get().items, name);

		if (it.type == NEXT_LINE)
		{
			if (find_config_item(name, _S("Color")))
				return *(ImColor*)config_items[name];

			logs_system::push_log(_S("Lua error: cannot find menu variable (clr) \"") + name + '\"');
			return ImColor(255,255,255);
		}

		first_update[name] = true;
		stored_values[name] = it;

		return it.color_picker_value;
	}

	void set_bool(std::string name, bool value)
	{
		if (!find_config_item(name, _S("bool")))
			return;

		*(bool*)config_items[name] = value;
	}

	void set_int(std::string name, int value)
	{
		if (!find_config_item(name, _S("int")))
			return;

		*(int*)config_items[name] = value; //-V206
	}

	void set_float(std::string name, float value)
	{
		if (!find_config_item(name, _S("float")))
			return;

		*(float*)config_items[name] = value;
	}

	void set_color(std::string name, ImColor value)
	{
		if (!find_config_item(name, _S("Color")))
			return;

		*(ImColor*)config_items[name] = value;
	}
}

namespace ns_globals
{
	int get_framerate()
	{
		return ImGui::GetIO().DeltaTime;
	}

	std::string get_username()
	{
		return username;
	}

	float get_realtime()
	{
		return m_globals()->m_realtime;
	}

	int get_maxclients()
	{
		return m_globals()->m_maxclients;
	}
}

namespace ns_engine
{
	static int width, height;

	int get_screen_width()
	{
		m_engine()->GetScreenSize(width, height);
		return width;
	}

	int get_screen_height()
	{
		m_engine()->GetScreenSize(width, height);
		return height;
	}

	player_info_t get_player_info(int i)
	{
		player_info_t player_info;
		m_engine()->GetPlayerInfo(i, &player_info);

		return player_info;
	}

	int get_player_for_user_id(int i)
	{
		return m_engine()->GetPlayerForUserID(i);
	}

	int get_local_player_index()
	{
		return m_engine()->GetLocalPlayer();
	}

	Vector get_view_angles()
	{
		Vector view_angles;
		m_engine()->GetViewAngles(view_angles);

		return view_angles;
	}

	void set_view_angles(Vector view_angles)
	{
		math::normalize_angles(view_angles);
		m_engine()->SetViewAngles(view_angles);
	}

	bool is_in_game()
	{
		return m_engine()->IsInGame();
	}

	bool is_connected()
	{
		return m_engine()->IsConnected();
	}

	std::string get_level_name()
	{
		return m_engine()->GetLevelName();
	}

	std::string get_level_name_short()
	{
		return m_engine()->GetLevelNameShort();
	}

	std::string get_map_group_name()
	{
		return m_engine()->GetMapGroupName();
	}

	bool is_playing_demo()
	{
		return m_engine()->IsPlayingDemo();
	}

	bool is_recording_demo()
	{
		return m_engine()->IsRecordingDemo();
	}

	bool is_paused()
	{
		return m_engine()->IsPaused();
	}

	bool is_taking_screenshot()
	{
		return m_engine()->IsTakingScreenshot();
	}

	bool is_hltv()
	{
		return m_engine()->IsHLTV();
	}
}

namespace ns_render
{
	Vector world_to_screen(const Vector& world)
	{
		Vector screen;

		if (!math::world_to_screen(world, screen))
			return ZERO;

		return screen;
	}

	//render test
	//int get_text_width(ImFont* font, const std::string& text)
	//{
	//	return font->CalcTextSizeA(font->FontSize, FLT_MAX, 0.0f, text.c_str()).x;
	//	//return render::get().text_width(font, text.c_str());
	//}

	//ImFont* create_font(const std::string& name, float size)
	//{
	//	auto& io = ImGui::GetIO();
	//	auto font = io.Fonts->AddFontFromFileTTF(name.c_str(), size);

	//	return font;
	//}

	//void draw_text(ImFont* font, float x, float y, Color color, bool center_x, bool center_y, bool outline, bool shadow, const std::string& text)
	//{
	//	renderer::get().text(font, x, y, color, center_x, center_y, outline, shadow, text.c_str());
	//	//render::get().text(font, (int)x, (int)y, color, HFONT_CENTERED_NONE, text.c_str());
	//}

	int get_text_width(vgui::HFont font, const std::string& text)
	{
		return render::get().text_width(font, text.c_str());
	};

	vgui::HFont create_font(const std::string& name, float size, float weight, std::optional <bool> antialias, std::optional <bool> dropshadow, std::optional <bool> outline)
	{
		DWORD flags = FONTFLAG_NONE;

		if (antialias.value_or(false))
			flags |= FONTFLAG_ANTIALIAS;

		if (dropshadow.value_or(false))
			flags |= FONTFLAG_DROPSHADOW;

		if (outline.value_or(false))
			flags |= FONTFLAG_OUTLINE;

		g_ctx.last_font_name = name;

		auto font = m_surface()->FontCreate();
		m_surface()->SetFontGlyphSet(font, name.c_str(), (int)size, (int)weight, 0, 0, flags);

		return font;
	}

	void draw_text(vgui::HFont font, float x, float y, Color color, const std::string& text)
	{
		render::get().text(font, (int)x, (int)y, color, HFONT_CENTERED_NONE, text.c_str());
	}

	void draw_text_centered(vgui::HFont font, float x, float y, Color color, bool centered_x, bool centered_y, const std::string& text)
	{
		DWORD centered_flags = HFONT_CENTERED_NONE;

		if (centered_x)
		{
			centered_flags &= ~HFONT_CENTERED_NONE; //-V753
			centered_flags |= HFONT_CENTERED_X;
		}

		if (centered_y)
		{
			centered_flags &= ~HFONT_CENTERED_NONE;
			centered_flags |= HFONT_CENTERED_Y;
		}

		render::get().text(font, (int)x, (int)y, color, centered_flags, text.c_str());
	}

	void draw_line(float x, float y, float x2, float y2, Color color)
	{
		render::get().line((int)x, (int)y, (int)x2, (int)y2, color);
	}

	void draw_rect(float x, float y, float w, float h, Color color)
	{
		render::get().rect((int)x, (int)y, (int)w, (int)h, color);
	}

	void draw_rect_filled(float x, float y, float w, float h, Color color)
	{
		//renderer::get().rect_filled(x, y, w, h, color, rounding, 0);
		render::get().rect_filled((int)x, (int)y, (int)w, (int)h, color);
	}

	void draw_rect_filled_gradient(float x, float y, float w, float h, Color color, Color color2, int gradient_type)
	{
		gradient_type = math::clamp(gradient_type, 0, 1);
		render::get().gradient((int)x, (int)y, (int)w, (int)h, color, color2, (GradientType)gradient_type);
	}

	void draw_circle(float x, float y, float points, float radius, Color color)
	{
		render::get().circle((int)x, (int)y, (int)points, (int)radius, color);
	}

	void draw_circle_filled(float x, float y, float points, float radius, Color color)
	{
		render::get().circle_filled((int)x, (int)y, (int)points, (int)radius, color);
	}

	void draw_triangle(float x, float y, float x2, float y2, float x3, float y3, Color color)
	{
		render::get().triangle(Vector2D(x, y), Vector2D(x2, y2), Vector2D(x3, y3), color);
	}
}
namespace ns_indicator
{
	void add_indicator(std::string& text, Color color)
	{
		if (!g_ctx.available())
			return;
		if (!g_ctx.local()->is_alive()) //-V807
			return;


		if (otheresp::get().m_indicators.empty())
		{
			otheresp::get().m_indicators.insert(otheresp::get().m_indicators.begin(), m_indicator(text, color));
		}
		else {
			otheresp::get().m_indicators.insert(otheresp::get().m_indicators.begin(), m_indicator(text, color));
		}


	}
	void add_indicator_with_pos(std::string& text, Color color, int pos)
	{
		if (!g_ctx.available())
			return;
		if (!g_ctx.local()->is_alive()) //-V807
			return;
		if (otheresp::get().m_indicators.empty())
		{
			otheresp::get().m_indicators.insert(otheresp::get().m_indicators.begin(), m_indicator(text, color));
		}
		else
		{
			auto iter1 = otheresp::get().m_indicators.cbegin();
			otheresp::get().m_indicators.insert(iter1 + (pos - 1), m_indicator(text, color));
		}
	}
}

namespace ns_console
{
	bool get_bool(const std::string& convar_name)
	{
		if (convars.find(convar_name) == convars.end())
		{
			convars[convar_name] = m_cvar()->FindVar(convar_name.c_str());

			if (!convars[convar_name])
			{
				logs_system::push_log(_S("Lua error: cannot find ConVar \"") + convar_name + '\"', false);
				return false;
			}
		}

		if (!convars[convar_name])
			return false;

		return convars[convar_name]->GetBool();
	}

	int get_int(const std::string& convar_name)
	{
		if (convars.find(convar_name) == convars.end())
		{
			convars[convar_name] = m_cvar()->FindVar(convar_name.c_str());

			if (!convars[convar_name])
			{
				logs_system::push_log(_S("Lua error: cannot find ConVar \"") + convar_name + '\"', false);
				return 0;
			}
		}

		if (!convars[convar_name])
			return 0;

		return convars[convar_name]->GetInt();
	}

	float get_float(const std::string& convar_name)
	{
		if (convars.find(convar_name) == convars.end())
		{
			convars[convar_name] = m_cvar()->FindVar(convar_name.c_str());

			if (!convars[convar_name])
			{
				logs_system::push_log(_S("Lua error: cannot find ConVar \"") + convar_name + '\"', false);
				return 0.0f;
			}
		}

		if (!convars[convar_name])
			return 0.0f;

		return convars[convar_name]->GetFloat();
	}

	std::string get_string(const std::string& convar_name)
	{
		if (convars.find(convar_name) == convars.end())
		{
			convars[convar_name] = m_cvar()->FindVar(convar_name.c_str());

			if (!convars[convar_name])
			{
				logs_system::push_log(_S("Lua error: cannot find ConVar \"") + convar_name + '\"', false);
				return _S("");
			}
		}

		if (!convars[convar_name])
			return "";

		return convars[convar_name]->GetString();
	}

	void set_bool(const std::string& convar_name, bool value)
	{
		if (convars.find(convar_name) == convars.end())
		{
			convars[convar_name] = m_cvar()->FindVar(convar_name.c_str());

			if (!convars[convar_name])
			{
				logs_system::push_log(_S("Lua error: cannot find ConVar \"") + convar_name + '\"', false);
				return;
			}
		}

		if (!convars[convar_name])
			return;

		if (convars[convar_name]->GetBool() != value)
			convars[convar_name]->SetValue(value);
	}

	void set_int(const std::string& convar_name, int value)
	{
		if (convars.find(convar_name) == convars.end())
		{
			convars[convar_name] = m_cvar()->FindVar(convar_name.c_str());

			if (!convars[convar_name])
			{
				logs_system::push_log(_S("Lua error: cannot find ConVar \"") + convar_name + '\"', false);
				return;
			}
		}

		if (!convars[convar_name])
			return;

		if (convars[convar_name]->GetInt() != value)
			convars[convar_name]->SetValue(value);
	}

	void set_float(const std::string& convar_name, float value)
	{
		if (convars.find(convar_name) == convars.end())
		{
			convars[convar_name] = m_cvar()->FindVar(convar_name.c_str());

			if (!convars[convar_name])
			{
				logs_system::push_log(_S("Lua error: cannot find ConVar \"") + convar_name + '\"', false);
				return;
			}
		}

		if (!convars[convar_name])
			return;

		if (convars[convar_name]->GetFloat() != value) //-V550
			convars[convar_name]->SetValue(value);
	}

	void set_string(const std::string& convar_name, const std::string& value)
	{
		if (convars.find(convar_name) == convars.end())
		{
			convars[convar_name] = m_cvar()->FindVar(convar_name.c_str());

			if (!convars[convar_name])
			{
				logs_system::push_log(_S("Lua error: cannot find ConVar \"") + convar_name + '\"', false);
				return;
			}
		}

		if (!convars[convar_name])
			return;

		if (convars[convar_name]->GetString() != value)
			convars[convar_name]->SetValue(value.c_str());
	}
}

namespace ns_events
{
	void register_event(sol::this_state s, std::string event_name, sol::protected_function function)
	{
		if (std::find(g_ctx.globals.events.begin(), g_ctx.globals.events.end(), event_name) == g_ctx.globals.events.end())
		{
			m_eventmanager()->AddListener(&hooks::hooked_events, event_name.c_str(), false);
			g_ctx.globals.events.emplace_back(event_name);
		}

		c_lua::get().events[get_current_script_id(s)][event_name] = function;
	}
}

namespace ns_utils
{
	uint64_t find_signature(const std::string& szModule, const std::string& szSignature)
	{
		return util::FindSignature(szModule.c_str(), szSignature.c_str());
	}
}

namespace ns_http 
{
	std::string get(sol::this_state s, std::string& link)
	{
		try
		{
			http::Request request(link);

			const http::Response response = request.send(_S("GET"));
			return std::string(response.body.begin(), response.body.end());
		}
		catch (const std::exception& e)
		{
			logs_system::push_log(_S("Request failed, error: ") + std::string(e.what()), false);
			return "";
		}
	}

	std::string post(sol::this_state s, std::string& link, std::string& params)
	{
		try
		{
			http::Request request(link);
			const http::Response response = request.send(_S("POST"), params, { _S("Content-Type: application/x-www-form-urlencoded") });
			return std::string(response.body.begin(), response.body.end());
		}
		catch (const std::exception& e)
		{
			logs_system::push_log(_S("Request failed, error: ") + std::string(e.what()), false);
			return "";
		}
	}
}


namespace ns_file 
{
	void append(sol::this_state s, std::string& path, std::string& data)
	{
		std::ofstream out(path, std::ios::app | std::ios::binary);

		if (out.is_open())
			out << data;
		else
			logs_system::push_log(_S("Can't append to file: ") + path, false);

		out.close();
	}
	void write(sol::this_state s, std::string& path, std::string& data)
	{
		std::ofstream out(path, std::ios::binary);

		if (out.is_open())
			out << data;

		else
			logs_system::push_log(_S("Can't write to file: ") + path, false);

		out.close();
	}
	std::string read(sol::this_state s, std::string& path)
	{
		std::ifstream file(path, std::ios::binary);

		if (file.is_open())
		{
			std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
			file.close();
			return content;
		}
		else
		{
			logs_system::push_log(_S("Can't read file: ") + path, false);
			file.close();
			return "";
		}
	}
}

namespace functions
{
#define luaL_optint(L,n,d)	((int)luaL_optinteger(L, (n), (d)))
	static int collectgarbage(lua_State* L) {
		static const char* const opts[] = { "stop", "restart", "collect",
		  "count", "step", "setpause", "setstepmul", NULL };
		static const int optsnum[] = { LUA_GCSTOP, LUA_GCRESTART, LUA_GCCOLLECT,
		  LUA_GCCOUNT, LUA_GCSTEP, LUA_GCSETPAUSE, LUA_GCSETSTEPMUL };
		int o = luaL_checkoption(L, 1, "collect", opts);
		int ex = luaL_optint(L, 2, 0);
		int res = lua_gc(L, optsnum[o], ex);
		switch (optsnum[o]) {
		case LUA_GCCOUNT: {
			int b = lua_gc(L, LUA_GCCOUNTB, 0);
			lua_pushnumber(L, res + ((lua_Number)b / 1024));
			return 1;
		}
		case LUA_GCSTEP: {
			lua_pushboolean(L, res);
			return 1;
		}
		default: {
			lua_pushnumber(L, res);
			return 1;
		}
		}
	}

	static int getmetatable(lua_State* L) {
		luaL_checkany(L, 1);
		if (!lua_getmetatable(L, 1)) {
			lua_pushnil(L);
			return 1;  /* no metatable */
		}
		luaL_getmetafield(L, 1, "__metatable");
		return 1;  /* returns either __metatable field (if present) or metatable */
	}

	static int setmetatable(lua_State* L) {
		int t = lua_type(L, 2);
		luaL_checktype(L, 1, LUA_TTABLE);
		luaL_argcheck(L, t == LUA_TNIL || t == LUA_TTABLE, 2,
			"nil or table expected");
		if (luaL_getmetafield(L, 1, "__metatable"))
			luaL_error(L, "cannot change a protected metatable");
		lua_settop(L, 2);
		lua_setmetatable(L, 1);
		return 1;
	}

	static int xpcall(lua_State* L) {
		int status;
		luaL_checkany(L, 2);
		lua_settop(L, 2);
		lua_insert(L, 1);  /* put error function under function to be called */
		status = lua_pcall(L, 0, LUA_MULTRET, 1);
		lua_pushboolean(L, (status == 0));
		lua_replace(L, 1);
		return lua_gettop(L);  /* return status + all results */
	}

	static int pcall(lua_State* L) {
		int status;
		luaL_checkany(L, 1);
		lua_State* Hold = lua_newthread(L);
		luaL_ref(L, LUA_REGISTRYINDEX);//Ref it
		lua_xmove(L, Hold, lua_gettop(L));
		status = lua_pcall(Hold, lua_gettop(Hold) - 1, LUA_MULTRET, 0);
		lua_pushboolean(L, (status == 0));
		lua_insert(L, 1);
		lua_xmove(Hold, L, lua_gettop(Hold));
		return lua_gettop(L);  /* return status + all results */
	}

	static int load_aux(lua_State* L, int status) {
		if (status == 0)  /* OK? */
			return 1;
		else {
			lua_pushnil(L);
			lua_insert(L, -2);  /* put before error message */
			return 2;  /* return nil plus error message */
		}
	}

	static int luaB_loadfile(lua_State* L) {
		return luaL_error(L, "Function Disabled");
		const char* fname = luaL_optstring(L, 1, NULL);
		return load_aux(L, luaL_loadfile(L, fname));
	}

	static int loadstring(lua_State* L) {
		size_t l;
		const char* s = luaL_checklstring(L, 1, &l);
		const char* chunkname = luaL_optstring(L, 2, s);
		return load_aux(L, luaL_loadbuffer(L, s, l, chunkname));
	}

	static int loadfile(lua_State* L) {
		return luaL_error(L, "Function Disabled");
		const char* fname = luaL_optstring(L, 1, NULL);
		return load_aux(L, luaL_loadfile(L, fname));
	}

	static int dofile(lua_State* L) {
		const char* fname = luaL_optstring(L, 1, NULL);
		int n = lua_gettop(L);
		if (luaL_loadfile(L, fname) != 0) lua_error(L);
		lua_call(L, 0, LUA_MULTRET);
		return lua_gettop(L) - n;
	}
}

sol::state lua;
void c_lua::initialize()
{
	//L::Print("Made it");
	lua = sol::state(sol::c_call<decltype(&lua_panic), &lua_panic>);
	lua.open_libraries(sol::lib::base, sol::lib::string, sol::lib::math, sol::lib::table, sol::lib::debug, sol::lib::package, sol::lib::ffi, sol::lib::jit, sol::lib::utf8);

	lua[_S("collectgarbage")] = &functions::collectgarbage;
	lua[_S("dofile")] = &functions::dofile;
	lua[_S("load")] = sol::nil;
	lua[_S("loadfile")] = &functions::loadfile;
	lua[_S("pcall")] = &functions::pcall;
	lua[_S("print")] = &ns_client::log;
	lua[_S("xpcall")] = &functions::xpcall;
	lua[_S("getmetatable")] = &functions::getmetatable;
	lua[_S("setmetatable")] = &functions::setmetatable;

	lua[_S("__nil_callback")] = []() {};

	lua.new_enum(_S("key_binds"),
		_S("legit_automatic_fire"), 0,
		_S("legit_enable"), 1,
		_S("double_tap"), 2,
		_S("safe_points"), 3,
		_S("damage_override"), 4,
		_S("hide_shots"), 12,
		_S("manual_back"), 13,
		_S("manual_left"), 14,
		_S("manual_right"), 15,
		_S("flip_desync"), 16,
		_S("thirdperson"), 17,
		_S("automatic_peek"), 18,
		_S("edge_jump"), 19,
		_S("fakeduck"), 20,
		_S("slowwalk"), 21,
		_S("body_aim"), 22,
		_S("roll_resolver"), 23
	);

	lua.new_enum(_S("key_bind_mode"),
		_S("hold"), 0,
		_S("toggle"), 1,
		_S("always"), 2
	);

	lua.new_usertype<entity_t>(_S("entity"), // new
		(std::string)_S("get_prop_int"), &entity_t::GetPropInt,
		(std::string)_S("get_prop_float"), &entity_t::GetPropFloat,
		(std::string)_S("get_prop_bool"), &entity_t::GetPropBool,
		(std::string)_S("get_prop_string"), &entity_t::GetPropString,
		(std::string)_S("set_prop_int"), &entity_t::SetPropInt,
		(std::string)_S("set_prop_float"), &entity_t::SetPropFloat,
		(std::string)_S("set_prop_bool"), &entity_t::SetPropBool,
		(std::string)_S("set_prop_string"), &entity_t::SetPropString
		);

	lua.new_usertype <Color>(_S("color"), sol::constructors <Color(), Color(int, int, int), Color(int, int, int, int)>(),
		(std::string)_S("r"), &Color::r,
		(std::string)_S("g"), &Color::g,
		(std::string)_S("b"), &Color::b,
		(std::string)_S("a"), &Color::a
		);

	lua.new_usertype <Vector>(_S("vector3"), sol::constructors <Vector(), Vector(float, float, float)>(),
		(std::string)_S("x"), &Vector::x,
		(std::string)_S("y"), &Vector::y,
		(std::string)_S("z"), &Vector::z,
		(std::string)_S("length"), &Vector::Length,
		(std::string)_S("length_sqr"), &Vector::LengthSqr,
		(std::string)_S("length_2d"), &Vector::Length2D,
		(std::string)_S("length_2d_sqr"), &Vector::Length2DSqr,
		(std::string)_S("is_zero"), &Vector::IsZero,
		(std::string)_S("is_valid"), &Vector::IsValid,
		(std::string)_S("zero"), &Vector::Zero,
		(std::string)_S("dist_to"), &Vector::DistTo,
		(std::string)_S("dist_to_sqr"), &Vector::DistToSqr,
		(std::string)_S("cross_product"), &Vector::Cross,
		(std::string)_S("normalize"), &Vector::Normalize
		);


	lua.new_usertype <IGameEvent>(_S("game_event"),
		(std::string)_S("get_bool"), &IGameEvent::GetBool,
		(std::string)_S("get_int"), &IGameEvent::GetInt,
		(std::string)_S("get_float"), &IGameEvent::GetFloat,
		(std::string)_S("get_string"), &IGameEvent::GetString,
		(std::string)_S("set_bool"), &IGameEvent::SetBool,
		(std::string)_S("set_int"), &IGameEvent::SetInt,
		(std::string)_S("set_float"), &IGameEvent::SetFloat,
		(std::string)_S("set_string"), &IGameEvent::SetString
		);

	lua.new_enum(_S("hitboxes"),
		_S("head"), CSGOHitboxID::Head,
		_S("neck"), CSGOHitboxID::Neck,
		_S("pelvis"), CSGOHitboxID::Pelvis,
		_S("stomach"), CSGOHitboxID::Stomach,
		_S("lower_chest"), CSGOHitboxID::LowerChest,
		_S("chest"), CSGOHitboxID::Chest,
		_S("upper_chest"), CSGOHitboxID::UpperChest,
		_S("right_thigh"), CSGOHitboxID::RightThigh,
		_S("left_thigh"), CSGOHitboxID::LeftThigh,
		_S("right_calf"), CSGOHitboxID::RightCalf,
		_S("left_calf"), CSGOHitboxID::LeftCalf,
		_S("right_foot"), CSGOHitboxID::RightFoot,
		_S("left_foot"), CSGOHitboxID::LeftFoot,
		_S("right_hand"), CSGOHitboxID::RightHand,
		_S("left_hand"), CSGOHitboxID::LeftHand,
		_S("right_upper_arm"), CSGOHitboxID::RightUpperArm,
		_S("right_forearm"), CSGOHitboxID::RightLowerArm,
		_S("left_upper_arm"), CSGOHitboxID::LeftUpperArm,
		_S("left_forearm"), CSGOHitboxID::LeftLowerArm
	);

	lua.new_usertype <player_t>(_S("player"), sol::base_classes, sol::bases<entity_t>(), 
		(std::string)_S("get_index"), &player_t::EntIndex,
		(std::string)_S("is_dormant"), &player_t::IsDormant,
		(std::string)_S("get_team"), &player_t::m_iTeamNum,
		(std::string)_S("is_alive"), &player_t::is_alive,
		(std::string)_S("get_velocity"), &player_t::m_vecVelocity,
		(std::string)_S("get_origin"), &player_t::GetAbsOrigin,
		(std::string)_S("get_angles"), &player_t::m_angEyeAngles,
		(std::string)_S("get_hitbox_position"), &player_t::hitbox_position,
		(std::string)_S("has_helmet"), &player_t::m_bHasHelmet,
		(std::string)_S("has_heavy_armor"), &player_t::m_bHasHeavyArmor,
		(std::string)_S("is_scoped"), &player_t::m_bIsScoped,
		(std::string)_S("get_health"), &player_t::m_iHealth
		);

	lua.new_usertype <weapon_t>(_S("weapon"), sol::base_classes, sol::bases<entity_t>(),
		(std::string)_S("is_empty"), &weapon_t::is_empty,
		(std::string)_S("can_fire"), &weapon_t::can_fire,
		(std::string)_S("is_non_scope"), &weapon_t::is_non_aim,
		(std::string)_S("can_double_tap"), &weapon_t::can_double_tap,
		(std::string)_S("get_name"), &weapon_t::get_name,
		(std::string)_S("get_inaccuracy"), &weapon_t::get_inaccuracy,
		(std::string)_S("get_spread"), &weapon_t::get_spread
		);

	lua.new_enum(_S("buttons"),
		_S("in_attack"), IN_ATTACK,
		_S("in_jump"), IN_JUMP,
		_S("in_duck"), IN_DUCK,
		_S("in_forward"), IN_FORWARD,
		_S("in_back"), IN_BACK,
		_S("in_use"), IN_USE,
		_S("in_cancel"), IN_CANCEL,
		_S("in_left"), IN_LEFT,
		_S("in_right"), IN_RIGHT,
		_S("in_moveleft"), IN_MOVELEFT,
		_S("in_moveright"), IN_MOVERIGHT,
		_S("in_attack2"), IN_ATTACK2,
		_S("in_run"), IN_RUN,
		_S("in_reload"), IN_RELOAD,
		_S("in_alt1"), IN_ALT1,
		_S("in_alt2"), IN_ALT2,
		_S("in_score"), IN_SCORE,
		_S("in_speed"), IN_SPEED,
		_S("in_walk"), IN_WALK,
		_S("in_zoom"), IN_ZOOM,
		_S("in_weapon1"), IN_WEAPON1,
		_S("in_weapon2"), IN_WEAPON2,
		_S("in_bullrush"), IN_BULLRUSH,
		_S("in_grenade1"), IN_GRENADE1,
		_S("in_grenade2"), IN_GRENADE2,
		_S("in_lookspin"), IN_LOOKSPIN
	);

	lua.new_usertype <shot_info>(_S("shot_info"), sol::constructors <>(),
		(std::string)_S("target_name"), &shot_info::target_name,
		(std::string)_S("result"), &shot_info::result,
		(std::string)_S("client_hitbox"), &shot_info::client_hitbox,
		(std::string)_S("server_hitbox"), &shot_info::server_hitbox,
		(std::string)_S("client_damage"), &shot_info::client_damage,
		(std::string)_S("server_damage"), &shot_info::server_damage,
		(std::string)_S("hitchance"), &shot_info::hitchance,
		(std::string)_S("backtrack_ticks"), &shot_info::backtrack_ticks,
		(std::string)_S("aim_point"), &shot_info::aim_point
		);

	auto client = lua.create_table();
	client[_S("register_callback")] = ns_client::add_callback;
	//client[_S("load_script")] = ns_client::load_script;
	//client[_S("unload_script")] = ns_client::unload_script;
	client[_S("log")] = ns_client::log;

	auto menu = lua.create_table();
	menu[_S("next_line")] = ns_menu::next_line;
	menu[_S("add_check_box")] = ns_menu::add_check_box;
	menu[_S("add_combo_box")] = ns_menu::add_combo_box;
	menu[_S("add_slider_int")] = ns_menu::add_slider_int;
	menu[_S("add_slider_float")] = ns_menu::add_slider_float;
	menu[_S("add_color_picker")] = ns_menu::add_color_picker;
	menu[_S("get_bool")] = ns_menu::get_bool;
	menu[_S("get_int")] = ns_menu::get_int;
	menu[_S("get_float")] = ns_menu::get_float;
	menu[_S("get_color")] = ns_menu::get_color;
	menu[_S("get_key_bind_state")] = ns_menu::get_key_bind_state;
	menu[_S("get_key_bind_mode")] = ns_menu::get_key_bind_mode;
	menu[_S("set_bool")] = ns_menu::set_bool;
	menu[_S("set_int")] = ns_menu::set_int;
	menu[_S("set_float")] = ns_menu::set_float;
	menu[_S("set_color")] = ns_menu::set_color;

	auto globals = lua.create_table();
	globals[_S("get_framerate")] = ns_globals::get_framerate;
	globals[_S("get_ping")] = ns_globals::get_ping;
	globals[_S("get_server_address")] = ns_globals::get_server_address;
	globals[_S("get_time")] = ns_globals::get_time;
	globals[_S("get_username")] = ns_globals::get_username;
	globals[_S("get_realtime")] = ns_globals::get_realtime;
	globals[_S("get_curtime")] = ns_globals::get_curtime;
	globals[_S("get_frametime")] = ns_globals::get_frametime;
	globals[_S("get_tickcount")] = ns_globals::get_tickcount;
	globals[_S("get_framecount")] = ns_globals::get_framecount;
	globals[_S("get_intervalpertick")] = ns_globals::get_intervalpertick;
	globals[_S("get_maxclients")] = ns_globals::get_maxclients;

	auto engine = lua.create_table();
	engine[_S("get_screen_width")] = ns_engine::get_screen_width;
	engine[_S("get_screen_height")] = ns_engine::get_screen_height;
	engine[_S("get_level_name")] = ns_engine::get_level_name;
	engine[_S("get_level_name_short")] = ns_engine::get_level_name_short;
	engine[_S("get_local_player_index")] = ns_engine::get_local_player_index;
	engine[_S("get_map_group_name")] = ns_engine::get_map_group_name;
	engine[_S("get_player_for_user_id")] = ns_engine::get_player_for_user_id;
	engine[_S("get_player_info")] = ns_engine::get_player_info;
	engine[_S("get_view_angles")] = ns_engine::get_view_angles;
	engine[_S("is_connected")] = ns_engine::is_connected;
	engine[_S("is_hltv")] = ns_engine::is_hltv;
	engine[_S("is_in_game")] = ns_engine::is_in_game;
	engine[_S("is_paused")] = ns_engine::is_paused;
	engine[_S("is_playing_demo")] = ns_engine::is_playing_demo;
	engine[_S("is_recording_demo")] = ns_engine::is_recording_demo;
	engine[_S("is_taking_screenshot")] = ns_engine::is_taking_screenshot;
	engine[_S("set_view_angles")] = ns_engine::set_view_angles;

	auto render = lua.create_table();
	render[_S("world_to_screen")] = ns_render::world_to_screen;
	render[_S("get_text_width")] = ns_render::get_text_width;
	render[_S("create_font")] = ns_render::create_font;
	render[_S("draw_text")] = ns_render::draw_text;
	render[_S("draw_text_centered")] = ns_render::draw_text_centered;
	render[_S("draw_line")] = ns_render::draw_line;
	render[_S("draw_rect")] = ns_render::draw_rect;
	render[_S("draw_rect_filled")] = ns_render::draw_rect_filled;
	render[_S("draw_rect_filled_gradient")] = ns_render::draw_rect_filled_gradient;
	render[_S("draw_circle")] = ns_render::draw_circle;
	render[_S("draw_circle_filled")] = ns_render::draw_circle_filled;
	render[_S("draw_triangle")] = ns_render::draw_triangle;

	auto console = lua.create_table();
	console[_S("execute")] = ns_console::execute;
	console[_S("get_int")] = ns_console::get_int;
	console[_S("get_float")] = ns_console::get_float;
	console[_S("get_string")] = ns_console::get_string;
	console[_S("set_int")] = ns_console::set_int;
	console[_S("set_float")] = ns_console::set_float;
	console[_S("set_string")] = ns_console::set_string;

	auto events = lua.create_table();
	events[_S("register_event")] = ns_events::register_event;

	auto entitylist = lua.create_table();
	entitylist[_S("get_local_player")] = ns_entitylist::get_local_player;
	entitylist[_S("get_player_by_index")] = ns_entitylist::get_player_by_index;
	entitylist[_S("get_weapon_by_player")] = ns_entitylist::get_weapon_by_player;

	auto cmd = lua.create_table();
	cmd[_S("get_send_packet")] = ns_cmd::get_send_packet;
	cmd[_S("set_send_packet")] = ns_cmd::set_send_packet;
	cmd[_S("get_choke")] = ns_cmd::get_choke;
	cmd[_S("get_button_state")] = ns_cmd::get_button_state;
	cmd[_S("set_button_state")] = ns_cmd::set_button_state;

	auto utils = lua.create_table();
	utils[_S("find_signature")] = ns_utils::find_signature;

	auto indicators = lua.create_table();
	indicators[_S("add")] = ns_indicator::add_indicator;
	indicators[_S("add_position")] = ns_indicator::add_indicator_with_pos;

	auto http = lua.create_table();
	http[_S("get")] = ns_http::get;
	http[_S("post")] = ns_http::post;

	auto exploits = lua.create_table();
	exploits[XorStr("can_double_tap")] = ns_exploits::can_doubletap;
	exploits[XorStr("get_shift_ticks")] = ns_exploits::get_shift_ticks;

	//auto file = lua.create_table();
	//file[_S("append")] = ns_file::append;
	//file[_S("write")] = ns_file::write;
	//file[_S("read")] = ns_file::read;

	lua[_S("client")] = client;
	lua[_S("menu")] = menu;
	lua[_S("globals")] = globals;
	lua[_S("engine")] = engine;
	lua[_S("render")] = render;
	lua[_S("cvar")] = console;
	lua[_S("events")] = events;
	lua[_S("entitylist")] = entitylist;
	lua[_S("cmd")] = cmd;
	lua[_S("utils")] = utils;
	lua[_S("indicators")] = indicators;
	lua[_S("http")] = http;
	lua[_S("exploits")] = exploits;
	//lua[_S("file")] = file;



	refresh_scripts();
}

int c_lua::get_script_id(const std::string& name)
{
	for (auto i = 0; i < scripts.size(); i++)
		if (scripts.at(i) == name) //-V106
			return i;

	return -1;
}

int c_lua::get_script_id_by_path(const std::string& path)
{
	for (auto i = 0; i < pathes.size(); i++)
		if (pathes.at(i).string() == path) //-V106
			return i;

	return -1;
}

void c_lua::refresh_scripts()
{
	auto oldLoaded = loaded;
	auto oldScripts = scripts;

	loaded.clear();
	pathes.clear();
	scripts.clear();
	ns_console::convars.clear();

	std::string folder;

	folder = _S("C:\\genesis_cs2_external\\scripts\\");

	CreateDirectoryA(folder.c_str(), NULL);

	auto i = 0;

	for (auto& entry : std::filesystem::directory_iterator(folder))
	{
		if (entry.path().extension() == _S(".lua") || entry.path().extension() == _S(".luac"))
		{
			auto path = entry.path();
			auto filename = path.filename().string();

			auto didPut = false;

			for (auto i = 0; i < oldScripts.size(); i++)
			{
				if (filename == oldScripts.at(i)) //-V106
				{
					loaded.emplace_back(oldLoaded.at(i)); //-V106
					didPut = true;
				}
			}

			if (!didPut)
				loaded.emplace_back(false);

			pathes.emplace_back(path);
			scripts.emplace_back(filename);

			items.emplace_back(std::vector <std::pair <std::string, menu_item>>());
			++i;
		}
	}

}

void c_lua::load_script(int id)
{
	if (id == -1)
		return;

	if (loaded.at(id)) //-V106
		return;

	auto path = get_script_path(id);

	if (path == _S(""))
		return;

	auto error_load = false;
	loaded.at(id) = true;
	lua.script_file(path,
		[&error_load](lua_State*, sol::protected_function_result result)
		{
			if (!result.valid())
			{
				sol::error error = result;
				auto log = _S("Lua error: ") + (std::string)error.what();

				logs_system::push_log(log, false);
				error_load = true;

			}

			return result;
		}
	);

	if (error_load | loaded.at(id) == false)
	{
		unload_script(id);
		return;
	}


	//-V106
	loaded_script = true;
}

void c_lua::unload_script(int id)
{
	if (id == -1)
		return;

	if (!loaded.at(id)) //-V106
		return;

	items.at(id).clear(); //-V106


	if (c_lua::get().events.find(id) != c_lua::get().events.end()) //-V807
		c_lua::get().events.at(id).clear();


	hooks.unregisterHooks(id);
	loaded.at(id) = false; //-V106
}

void c_lua::reload_all_scripts()
{
	for (auto current : scripts)
	{
		if (!loaded.at(get_script_id(current))) //-V106
			continue;

		unload_script(get_script_id(current));
		load_script(get_script_id(current));
	}
}

void c_lua::unload_all_scripts()
{
	for (auto s : scripts)
		unload_script(get_script_id(s));
}

std::string c_lua::get_script_path(const std::string& name)
{
	return get_script_path(get_script_id(name));
}

std::string c_lua::get_script_path(int id)
{
	if (id == -1)
		return _S("");

	return pathes.at(id).string(); //-V106
}