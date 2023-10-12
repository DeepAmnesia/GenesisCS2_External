#include "Entity.h"

template <typename T>
bool GetDataAddressWithOffset(const DWORD64& Address, DWORD Offset, T& Data)
{
	if (Address == 0)
	{
		//std::string res = "Error reading memory: " + std::to_string(Address) + "\nSend screenshot to support";
		//MessageBoxA(0, res.c_str(), _S("Genesis"), MB_ICONERROR | MB_OK);
		return false;
	}
	if (!ProcessMgr.ReadMemory<T>(Address + Offset, Data))
	{
		//std::string res = "Error reading memory: " + std::to_string(Address) + "\nOffset: "+ std::to_string(Offset) +"\nSend screenshot to support";
		//MessageBoxA(0, res.c_str(), _S("Genesis"), MB_ICONERROR | MB_OK);
		return false;
	}

	return true;
}

bool CEntity::UpdateController(const DWORD64& PlayerControllerAddress)
{
	if (PlayerControllerAddress == 0)
		return false;
	this->Controller.Address = PlayerControllerAddress;

	if (!this->Controller.GetHealth())
		return false;
	if (!this->Controller.GetIsAlive())
		return false;
	if (!this->Controller.GetTeamID())
		return false;
	if (!this->Controller.GetPlayerName())
		return false;

	this->Pawn.Address = this->Controller.GetPlayerPawnAddress();

	return true;
}

bool CEntity::UpdatePawn(const DWORD64& PlayerPawnAddress)
{
	if (PlayerPawnAddress == 0)
		return false;
	this->Pawn.Address = PlayerPawnAddress;

	if (!this->Pawn.GetCameraPos())
		return false;
	if (!this->Pawn.GetPos())
		return false;
	if (!this->Pawn.GetViewAngle())
		return false;
	if (!this->Pawn.GetWeaponName())
		return false;
	if (!this->Pawn.GetAimPunchAngle())
		return false;
	if (!this->Pawn.GetShotsFired())
		return false;
	if (!this->Pawn.GetHealth())
		return false;
	if (!this->Pawn.GetTeamID())
		return false;
	if (!this->Pawn.GetFov())
		return false;
	if (!this->Pawn.GetSpotted())
		return false;
	if (!this->Pawn.BoneData.UpdateAllBoneData(PlayerPawnAddress))
		return false;
	if (!this->Pawn.GetScoped())
		return false;
	if (!this->Pawn.GetDefusing())
		return false;
	if (!this->Pawn.GetFFlags())
		return false;
	if (!this->Pawn.GetAimPunchCache())
		return false;
	return true;
}

bool PlayerController::GetTeamID()
{
	return GetDataAddressWithOffset<int>(Address, Offset::Entity.TeamID, this->TeamID);
}

bool PlayerController::GetHealth()
{
	return GetDataAddressWithOffset<int>(Address, Offset::Entity.Health, this->Health);
}

bool PlayerController::GetIsAlive()
{
	return GetDataAddressWithOffset<int>(Address, Offset::Entity.IsAlive, this->AliveStatus);
}

bool PlayerController::GetPlayerName()
{
	char Buffer[MAX_PATH]{};

	if (!ProcessMgr.ReadMemory(Address + Offset::Entity.iszPlayerName, Buffer, MAX_PATH))
		return false;

	this->PlayerName = Buffer;
	if (this->PlayerName.empty())
		this->PlayerName = _S("Name_None");

	return true;
}
bool PlayerPawn::GetAimPunchCache()
{
	return GetDataAddressWithOffset<C_UTL_VECTOR>(Address, Offset::Pawn.aimPunchCache, this->AimPunchCache);
}

bool PlayerPawn::GetViewAngle()
{
	return GetDataAddressWithOffset<Vector3>(Address, Offset::Pawn.angEyeAngles, this->ViewAngle);
}

bool PlayerPawn::GetCameraPos()
{
	return GetDataAddressWithOffset<Vector3>(Address, Offset::Pawn.vecLastClipCameraPos, this->CameraPos);
}

bool PlayerPawn::GetSpotted()
{
	return GetDataAddressWithOffset<int>(Address, Offset::Pawn.bSpottedByMask, this->bSpottedByMask);
}
bool PlayerPawn::GetScoped()
{
	return GetDataAddressWithOffset<bool>(Address, Offset::Pawn.IsScoped, this->is_scoped);
}
bool PlayerPawn::GetDefusing()
{
	return GetDataAddressWithOffset<bool>(Address, Offset::Pawn.IsDefusing, this->is_defusing);
}
bool PlayerPawn::GetFFlags()
{
	return GetDataAddressWithOffset<int>(Address, Offset::Pawn.fFlags, this->fFlags);
}
bool PlayerPawn::GetWeaponName()
{
	DWORD64 WeaponNameAddress = 0;
	char Buffer[MAX_PATH]{};

	WeaponNameAddress = ProcessMgr.TraceAddress(this->Address + Offset::Pawn.pClippingWeapon, { 0x10,0x20 ,0x0 });
	if (WeaponNameAddress == 0)
		return false;

	if (!ProcessMgr.ReadMemory(WeaponNameAddress, Buffer, MAX_PATH))
		return false;

	WeaponName = std::string(Buffer);
	std::size_t Index = WeaponName.find(_S("_"));
	if (Index == std::string::npos || WeaponName.empty())
	{
		WeaponName = _S("Weapon_None");
	}
	else
	{
		WeaponName = WeaponName.substr(Index + 1, WeaponName.size() - Index - 1);
	}

	return true;
}

bool PlayerPawn::GetShotsFired()
{
	return GetDataAddressWithOffset<DWORD>(Address, Offset::Pawn.iShotsFired, this->ShotsFired);
}

bool PlayerPawn::GetAimPunchAngle()
{
	return GetDataAddressWithOffset<Vector3>(Address, Offset::Pawn.aimPunchAngle, this->AimPunchAngle);
}

bool PlayerPawn::GetTeamID()
{
	return GetDataAddressWithOffset<int>(Address, Offset::Pawn.iTeamNum, this->TeamID);
}

DWORD64 PlayerController::GetPlayerPawnAddress()
{
	DWORD64 EntityPawnListEntry = 0;
	DWORD64 EntityPawnAddress = 0;

	if (!GetDataAddressWithOffset<DWORD>(Address, Offset::Entity.PlayerPawn, this->Pawn))
		return 0;

	if (!ProcessMgr.ReadMemory<DWORD64>(gGame.GetEntityListAddress(), EntityPawnListEntry))
		return 0;

	if (!ProcessMgr.ReadMemory<DWORD64>(EntityPawnListEntry + 0x10 + 8 * ((Pawn & 0x7FFF) >> 9), EntityPawnListEntry))
		return 0;

	if (!ProcessMgr.ReadMemory<DWORD64>(EntityPawnListEntry + 0x78 * (Pawn & 0x1FF), EntityPawnAddress))
		return 0;

	return EntityPawnAddress;
}

bool PlayerPawn::GetPos()
{
	return GetDataAddressWithOffset<Vector3>(Address, Offset::Pawn.Pos, this->Pos);
}

bool PlayerPawn::GetHealth()
{
	return GetDataAddressWithOffset<int>(Address, Offset::Pawn.CurrentHealth, this->Health);
}

bool PlayerPawn::GetFov()
{
	DWORD64 CameraServices = 0;
	if (!ProcessMgr.ReadMemory<DWORD64>(Address + Offset::Pawn.CameraServices, CameraServices))
		return false;
	return GetDataAddressWithOffset<int>(CameraServices, Offset::Pawn.iFov, this->Fov);
}

bool CEntity::IsAlive()
{
	return this->Controller.AliveStatus == 1 && this->Controller.Health > 0;
}

bool CEntity::IsInScreen()
{
	return gGame.View.WorldToScreen(this->Pawn.Pos, this->Pawn.ScreenPos);
}

CBone CEntity::GetBone() const
{
	if (this->Pawn.Address == 0)
		return CBone{};
	return this->Pawn.BoneData;
}