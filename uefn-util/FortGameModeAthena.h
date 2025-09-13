#pragma once

#include "Includes.h"
#include "FortInventory.h"
#include "Memory.h"

static void (*CreateAIServices)(UAthenaAIServiceManager*, TArray<TSubclassOf<class UAthenaAIService>>) = decltype(CreateAIServices)(BaseAddress() + 0x15F9A940);
static void (*OnPlaylistDataLoaded)(UAthenaAIServiceManager*, AFortGameModeAthena*) = decltype(OnPlaylistDataLoaded)(BaseAddress() + 0x15FB0570);

static void (*HandleStartingNewPlayer)(AFortGameModeAthena*, AFortPlayerControllerAthena*);
void HandleStartingNewPlayerHook(AFortGameModeAthena* GameMode, AFortPlayerControllerAthena* NewPlayer)
{
	auto PickaxeDefinition = NewPlayer->CosmeticLoadoutPC.Pickaxe->WeaponDefinition;

	if (UKismetSystemLibrary::IsValidSoftObjectReference((TSoftObjectPtr<class UObject>)GetDefaultObject<UCosmeticDeveloperSettings>()->CosmeticLoadoutOverrides.Pickaxe))
	{
		TSoftObjectPtr<class UAthenaPickaxeItemDefinition> PickaxeDefinitionCheats = GetDefaultObject<UCosmeticDeveloperSettings>()->CosmeticLoadoutOverrides.Pickaxe;
		auto PickaxeDefinitionLoaded = (UAthenaPickaxeItemDefinition*)UKismetSystemLibrary::LoadAsset_Blocking((TSoftObjectPtr<class UObject>)PickaxeDefinitionCheats);

		if (PickaxeDefinitionLoaded)
		{
			PickaxeDefinition = PickaxeDefinitionLoaded->WeaponDefinition;
		}
	}

	AddItemForced(NewPlayer, PickaxeDefinition, 1, 0);

	for (int32 i = 0; i < GameMode->StartingItems.Num(); i++)
	{
		auto& StartingItem = GameMode->StartingItems[i];

		if (StartingItem.Item->IsA(UFortSmartBuildingItemDefinition::StaticClass()))
			continue;

		AddItemForced(NewPlayer, Cast<UFortItemDefinition>(StartingItem.Item), StartingItem.Count);
	}

	AddItemForced(NewPlayer, UFortKismetLibrary::K2_GetResourceItemDefinition(EFortResourceType::Wood), 500, 0);
	AddItemForced(NewPlayer, UFortKismetLibrary::K2_GetResourceItemDefinition(EFortResourceType::Stone), 500, 0);
	AddItemForced(NewPlayer, UFortKismetLibrary::K2_GetResourceItemDefinition(EFortResourceType::Metal), 500, 0);
	AddItemForced(NewPlayer, UFortKismetLibrary::K2_GetResourceItemDefinition(EFortResourceType::Permanite), 1, 0);

	if (UFortKismetLibrary::InEditorOrPIE)
	{
		for (int i = 0; i < GetDefaultObject<UAthenaDeveloperSettings>()->InventoryItemsToGrant.Num(); ++i)
		{
			auto& BRDevItemInstance = GetDefaultObject<UAthenaDeveloperSettings>()->InventoryItemsToGrant[i];

			if (!UKismetSystemLibrary::IsValid(BRDevItemInstance.Item))
				continue;

			AddItemForced(NewPlayer, Cast<UFortItemDefinition>(BRDevItemInstance.Item), BRDevItemInstance.Count);
		}
	}

	if (NewPlayer->BroadcastRemoteClientInfo)
	{
		NewPlayer->BroadcastRemoteClientInfo->bActive = true;
		NewPlayer->BroadcastRemoteClientInfo->OnRep_bActive();
		NewPlayer->BroadcastRemoteClientInfo->OnRep_bActive();
	}

	NewPlayer->bClientSideEditPrediction = true;

	return HandleStartingNewPlayer(GameMode, NewPlayer);
}

static bool (*PlayerCanRestart)(AFortGameModeAthena*, AFortPlayerControllerAthena*);
bool PlayerCanRestartHook(AFortGameModeAthena* GameMode, AFortPlayerControllerAthena* NewPlayer)
{
	return true;
}

bool (*ReadyToStartMatch)(AFortGameModeAthena* GameMode);
bool ReadyToStartMatchHook(AFortGameModeAthena* GameMode)
{
	AFortGameStateAthena* GameState = (AFortGameStateAthena*)UWorld::GetWorld()->GameState;

	if (UKismetSystemLibrary::IsValidSoftObjectReference((TSoftObjectPtr<class UObject>)GetDefaultObject<UAthenaDeveloperSettings>()->Playlist))
	{
		GameState->CurrentPlaylistInfo.BasePlaylist = GetDefaultObject<UAthenaDeveloperSettings>()->Playlist.Get();
		GameState->CurrentPlaylistInfo.OverridePlaylist = GetDefaultObject<UAthenaDeveloperSettings>()->Playlist.Get();
	}
	else
	{
		static auto PlaylistDefault = StaticFindObject<UFortPlaylistAthena>("/Game/Athena/Playlists/Playlist_DefaultPIE.Playlist_DefaultPIE");

		GameState->CurrentPlaylistInfo.BasePlaylist = PlaylistDefault;
		GameState->CurrentPlaylistInfo.OverridePlaylist = PlaylistDefault;
	}

	auto Playlist = GameState->CurrentPlaylistInfo.BasePlaylist;

	auto AthenaAISpawner = UFortAIFunctionLibrary::GetAISpawner(UWorld::GetWorld());
	auto AthenaAISystem = (UAthenaAISystem*)(AthenaAISpawner->Outer);
	auto AIServiceManager = AthenaAISystem->AIServiceManager;

	GameState->bPlaylistDataIsLoaded = true;

	GameState->CurrentPlaylistInfo.MarkArrayDirty();
	GameState->OnRep_CurrentPlaylistInfo();

	static auto GamePhaseComponentClass = StaticFindObject<UClass>("/Script/FortniteGame.FortGameStateComponent_BattleRoyaleGamePhaseLogic");
	auto GamePhaseComponent = Cast<UFortGameStateComponent_BattleRoyaleGamePhaseLogic>(GameState->GetComponentByClass(GamePhaseComponentClass));
	GamePhaseComponent->GamePhase = EAthenaGamePhase::None;
	GamePhaseComponent->OnRep_GamePhase(EAthenaGamePhase::None);

	GameMode->WarmupRequiredPlayerCount = GetDefaultObject<UAthenaDeveloperSettings>()->AthenaWarmupRequiredPlayerCount;
	GamePhaseComponent->WarmupEarlyCountdownDuration = GetDefaultObject<UAthenaDeveloperSettings>()->AthenaWarmupEarlyCountdownDuration;
	GamePhaseComponent->WarmupCountdownDuration = GetDefaultObject<UAthenaDeveloperSettings>()->AthenaWarmupCountdownDuration;

	if (GetDefaultObject<UAthenaDeveloperSettings>()->AthenaStartupPhase == EAthenaPIEStartupMode::Warmup)
	{
		GamePhaseComponent->GamePhase = EAthenaGamePhase::Warmup;
		GamePhaseComponent->OnRep_GamePhase(EAthenaGamePhase::Warmup);
	}

	if (GetDefaultObject<UAthenaDeveloperSettings>()->AthenaStartupPhase == EAthenaPIEStartupMode::Aircraft)
	{
		GamePhaseComponent->GamePhase = EAthenaGamePhase::Aircraft;
		GamePhaseComponent->OnRep_GamePhase(EAthenaGamePhase::Aircraft);
	}

	if (GetDefaultObject<UAthenaDeveloperSettings>()->AthenaStartupPhase == EAthenaPIEStartupMode::Gameplay)
	{
		GamePhaseComponent->GamePhase = EAthenaGamePhase::SafeZones;
		GamePhaseComponent->OnRep_GamePhase(EAthenaGamePhase::SafeZones);
	}

	float TimeSeconds = GameState->GetServerWorldTimeSeconds();

	GamePhaseComponent->WarmupCountdownStartTime = TimeSeconds;
	GamePhaseComponent->WarmupCountdownEndTime = TimeSeconds + GamePhaseComponent->WarmupEarlyCountdownDuration;
	GamePhaseComponent->OnRep_WarmupCountdownEndTime();

	GamePhaseComponent->AirCraftBehavior = Playlist->AirCraftBehavior;

	ReadyToStartMatch(GameMode);

	bRanPossess = false;

	bool ret = true;
	return ret;
}

static APawn* (*SpawnDefaultPawnFor)(AFortGameModeAthena*, AFortPlayerControllerAthena*, AActor*);
APawn* SpawnDefaultPawnForHook(AFortGameModeAthena* GameMode, AFortPlayerControllerAthena* NewPlayer, AActor* StartSpot)
{
	auto PlayerPawn = GameMode->SpawnDefaultPawnAtTransform(NewPlayer, StartSpot->GetTransform());
	SpawnDefaultPawnFor(GameMode, NewPlayer, StartSpot);

	return PlayerPawn;
}