#pragma once
#include <Windows.h>
#include "../utils/ProcessManager.hpp"
#include "../libs/xorstr.hpp"

namespace Offset
{
	inline DWORD EntityList;
	inline DWORD Matrix;
	inline DWORD ViewAngle;
	inline DWORD LocalPlayerController;
	inline DWORD LocalPlayerPawn;

	struct
{
DWORD Health = 0x808;
DWORD TeamID = 0x3BF;
DWORD IsAlive = 0x804;
DWORD PlayerPawn = 0x7FC;
DWORD iszPlayerName = 0x610;
}Entity;

struct
{
DWORD IsDefusing = 0x1390;
DWORD IsScoped = 0x1388;
DWORD Pos = 0xCD8;
DWORD MaxHealth = 0x328;
DWORD CurrentHealth = 0x32C;
DWORD GameSceneNode = 0x310;
DWORD BoneArray = 0x1E0;
DWORD angEyeAngles = 0x1500;
DWORD vecLastClipCameraPos = 0x1274;
DWORD pClippingWeapon = 0x1290;
DWORD iShotsFired = 0x1404;
DWORD aimPunchAngle = 0x1704;
DWORD aimPunchCache = 0x1728;
DWORD iIDEntIndex = 0x152C;
DWORD iTeamNum = 0x3BF;
DWORD CameraServices = 0x10E0;
DWORD iFov = 0x214;
DWORD bSpottedByMask = 0x1620 + 0xC; // entitySpottedState + bSpottedByMask
DWORD fFlags = 0x3C8;
}Pawn;

namespace Signatures
{
inline std::string EntityList = "48 8B 0D ?? ?? ?? ?? 48 89 7C 24 ?? 8B FA C1";
inline std::string LocalPlayerController = "48 8B 05 ?? ?? ?? ?? 48 85 C0 74 4F";
inline std::string ViewAngles = "48 8B 0D ?? ?? ?? ?? 48 8B 01 48 FF 60 30";
inline std::string ViewMatrix = "48 8D 0D ?? ?? ?? ?? 48 C1 E0 06";
inline std::string LocalPlayerPawn = "48 8D 05 ?? ?? ?? ?? C3 CC CC CC CC CC CC CC CC 48 83 EC ?? 8B 0D";
}

	// һ�����»�ַ
	bool UpdateOffsets();
}
