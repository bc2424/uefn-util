#pragma once

#include "Includes.h"
#include "FortGameModeAthena.h"

namespace FortGameModeBR
{
	void InitHooks()
	{
		auto DefaultFortGameModeBR = AFortGameModeBR::StaticClass()->DefaultObject;

		VirtualHook(DefaultFortGameModeBR->VTable, 382, ReadyToStartMatchHook, (PVOID*)&ReadyToStartMatch);
		VirtualHook(DefaultFortGameModeBR->VTable, 326, HandleStartingNewPlayerHook, (PVOID*)&HandleStartingNewPlayer);
		VirtualHook(DefaultFortGameModeBR->VTable, 320, SpawnDefaultPawnForHook, (PVOID*)&SpawnDefaultPawnFor);
		VirtualHook(DefaultFortGameModeBR->VTable, 321, PlayerCanRestartHook, (PVOID*)&PlayerCanRestart);
	}
}