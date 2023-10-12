#pragma once
#include "../game.h"
#include "../classes/Entity.h"

namespace Bunnyhop
{
	inline void Run(const CEntity& Local)
	{
		const bool hasFlagInAir = Local.Pawn.HasFlag(PlayerPawn::Flags::IN_AIR);
		if ((::GetKeyState(VK_SPACE) & 0x8000) && !hasFlagInAir)
		{
			
		}
	}
}