#include "TriggerBot.h"
#include "../libs/xorstr.hpp"

inline int weapon_to_int2(std::string weapon)
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
	if (weapon == _S("SSG 08)")) return 27;
	if (weapon == _S("PP-Bizon")) return 28;
	if (weapon == _S("MAC-10")) return 29;
	if (weapon == _S("MP5-SD")) return 30;
	if (weapon == _S("MP7")) return 31;
	if (weapon == _S("MP9")) return 32;
	if (weapon == _S("P90")) return 33;
	if (weapon == _S("UMP-45")) return 34;
	return 0;
}

inline bool is_sniper2(std::string weapon)
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

void TriggerBot::Run(const CEntity& LocalEntity)
{
	trigger_is_working = false;
	if (!weapon_to_int2(LocalEntity.Pawn.WeaponName))
		return;

	trigger_settings aim_config = settings::trigger_settings_bot[weapon_to_int2(LocalEntity.Pawn.WeaponName)].override_global ? settings::trigger_settings_bot[weapon_to_int2(LocalEntity.Pawn.WeaponName)] : settings::global_trigger;

	if (!aim_config.TriggerBot)
		return;

	if (is_sniper2(LocalEntity.Pawn.WeaponName) && !LocalEntity.Pawn.is_scoped)
		return;

	DWORD uHandle = 0;
	if (!ProcessMgr.ReadMemory<DWORD>(LocalEntity.Pawn.Address + Offset::Pawn.iIDEntIndex, uHandle))
		return;
	if (uHandle == -1)
		return;

	DWORD64 ListEntry = 0;
	ListEntry = ProcessMgr.TraceAddress(gGame.GetEntityListAddress(), { 0x8 * (uHandle >> 9) + 0x10,0x0 });
	if (ListEntry == 0)
		return;

	DWORD64 PawnAddress = 0;
	if (!ProcessMgr.ReadMemory<DWORD64>(ListEntry + 0x78 * (uHandle & 0x1FF), PawnAddress))
		return;

	CEntity Entity;
	if (!Entity.UpdatePawn(PawnAddress))
		return;

	bool AllowShoot = LocalEntity.Pawn.TeamID != Entity.Pawn.TeamID && Entity.Pawn.Health > 0;

	if (AllowShoot)
	{
		static std::chrono::time_point LastTimePoint = std::chrono::steady_clock::now();
		auto CurTimePoint = std::chrono::steady_clock::now();
		if (CurTimePoint - LastTimePoint >= std::chrono::milliseconds(TriggerDelay))
		{
			trigger_is_working = true;
			mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
			if (!aim_is_working)
			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
			LastTimePoint = CurTimePoint;
		}
	}
}