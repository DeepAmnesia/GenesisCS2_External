#pragma once
#include <thread>
#include "../render/render.hpp"
#include "../menu.h"
#include "math.hpp"
#include "../configs/cheat_cfg.hpp"
#include "esp.hpp"
#include "../libs/imgui/imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "../libs/imgui/imgui_internal.h"
#include "../classes/Entity.h"
#include "../game.h"
#include "radar.h"
#include "triggerbot.h"
#include <ctime>
#include <chrono>

typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::milliseconds milliseconds;

inline float AimFOV = 10.f;
inline bool isaimbotting;
inline bool smooth;

inline int ScreenCenterX = ScreenWidth / 2;
inline int ScreenCenterY = ScreenHeight / 2;

inline std::vector<int> get_color_for_hack(int iHealth)
{
	if (iHealth <= 10) return { 51, 14, 0 };
	if (iHealth <= 30) return { 255, 72, 0 };
	if (iHealth <= 60) return { 255, 136, 0 };
	if (iHealth <= 80) return { 255, 234, 0 };
	if (iHealth <= 100) return { 0, 255, 0 };
}

inline float DistanceBetweenCross(float X, float Y)
{
	float ydist = (Y - ScreenCenterY/*GetSystemMetrics(SM_CYSCREEN)/2*/);
	float xdist = (X - ScreenCenterX/*GetSystemMetrics(SM_CXSCREEN)/2*/);
	float Hypotenuse = sqrt(pow(ydist, 2) + pow(xdist, 2));
	return Hypotenuse;
}

inline bool GetAimKey()
{
	return (GetAsyncKeyState(VK_LBUTTON));
}

inline void AimAtPosSmoothed(float x, float y, aim_settings sett)
{
	float TargetX = 0;
	float TargetY = 0;

	//X Axis
	if (x != 0)
	{
		if (x > ScreenCenterX)
		{
			TargetX = -(ScreenCenterX - x);
			TargetX /= sett.smooth_value;
			if (TargetX + ScreenCenterX > ScreenCenterX * 2) TargetX = 0;
		}

		if (x < ScreenCenterX)
		{
			TargetX = x - ScreenCenterX;
			TargetX /= sett.smooth_value;
			if (TargetX + ScreenCenterX < 0) TargetX = 0;
		}
	}

	//Y Axis
	if (y != 0)
	{
		if (y > ScreenCenterY)
		{
			TargetY = -(ScreenCenterY - y);
			TargetY /= sett.smooth_value;
			if (TargetY + ScreenCenterY > ScreenCenterY * 2) TargetY = 0;
		}

		if (y < ScreenCenterY)
		{
			TargetY = y - ScreenCenterY;
			TargetY /= sett.smooth_value;
			if (TargetY + ScreenCenterY < 0) TargetY = 0;
		}
	}

	if (!sett.smooth)
	{
		mouse_event(MOUSEEVENTF_MOVE, (DWORD)(TargetX), (DWORD)(TargetY), NULL, NULL);
		return;
	}

	TargetX /= 10;
	TargetY /= 10;

	//Mouse even't will round to 0 a lot of the time, so we can add this, to make it more accurrate on slow speeds.
	if (fabs(TargetX) < 1)
	{
		if (TargetX > 0)
		{
			TargetX = 1;
		}
		if (TargetX < 0)
		{
			TargetX = -1;
		}
	}
	if (fabs(TargetY) < 1)
	{
		if (TargetY > 0)
		{
			TargetY = 1;
		}
		if (TargetY < 0)
		{
			TargetY = -1;
		}
	}
	mouse_event(MOUSEEVENTF_MOVE, TargetX, TargetY, NULL, NULL);
}

inline void AimAtPos(float x, float y, aim_settings sett)
{
	mouse_event(MOUSEEVENTF_MOVE, (DWORD)(x), (DWORD)(y), NULL, NULL);
	return;

}
inline std::vector<CEntity> aim_entities;


inline ImVec4 Get2DBox(const CEntity& Entity)
{
	BoneJointPos Head = Entity.GetBone().BonePosList[BONEINDEX::head];

	ImVec2 Size, Pos;
	Size.y = (Entity.Pawn.ScreenPos.y - Head.ScreenPos.y) * 1.09;
	Size.x = Size.y * 0.6;

	Pos = ImVec2(Entity.Pawn.ScreenPos.x - Size.x / 2, Head.ScreenPos.y - Size.y * 0.08);

	return ImVec4{ Pos.x,Pos.y,Size.x,Size.y };
}

inline CEntity get_nearest_entity(std::vector<CEntity> player_list)
{
	float distance = FLT_MAX;
	CEntity ent;
	for (int i = 0; i < player_list.size(); i++)
	{
		BoneJointPos Head = player_list[i].GetBone().BonePosList[BONEINDEX::head];
		float dist = DistanceBetweenCross(Head.ScreenPos.x, Head.ScreenPos.y);
		if (dist < distance)
		{
			distance = dist;
			ent = player_list[i];
		}

	}
	return ent;
}

inline void DrawBone(const CEntity& Entity, ImColor Color)
{
	BoneJointPos Previous, Current;
	for (auto i : BoneJointList::List)
	{
		Previous.Pos = Vector3(0, 0, 0);
		for (auto Index : i)
		{
			Current = Entity.GetBone().BonePosList[Index];
			if (Previous.Pos == Vector3(0, 0, 0))
			{
				Previous = Current;
				continue;
			}
			if (Previous.IsVisible && Current.IsVisible)
				g_Renderer->AddLine(ImVec2(Previous.ScreenPos.x, Previous.ScreenPos.y), ImVec2(Current.ScreenPos.x, Current.ScreenPos.y), Color);
			
			Previous = Current;
		}
	}
}

inline int weapon_to_int(std::string weapon)
{
	if (weapon == _S("m249")) return 1;
	if (weapon == _S("mag7")) return 2;
	if (weapon == _S("negev")) return 3;
	if (weapon == _S("nova")) return 4;
	if (weapon == _S("sawedoff")) return 5;
	if (weapon == _S("xm1014")) return 6;
	if (weapon == _S("cz75a")) return 7;
	if (weapon == _S("deagle")) return 8;
	if (weapon == _S("elite")) return 9;
	if (weapon == _S("fiveseven")) return 10;
	if (weapon == _S("glock")) return 11;
	if (weapon == _S("hkp2000")) return 12;
	if (weapon == _S("p250")) return 13;
	if (weapon == _S("revolver")) return 14;
	if (weapon == _S("tec9")) return 15;
	if (weapon == _S("usp_silencer")) 16;
	if (weapon == _S("ak47")) return 17;
	if (weapon == _S("aug")) return 18;
	if (weapon == _S("awp")) return 19;
	if (weapon == _S("famas")) return 20;
	if (weapon == _S("g3sg1")) return 21;
	if (weapon == _S("galilar")) return 22;
	if (weapon == _S("m4a1")) return 23;
	if (weapon == _S("m4a1_silencer")) 24;
	if (weapon == _S("scar20")) return 25;
	if (weapon == _S("sg556")) return 26;
	if (weapon == _S("ssg08")) return 27;
	if (weapon == _S("bizon")) return 28;
	if (weapon == _S("mac10")) return 29;
	if (weapon == _S("mp5sd")) return 30;
	if (weapon == _S("mp7")) return 31;
	if (weapon == _S("mp9")) return 32;
	if (weapon == _S("p90")) return 33;
	if (weapon == _S("ump45")) return 34;

	if (weapon == _S("M249")) return 1;
	if (weapon == _S("MAG-7")) return 2;
	if (weapon == _S("Negev")) return 3;
	if (weapon == _S("Nova")) return 4;
	if (weapon == _S("Sawed-Off")) return 5;
	if (weapon == _S("XM1014")) return 6;
	if (weapon == _S("CZ75-Auto")) return 7;
	if (weapon == _S("Desert Eagle")) return 8;
	if (weapon == _S("Dual Berettas")) return 9;
	if (weapon == _S("Five-SeveN")) return 10;
	if (weapon == _S("Glock-18")) return 11;
	if (weapon == _S("P2000")) return 12;
	if (weapon == _S("P250")) return 13;
	if (weapon == _S("R8 Revolver")) return 14;
	if (weapon == _S("Tec-9")) return 15;
	if (weapon == _S("USP-S")) return 16;
	if (weapon == _S("AK-47")) return 17;
	if (weapon == _S("AUG")) return 18;
	if (weapon == _S("AWP")) return 19;
	if (weapon == _S("FAMAS")) return 20;
	if (weapon == _S("G3SG1")) return 21;
	if (weapon == _S("Galil AR")) return 22;
	if (weapon == _S("M4A4")) return 23;
	if (weapon == _S("M4A1-S")) return 24;
	if (weapon == _S("SCAR-20")) return 25;
	if (weapon == _S("SG 553")) return 26;
	if (weapon == _S("SSG 08")) return 27;
	if (weapon == _S("PP-Bizon")) return 28;
	if (weapon == _S("MAC-10")) return 29;
	if (weapon == _S("MP5-SD")) return 30;
	if (weapon == _S("MP7")) return 31;
	if (weapon == _S("MP9")) return 32;
	if (weapon == _S("P90")) return 33;
	if (weapon == _S("UMP-45")) return 34;
	return 0;
}

inline std::string convert_weapon_name(std::string weapon)
{
	if (weapon == _S("m249")) return _S("M249");
	if (weapon == _S("mag7")) return _S("MAG-7");
	if (weapon == _S("negev")) return _S("Negev");
	if (weapon == _S("nova")) return _S("Nova");
	if (weapon == _S("sawedoff")) return _S("Sawed-Off");
	if (weapon == _S("xm1014")) return _S("XM1014");
	if (weapon == _S("cz75a")) return _S("CZ75-Auto");
	if (weapon == _S("deagle")) return _S("Desert Eagle");
	if (weapon == _S("elite")) return _S("Dual Berettas");
	if (weapon == _S("fiveseven")) return _S("Five-SeveN");
	if (weapon == _S("glock")) return _S("Glock-18");
	if (weapon == _S("hkp2000")) return _S("P2000");
	if (weapon == _S("p250")) return _S("P250");
	if (weapon == _S("revolver")) return  _S("R8 Revolver");
	if (weapon == _S("tec9")) return _S("Tec-9");
	if (weapon == _S("usp_silencer")) _S("USP-S");
	if (weapon == _S("ak47")) return _S("AK-47");
	if (weapon == _S("aug")) return _S("AUG");
	if (weapon == _S("awp")) return _S("AWP");
	if (weapon == _S("famas")) return _S("FAMAS");
	if (weapon == _S("g3sg1")) return _S("G3SG1");
	if (weapon == _S("galilar")) return _S("Galil AR");
	if (weapon == _S("m4a1")) return _S("M4A4");
	if (weapon == _S("m4a1_silencer"))return _S("M4A1-S");
	if (weapon == _S("scar20")) return _S("SCAR-20");
	if (weapon == _S("sg556")) return _S("SG 553");
	if (weapon == _S("ssg08")) return _S("SSG 08");
	if (weapon == _S("bizon")) return _S("PP-Bizon");
	if (weapon == _S("mac10")) return _S("MAC-10");
	if (weapon == _S("mp5sd")) return _S("MP5-SD");
	if (weapon == _S("mp7")) return _S("MP7");
	if (weapon == _S("mp9")) return _S("MP9");
	if (weapon == _S("p90")) return _S("P90");
	if (weapon == _S("ump45")) return _S("UMP-45");
	if (weapon == _S("knife")) return _S("Knige");
	return "";
}



inline bool is_sniper(std::string weapon)
{
		if (weapon == _S("awp")) return true;
		if (weapon == _S("scar20")) return true;
		if (weapon == _S("ssg08")) return true;
		if (weapon == _S("g3sg1")) return true;
		if (weapon == _S("AWP")) return true;
		if (weapon == _S("G3SG1")) return true;
		if (weapon == _S("SCAR-20")) return true;
		if (weapon == _S("SSG 08")) return true;
		return false;
}

inline BoneJointPos get_nearest_bone(CEntity entity)
{
	Vector3 result;
	BoneJointPos bone;
	float min_distance = FLT_MAX;

	std::vector<BoneJointPos> bones_list;
	bones_list.push_back(entity.Pawn.BoneData.BonePosList[BONEINDEX::head]);
	bones_list.push_back(entity.Pawn.BoneData.BonePosList[BONEINDEX::neck_0]);
	bones_list.push_back(entity.Pawn.BoneData.BonePosList[BONEINDEX::spine_2]);

	for (auto item : bones_list)
	{
		float dist = DistanceBetweenCross(item.ScreenPos.x, item.ScreenPos.y);
		if (min_distance > dist)
		{
			bone = item;
			min_distance = dist;
		}
	}

	return bone;
}

inline void render_crosshair()
{
	ImColor clr = ImColor(settings::get_color(_S("Crosshair"))[0], settings::get_color(_S("Crosshair"))[1], settings::get_color(_S("Crosshair"))[2]);
	if (settings::outline)
	{
		g_Renderer->AddLine(ImVec2(ScreenCenterX - settings::line_padding * dpi_scale + 1, ScreenCenterY), ImVec2(ScreenCenterX - settings::line_padding * dpi_scale - settings::line_length * dpi_scale - 1, ScreenCenterY), ImColor(0, 0, 0), settings::line_width + 3.f);
		g_Renderer->AddLine(ImVec2(ScreenCenterX + settings::line_padding * dpi_scale - 1, ScreenCenterY), ImVec2(ScreenCenterX + settings::line_padding * dpi_scale + settings::line_length * dpi_scale + 1, ScreenCenterY), ImColor(0, 0, 0), settings::line_width + 3.f);
		g_Renderer->AddLine(ImVec2(ScreenCenterX, ScreenCenterY - settings::line_padding * dpi_scale + 1), ImVec2(ScreenCenterX, ScreenCenterY - settings::line_padding * dpi_scale - settings::line_length * dpi_scale - 1), ImColor(0, 0, 0), settings::line_width + 3.f);
		g_Renderer->AddLine(ImVec2(ScreenCenterX, ScreenCenterY + settings::line_padding * dpi_scale - 1), ImVec2(ScreenCenterX, ScreenCenterY + settings::line_padding * dpi_scale + settings::line_length * dpi_scale + 1), ImColor(0, 0, 0), settings::line_width + 3.f);
	}
	g_Renderer->AddLine(ImVec2(ScreenCenterX - settings::line_padding * dpi_scale, ScreenCenterY), ImVec2(ScreenCenterX - settings::line_padding * dpi_scale - settings::line_length * dpi_scale, ScreenCenterY), clr, settings::line_width);
	g_Renderer->AddLine(ImVec2(ScreenCenterX + settings::line_padding * dpi_scale, ScreenCenterY), ImVec2(ScreenCenterX + settings::line_padding * dpi_scale + settings::line_length * dpi_scale, ScreenCenterY), clr, settings::line_width);
	g_Renderer->AddLine(ImVec2(ScreenCenterX, ScreenCenterY - settings::line_padding * dpi_scale), ImVec2(ScreenCenterX, ScreenCenterY - settings::line_padding * dpi_scale - settings::line_length * dpi_scale), clr, settings::line_width);
	g_Renderer->AddLine(ImVec2(ScreenCenterX, ScreenCenterY + settings::line_padding * dpi_scale), ImVec2(ScreenCenterX, ScreenCenterY + settings::line_padding * dpi_scale + settings::line_length * dpi_scale), clr, settings::line_width);
}

namespace hack {

	inline void loop() 
	{
		ScreenCenterX = ScreenWidth / 2;
		ScreenCenterY = ScreenHeight / 2;

		// Update matrix
		if (!ProcessMgr.ReadMemory(gGame.GetMatrixAddress(), gGame.View.Matrix, 64))
			return;

		// Update EntityList Entry
		gGame.UpdateEntityListEntry();

		DWORD64 LocalControllerAddress = 0;
		DWORD64 LocalPawnAddress = 0;

		if (!ProcessMgr.ReadMemory(gGame.GetLocalControllerAddress(), LocalControllerAddress))
			return;
		if (!ProcessMgr.ReadMemory(gGame.GetLocalPawnAddress(), LocalPawnAddress))
			return;

		// LocalEntity
		CEntity LocalEntity;
		if (!LocalEntity.UpdateController(LocalControllerAddress))
			return;
		if (!LocalEntity.UpdatePawn(LocalPawnAddress) && 0)
			return;

		// Radar Data
		g_Radar.Points.clear();
		
		aim_entities.clear();

		for (int i = 0; i < 64; i++)
		{
			CEntity Entity;
			DWORD64 EntityAddress = 0;
			if (!ProcessMgr.ReadMemory<DWORD64>(gGame.GetEntityListEntry() + (i + 1) * 0x78, EntityAddress))
				continue;
			if (EntityAddress == LocalEntity.Controller.Address)
				continue;
			if (!Entity.UpdateController(EntityAddress))
				continue;
			if (!Entity.UpdatePawn(Entity.Pawn.Address))
				continue;

			if (Entity.Controller.TeamID == LocalEntity.Controller.TeamID)
				continue;

			if (!Entity.IsAlive())
				continue;

			// Add entity to radar
			g_Radar.AddPoint(LocalEntity.Pawn.Pos, LocalEntity.Pawn.ViewAngle.y, Entity.Pawn.Pos, ImColor(237, 85, 106, 200), 2, Entity.Pawn.ViewAngle.y);

			if (!Entity.IsInScreen())
				continue;

			bool should_render = true;


			ImColor box_color = ImColor(settings::get_color(_S("Box Invisible"))[0], settings::get_color(_S("Box Invisible"))[1], settings::get_color(_S("Box Invisible"))[2]);
#ifndef clean_logs
			printf("entity working\n");
#endif
			if (Entity.Pawn.bSpottedByMask > 0)
			{
				box_color = ImColor(settings::get_color(_S("Box Visible"))[0], settings::get_color(_S("Box Visible"))[1], settings::get_color(_S("Box Visible"))[2]);
				aim_entities.push_back(Entity);
				should_render = true;
			}
			else if (settings::esp_visible)
				should_render = false;

			Vector3 screenhead = Vector3(Get2DBox(Entity).x, Get2DBox(Entity).y, 0);
			ImVec2 box_size = ImVec2(Get2DBox(Entity).z, Get2DBox(Entity).w);

			ESPPlayerData_t m_Data;
			bool defusing = true;
			bool scoped = true;

			if (settings::esp_enable && should_render)
			{
				for (auto a = 0; a < POOL_COND; a++)
				{
					m_Data.m_iDownOffset = 0;
					m_Data.m_iUpOffset = 0;

					m_Data.m_iLeftDownOffset = 0;
					m_Data.m_iLeftOffset = 0;

					m_Data.m_iRightDownOffset = 0;
					m_Data.m_iRightOffset = 0;

					for (auto b = 0; b < Pool[a].size(); b++)
					{
						if (Pool[a][b].Type == 0) {
							std::string Text = Entity.Controller.PlayerName;
							int Size = 14;
							ImColor Color;
							int outline;
							int transform;
							if (Pool[a][b].ItemName == _S("Username"))
							{
								if (Text.length() > 32)
								{
									Text.erase(32, Text.length() - 32);
									Text.append("...");
								}
								outline = settings::esp_name_flag;
								transform = settings::esp_name_transform;
								Size = settings::username_fontsize;
								Color = ImColor(settings::get_color(_S("Username"))[0], settings::get_color(_S("Username"))[1], settings::get_color(_S("Username"))[2]);
							}
							else if (Pool[a][b].ItemName == _S("Scope")) {
								if (scoped)
								{
									Size = settings::scope_fontsize;
									Text = convert_weapon_name(Entity.Pawn.WeaponName);
									Color = ImColor(settings::get_color(_S("Scope"))[0], settings::get_color(_S("Scope"))[1], settings::get_color(_S("Scope"))[2]);
								}
								else
									continue;
							}
							else if (Pool[a][b].ItemName == _S("Defusing")) {
								if (defusing)
								{
									Size = settings::defusing_fontsize;
									Text = _S("Defusing");
									Color = ImColor(settings::get_color(_S("Defusing"))[0], settings::get_color(_S("Defusing"))[1], settings::get_color(_S("Defusing"))[2]);
								}
								else
									continue;
							}
							else if (Pool[a][b].ItemName == _S("Weapon")) {
								if (scoped)
								{
									outline = settings::esp_weapon_flag;
									transform = settings::esp_weapon_transform;
									Size = settings::weapon_fontsize;
									Text = convert_weapon_name(Entity.Pawn.WeaponName);
									Color = ImColor(settings::get_color(_S("Weapon"))[0], settings::get_color(_S("Weapon"))[1], settings::get_color(_S("Weapon"))[2]);
								}
								else
									continue;
							}


							g_PlayerESP->AddText(Text, Size, (DraggableItemCondiction)a, Color, screenhead, box_size, m_Data, transform, outline);
						}
						if (Pool[a][b].Type == 1) {
							ImColor Main;
							ImColor Inner;
							ImColor Outer;

							if (Pool[a][b].ItemName == _S("Health")) {
								float iHealthValue = std::clamp(Entity.Pawn.Health, 0, 100);

								Main = ImColor(settings::get_color(_S("Health Bar"))[0], settings::get_color(_S("Health Bar"))[1], settings::get_color(_S("Health Bar"))[2]);
								Outer = ImColor(settings::get_color(_S("Health Gradient Top"))[0], settings::get_color(_S("Health Gradient Top"))[1], settings::get_color(_S("Health Gradient Top"))[2]);
								Inner = ImColor(settings::get_color(_S("Health Gradient Bottom"))[0], settings::get_color(_S("Health Gradient Bottom"))[1], settings::get_color(_S("Health Gradient Bottom"))[2]);

								g_PlayerESP->AddBar((DraggableItemCondiction)a, iHealthValue, 100, Main, Outer, Inner, screenhead, box_size, m_Data, settings::health_color_type, iHealthValue);
							}

							if (Pool[a][b].ItemName == _S("Armor")) {
								float iArmorValue = std::clamp(100, 0, 100);
				
								Main = ImColor(settings::get_color(_S("Armor Bar"))[0], settings::get_color(_S("Armor Bar"))[1], settings::get_color(_S("Armor Bar"))[2]);
								Outer = ImColor(0,0,0);
								Inner = ImColor(0, 0, 0);

								//g_PlayerESP->AddBar((DraggableItemCondiction)a, iArmorValue, 100, Main, Inner, Outer, screenhead, box_size, m_Data);
							}

							if (Pool[a][b].ItemName == _S("Ammo"))
							{

							}
						}
						if (Pool[a][b].Type == 2) 
							g_PlayerESP->RenderBox(screenhead, box_size, m_Data, box_color, settings::esp_box_type, settings::box_outline);
					}
				}
				if (settings::esp_skeleton)
				{
					ImColor clr = ImColor(settings::get_color(_S("Skeleton"))[0], settings::get_color(_S("Skeleton"))[1], settings::get_color(_S("Skeleton"))[2]);
					if (settings::skeleton_color_type == 1)
						clr = ImColor(get_color_for_hack(Entity.Pawn.Health)[0], get_color_for_hack(Entity.Pawn.Health)[1], get_color_for_hack(Entity.Pawn.Health)[2]);
					DrawBone(Entity, clr);
				}
			}
		}

		//draw crosshair
		if (is_sniper(LocalEntity.Pawn.WeaponName) && settings::draw_crosshair)
			render_crosshair();

		//triggerbot
		TriggerBot::Run(LocalEntity);

		//aimbot
		//render aimbot fov
		aim_settings aim_config = settings::aim_settings_bot[weapon_to_int(LocalEntity.Pawn.WeaponName)].override_global? settings::aim_settings_bot[weapon_to_int(LocalEntity.Pawn.WeaponName)] : settings::global_aim;
		aim_is_working = false;
		if (aim_config.enable && weapon_to_int(LocalEntity.Pawn.WeaponName) != 0)
		{
			if (is_sniper(LocalEntity.Pawn.WeaponName) && !LocalEntity.Pawn.is_scoped)
				return;
#ifndef clean_logs
			printf(std::to_string(weapon_to_int(LocalEntity.Pawn.WeaponName)).c_str());
			printf("\n");
			printf(std::to_string(aim_config.aim_fov).c_str());
			printf("\n");
			printf(std::to_string(aim_config.bone_num).c_str());
			printf("\n");
			printf(std::to_string(aim_config.enable).c_str());
			printf("\n");
			printf(std::to_string(aim_config.override_global).c_str());
			printf("\n");
			printf(LocalEntity.Pawn.WeaponName.c_str());
			printf("\n");
			printf("rendering fov\n");
#endif
			if (settings::render_aim_fov)
			{
				ImColor clr = ImColor(settings::get_color(_S("Aim FOV"))[0], settings::get_color(_S("Aim FOV"))[1], settings::get_color(_S("Aim FOV"))[2]);
				g_Renderer->AddCircle2D(Vector3(0, 0, 0), aim_config.aim_fov * dpi_scale * 10 * 3, aim_config.aim_fov * dpi_scale * 10, clr);
			}

			if (aim_entities.size() == 0)
				return;

			if (menuShow)
				return;

#ifndef clean_logs
			printf("inside aimbot\n");
			printf((std::to_string(aim_entities.size()) + ("\n")).c_str());
#endif
			CEntity entity = get_nearest_entity(aim_entities); 

			//target switch delay stuff
			static Clock::time_point start, finish;
			static bool need_wait = false;
			static CEntity prev_antity = entity;

			Vector3 target;

			
			

			//select bone
			int bone_index;
			if (aim_config.bone_num == 0)
				bone_index = BONEINDEX::head;
			if (aim_config.bone_num == 1)
				bone_index = BONEINDEX::neck_0;
			if (aim_config.bone_num == 2)
				bone_index = BONEINDEX::spine_2;

			if (aim_config.bone_num > 2)
				target = get_nearest_bone(entity).ScreenPos;
			else
				target = entity.GetBone().BonePosList[bone_index].ScreenPos;

			//add rcs 
			//if (aim_config.rcs)
			//	target = { target.x - LocalEntity.Pawn.AimPunchAngle.x * aim_config.smooth_value, target.y + LocalEntity.Pawn.AimPunchAngle.y * aim_config.smooth_value, 0 };

			if (DistanceBetweenCross(target.x, target.y) <= aim_config.aim_fov * dpi_scale * 10)
			{
				if (settings::render_aim_line)
				{
					ImColor clr = ImColor(settings::get_color(_S("Aim line"))[0], settings::get_color(_S("Aim line"))[1], settings::get_color(_S("Aim line"))[2]);
					g_Renderer->AddLine(ImVec2(ScreenCenterX, ScreenCenterY), ImVec2(target.x, target.y), clr);
				}
				if (GetAimKey() && !trigger_is_working)
				{
					aim_is_working = true;
					if (aim_is_working && 0)
					{
						if (prev_antity.Controller.PlayerName != entity.Controller.PlayerName)
						{
							if (!need_wait)
							{
								start = Clock::now();
								need_wait = true;
							}
							else
							{
								finish = Clock::now();
								if (std::chrono::duration_cast<milliseconds>(finish - start).count() < aim_config.target_switch_delay * 1000)
									return;
								else
								{
									prev_antity = entity;
									need_wait = false;
								}
							}

						}
					}

					Vec2 PunchAngle;
					if (LocalEntity.Pawn.AimPunchCache.Count <= 0 && LocalEntity.Pawn.AimPunchCache.Count > 0xFFFF)
						return;
					if (!ProcessMgr.ReadMemory<Vec2>(LocalEntity.Pawn.AimPunchCache.Data + (LocalEntity.Pawn.AimPunchCache.Count - 1) * sizeof(Vector3), PunchAngle))
						return;

					if (aim_config.rcs)
					{
						target.x -= PunchAngle.x;
						target.y -= PunchAngle.y;
					}
					AimAtPosSmoothed(target.x, target.y, aim_config);
				}
				else
				{
					need_wait = false;
				}
			}
		}
	}
}

