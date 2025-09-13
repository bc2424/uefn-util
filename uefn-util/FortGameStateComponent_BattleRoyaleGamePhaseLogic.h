#pragma once

#include "Includes.h"
#include "FortGameModeAthena.h"

static void (*InitializeFlightPath)(AFortAthenaMapInfo*, AFortGameStateAthena*, UFortGameStateComponent_BattleRoyaleGamePhaseLogic*, void*) = decltype(InitializeFlightPath)(BaseAddress() + 0x12FAACD0);
static __int64 (*FAircraftFlightConstructionInfo_Constructor)(void* AircraftFlightConstructionInfo, void* GameStateAthena, void* BattleRoyaleGamePhaseLogic) = decltype(FAircraftFlightConstructionInfo_Constructor)(BaseAddress() + 0x12F91F80);

namespace FortGameStateComponent_BattleRoyaleGamePhaseLogic
{
	void InitHooks()
	{
		auto DefaultFortGameStateComponent_BattleRoyaleGamePhaseLogic = UFortGameStateComponent_BattleRoyaleGamePhaseLogic::StaticClass()->DefaultObject;

		//auto HandlePlaylistDataLoadedFn = DefaultFortGameStateComponent_BattleRoyaleGamePhaseLogic->Class->GetFunction("FortGameStateComponent_BattleRoyaleGamePhaseLogic", "HandlePlaylistDataLoaded");
		//HookExec(HandlePlaylistDataLoadedFn, HandlePlaylistDataLoadedHook, (PVOID*)&HandlePlaylistDataLoaded);

		//auto HandleGamePhaseStepChangedFn = DefaultFortGameStateComponent_BattleRoyaleGamePhaseLogic->Class->GetFunction("FortGameStateComponent_BattleRoyaleGamePhaseLogic", "HandleGamePhaseStepChanged");
		//HookExec(HandleGamePhaseStepChangedFn, HandleGamePhaseStepChangedHook, (PVOID*)&HandleGamePhaseStepChanged);

		//CREATEHOOK(BaseAddress() + 0x15BE6AF0, UpdateGamePhaseStepHook, &UpdateGamePhaseStep);

		//CREATEHOOK(BaseAddress() + 0x15BDBCD0, HandlePlaylistDataLoadedHook, &HandlePlaylistDataLoaded);
		//CREATEHOOK(BaseAddress() + 0x15BE6AF0, TickComponentBRHook, &TickComponentBR);
	}
}