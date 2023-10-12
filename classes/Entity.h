#pragma once
#include "../Game.h"
#include "View.hpp"
#include "Bone.h"
#include "../libs/xorstr.hpp"

struct C_UTL_VECTOR
{
	DWORD64 Count = 0;
	DWORD64 Data = 0;
};

class PlayerController
{
public:
	DWORD64 Address = 0;
	int TeamID = 0;
	int Health = 0;
	int AliveStatus = 0;
	DWORD Pawn = 0;
	std::string PlayerName;
public:
	bool GetTeamID();
	bool GetHealth();
	bool GetIsAlive();
	bool GetPlayerName();
	DWORD64 GetPlayerPawnAddress();
};

class PlayerPawn
{
public:
	enum class Flags
	{
		NONE,
		IN_AIR = 1 << 0
	};
	int fFlags;

	DWORD64 Address = 0;
	CBone BoneData;
	Vector3 ViewAngle;
	Vector3 Pos;
	Vector3 ScreenPos;
	Vector3 CameraPos;
	std::string WeaponName;
	DWORD ShotsFired;
	Vector3 AimPunchAngle;
	int Health;
	int TeamID;
	int Fov;
	bool is_scoped;
	bool is_defusing;
	int bSpottedByMask;
	C_UTL_VECTOR AimPunchCache;
	bool GetAimPunchCache();
public:
	bool GetPos();
	bool GetViewAngle();
	bool GetCameraPos();
	bool GetWeaponName();
	bool GetShotsFired();
	bool GetAimPunchAngle();
	bool GetHealth();
	bool GetTeamID();
	bool GetFov();
	bool GetSpotted();
	bool GetScoped();
	bool GetDefusing();
	bool GetFFlags();
	constexpr bool HasFlag(const Flags Flag) const noexcept {
		return fFlags & (int)Flag;
	}
};

class CEntity
{
public:
	PlayerController Controller;
	PlayerPawn Pawn;
public:
	// ��������
	bool UpdateController(const DWORD64& PlayerControllerAddress);
	bool UpdatePawn(const DWORD64& PlayerPawnAddress);
	// �Ƿ���
	bool IsAlive();
	// �Ƿ�����Ļ��
	bool IsInScreen();
	// ��ȡ��������
	CBone GetBone() const;
};