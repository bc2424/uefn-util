#pragma once

#include "FortPlayerController.h"
#include "FortInventory.h"
#include "Includes.h"

static void (*ServerAcknowledgePossession)(AFortPlayerControllerAthena*, AFortPlayerPawnAthena*);
void ServerAcknowledgePossessionHook(AFortPlayerControllerAthena* NewPlayer, AFortPlayerPawnAthena* P)
{
	auto PlayerState = Cast<AFortPlayerStateAthena>(NewPlayer->PlayerState);

	UFortKismetLibrary::UpdatePlayerCustomCharacterPartsVisualization(PlayerState);
	PlayerState->OnRep_CharacterData();

	TScriptInterface<IAbilitySystemInterface> AbilityInterface{};
	AbilityInterface.ObjectPointer = PlayerState;
	AbilityInterface.InterfacePointer = GetInterfaceAddress(PlayerState, IAbilitySystemInterface::StaticClass());

	InitializePlayerGameplayAbilities(AbilityInterface.InterfacePointer);

	NewPlayer->AcknowledgedPawn = P;

	if (!bRanPossess)
	{
		for (int i = 0; i < GetDefaultObject<UAthenaDeveloperSettings>()->CheatsToRunOnPlayerPawnPossession.Num(); ++i) //actually in onpossessed in pawn but lolll
		{
			auto& CheatString = GetDefaultObject<UAthenaDeveloperSettings>()->CheatsToRunOnPlayerPawnPossession[i];

			UKismetSystemLibrary::ExecuteConsoleCommand(NewPlayer->GetWorld(), CheatString, NewPlayer);
		}

		bRanPossess = true;
	}

	return(ServerAcknowledgePossession(NewPlayer, P));
}

static void (*OnPawnDied)(AFortPlayerControllerZone* DeadPlayerController, class AFortPlayerPawn* InstigatingPawn, float a3, FGameplayTagContainer& a4, FGameplayEffectContextHandle& a5, class AController* OpposingPlayerController, class AActor* a7, class AController* a8);
void OnPawnDiedHook(AFortPlayerControllerZone* DeadPlayerController, class AFortPlayerPawn* DeadPawn, float a3, FGameplayTagContainer& a4, FGameplayEffectContextHandle& a5, class AController* OpposingPlayerController, class AActor* a7, class AController* a8)
{
	AFortPlayerPawnAthena* PlayerPawnAthena = Cast<AFortPlayerPawnAthena>(DeadPlayerController->MyFortPawn);
	AFortPlayerStateAthena* PlayerStateAthena = Cast<AFortPlayerStateAthena>(DeadPlayerController->PlayerState);

	if (!PlayerPawnAthena || !PlayerStateAthena)
		return OnPawnDied(DeadPlayerController, DeadPawn, a3, a4, a5, OpposingPlayerController, a7, a8);

	AFortGameModeAthena* GameModeAthena = Cast<AFortGameModeAthena>(DeadPlayerController->GetWorld()->AuthorityGameMode);
	AFortGameStateAthena* GameStateAthena = Cast<AFortGameStateAthena>(GameModeAthena->GameState);

	if (!GameModeAthena || !GameStateAthena)
		return OnPawnDied(DeadPlayerController, DeadPawn, a3, a4, a5, OpposingPlayerController, a7, a8);

	if (!OpposingPlayerController)
		return OnPawnDied(DeadPlayerController, DeadPawn, a3, a4, a5, OpposingPlayerController, a7, a8);

	auto KillerPawn = (AFortPlayerPawnAthena*)OpposingPlayerController->Pawn;
	auto KillerPlayerState = (AFortPlayerStateAthena*)OpposingPlayerController->PlayerState;
	auto KillerPlayerControllerAthena = (AFortPlayerControllerAthena*)(OpposingPlayerController);

	float Distance = KillerPawn ? KillerPawn->GetDistanceTo(PlayerPawnAthena) : 0;

	EDeathCause DeathCause = AFortPlayerStateAthena::ToDeathCause(a4, PlayerPawnAthena->bIsDBNO);

	auto FinisherOrDowner = KillerPlayerState ? KillerPlayerState : PlayerStateAthena;

	FDeathInfo DeathInfo = {};
	TWeakObjectPtr<AActor> WeakFinisherOrDowner{};
	WeakFinisherOrDowner.ObjectIndex = FinisherOrDowner->Index;
	WeakFinisherOrDowner.ObjectSerialNumber = UObject::GObjects->GetObjByIndex(FinisherOrDowner->Index)->SerialNumber;

	DeathInfo.FinisherOrDowner = WeakFinisherOrDowner;
	DeathInfo.bDBNO = PlayerPawnAthena->bIsDBNO;
	DeathInfo.DeathCause = DeathCause;
	DeathInfo.Distance = (DeathCause == EDeathCause::FallDamage) ? PlayerPawnAthena->LastFallDistance : Distance;
	DeathInfo.DeathLocation = PlayerPawnAthena->K2_GetActorLocation();
	DeathInfo.bInitialized = true;
	PlayerStateAthena->PawnDeathLocation = DeathInfo.DeathLocation;

	PlayerStateAthena->DeathInfo = DeathInfo;
	PlayerStateAthena->OnRep_DeathInfo();

	AFortPlayerControllerAthena* PlayerControllerAthena = Cast<AFortPlayerControllerAthena>(DeadPlayerController);

	if (PlayerControllerAthena)
	{
		if (KillerPlayerState && PlayerStateAthena != KillerPlayerState)
		{
			KillerPlayerState->KillScore++;
			KillerPlayerState->OnRep_Kills();

			KillerPlayerState->ClientReportKill(PlayerStateAthena);

			for (int32 i = 0; i < GameStateAthena->Teams.Num(); i++)
			{
				AFortTeamInfo* TeamInfo = GameStateAthena->Teams[i];
				if (!TeamInfo) continue;

				if (TeamInfo->Team != KillerPlayerState->TeamIndex)
					continue;

				for (int32 j = 0; j < TeamInfo->TeamMembers.Num(); j++)
				{
					AFortPlayerControllerAthena* TeamMember = Cast<AFortPlayerControllerAthena>(TeamInfo->TeamMembers[j]);
					if (!TeamMember) continue;

					AFortPlayerStateAthena* TeamMemberPlayerState = Cast<AFortPlayerStateAthena>(TeamMember->PlayerState);
					if (!TeamMemberPlayerState) continue;

					TeamMemberPlayerState->TeamKillScore++;
					TeamMemberPlayerState->OnRep_TeamKillScore();

					TeamMemberPlayerState->ClientReportTeamKill(TeamMemberPlayerState->TeamKillScore);
				}
				break;
			}
		}

		if (!GameStateAthena->IsRespawningAllowed(PlayerStateAthena) && !PlayerPawnAthena->bIsDBNO && !GetDefaultObject<UAthenaDeveloperSettings>()->bAllowAthenaRespawning);
		{
			AFortPlayerStateAthena* CorrectKillerPlayerState = (KillerPlayerState && KillerPlayerState == PlayerStateAthena) ? nullptr : KillerPlayerState;
			UFortWeaponItemDefinition* KillerWeaponItemDefinition = nullptr;

			if (a7)
			{
				AFortProjectileBase* ProjectileBase = Cast<AFortProjectileBase>(a7);
				AFortWeapon* Weapon = Cast<AFortWeapon>(a7);

				if (ProjectileBase)
				{
					AFortWeapon* ProjectileBaseWeapon = Cast<AFortWeapon>(ProjectileBase->Owner);

					if (ProjectileBaseWeapon)
						KillerWeaponItemDefinition = ProjectileBaseWeapon->WeaponData;
				}
				else if (Weapon)
					KillerWeaponItemDefinition = Weapon->WeaponData;
			}

			bool bMatchEnded = GameModeAthena->HasMatchEnded();

			GameModeAthena->RemoveFromAlivePlayers(PlayerControllerAthena, CorrectKillerPlayerState, KillerPawn, KillerWeaponItemDefinition, DeathCause);

			if (GetDefaultObject<UAthenaDeveloperSettings>()->bAllowEndGamePhase)
			{
				for (int32 i = 0; i < GameStateAthena->Teams.Num(); i++)
				{
					AFortTeamInfo* TeamInfo = GameStateAthena->Teams[i];
					if (!TeamInfo) continue;

					if (TeamInfo->Team != PlayerStateAthena->TeamIndex)
						continue;

					bool bIsTeamAlive = false;
					for (int32 j = 0; j < TeamInfo->TeamMembers.Num(); j++)
					{
						AFortPlayerControllerAthena* TeamMember = Cast<AFortPlayerControllerAthena>(TeamInfo->TeamMembers[j]);
						if (!TeamMember || (TeamMember == PlayerControllerAthena)) continue;

						AFortPlayerPawn* TeamMemberPlayerPawn = Cast<AFortPlayerPawn>(TeamMember->MyFortPawn);
						if (!TeamMemberPlayerPawn || TeamMemberPlayerPawn->bIsDying) continue;

						bIsTeamAlive = true;
						break;
					}

					if (!bIsTeamAlive)
					{
						for (int32 j = 0; j < TeamInfo->TeamMembers.Num(); j++)
						{
							AFortPlayerControllerAthena* TeamMember = Cast<AFortPlayerControllerAthena>(TeamInfo->TeamMembers[j]);
							if (!TeamMember) continue;
						}
					}
					break;
				}

				if (GameStateAthena->TeamsLeft == 1 && KillerPlayerState && !bMatchEnded)
				{
					for (int32 i = 0; i < GameStateAthena->Teams.Num(); i++)
					{
						AFortTeamInfo* TeamInfo = GameStateAthena->Teams[i];
						if (!TeamInfo) continue;

						if (TeamInfo->Team != KillerPlayerState->TeamIndex)
							continue;

						for (int32 j = 0; j < TeamInfo->TeamMembers.Num(); j++)
						{
							AFortPlayerControllerAthena* TeamMember = Cast<AFortPlayerControllerAthena>(TeamInfo->TeamMembers[j]);
							if (!TeamMember) continue;

							AFortPlayerStateAthena* TeamMemberPlayerState = Cast<AFortPlayerStateAthena>(TeamMember->PlayerState);
							if (!TeamMemberPlayerState) continue;

							TeamMemberPlayerState->Place = GameStateAthena->TeamsLeft;
							TeamMemberPlayerState->OnRep_Place();

							GameModeAthena->StartEndGamePhase(KillerPlayerControllerAthena, KillerPawn, KillerWeaponItemDefinition, DeathCause);
							GameModeAthena->StartEndGamePhaseTeam(KillerPlayerControllerAthena, TeamMemberPlayerState->Place, CorrectKillerPlayerState, 1, KillerPawn, KillerWeaponItemDefinition, DeathCause);
						}
						break;
					}
				}
			}
		}
	}

	return OnPawnDied(DeadPlayerController, DeadPawn, a3, a4, a5, OpposingPlayerController, a7, a8);
}

static void (*ServerClientIsReadyToRespawn)(AFortPlayerControllerAthena*);
void ServerClientIsReadyToRespawnHook(AFortPlayerControllerAthena* NewPlayer)
{
	auto PlayerState = Cast<AFortPlayerStateAthena>(NewPlayer->PlayerState);

	auto GameState = (AFortGameStateAthena*)UWorld::GetWorld()->GameState;

	if (GameState->IsRespawningAllowed(PlayerState))
	{
		FFortRespawnData RespawnData = PlayerState->RespawnData;

		if (RespawnData.bServerIsReady && RespawnData.bRespawnDataAvailable)
		{
			auto GameMode = (AFortGameModeAthena*)UWorld::GetWorld()->AuthorityGameMode;

			FTransform SpawnTransform{};
			SpawnTransform.Translation = RespawnData.RespawnLocation;
			SpawnTransform.Rotation = UKismetMathLibrary::Conv_RotatorToQuaternion(RespawnData.RespawnRotation);
			SpawnTransform.Scale3D = FVector(1, 1, 1);

			AFortPlayerPawn* Pawn = (AFortPlayerPawn*)GameMode->SpawnDefaultPawnAtTransform(NewPlayer, SpawnTransform);

			Pawn->SetOwner(NewPlayer);

			NewPlayer->Possess(Pawn);

			Pawn->SetMaxHealth(100);
			Pawn->SetHealth(100);
			Pawn->SetMaxShield(100);
			Pawn->SetShield(0);

			TScriptInterface<IAbilitySystemInterface> AbilityInterface{};
			AbilityInterface.ObjectPointer = PlayerState;
			AbilityInterface.InterfacePointer = GetInterfaceAddress(PlayerState, IAbilitySystemInterface::StaticClass());

			InitializePlayerGameplayAbilities(AbilityInterface.InterfacePointer);

			NewPlayer->RespawnPlayerAfterDeath(true, false);

			RespawnData.bClientIsReady = true;

		}
	}

	return ServerClientIsReadyToRespawn(NewPlayer);
}

namespace FortPlayerControllerAthena
{
	void InitHooks()
	{
		auto DefaultFortPlayerControllerAthena = (AFortPlayerControllerAthena*)AFortPlayerControllerAthena::StaticClass()->DefaultObject;

		VirtualHook(DefaultFortPlayerControllerAthena->VTable, 396, ServerAcknowledgePossessionHook, (PVOID*)&ServerAcknowledgePossession);
		VirtualHook(DefaultFortPlayerControllerAthena->VTable, 1511, ServerClientIsReadyToRespawnHook, (PVOID*)&ServerClientIsReadyToRespawn);
		CREATEHOOK(BaseAddress() + 0x152F2430, OnPawnDiedHook, &OnPawnDied);

		VirtualHook(DefaultFortPlayerControllerAthena->VTable, 0x2AC, ServerEditBuildingActorHook, (PVOID*)&ServerEditBuildingActor);
		VirtualHook(DefaultFortPlayerControllerAthena->VTable, 0x253, ServerCheatHook, (PVOID*)&ServerCheat);
		VirtualHook(DefaultFortPlayerControllerAthena->VTable, 0x153, OnPossessHook, (PVOID*)&OnPossess);
		VirtualHook(DefaultFortPlayerControllerAthena->VTable, 0x252, ServerCheatAllHook, (PVOID*)&ServerCheatAll);

		UObject* DefaultInventoryOwnerInterface = (UObject*)DefaultFortPlayerControllerAthena->GetInventoryOwnerInterface();

		VirtualHook(DefaultInventoryOwnerInterface->VTable, 0x22, SetLoadedAmmoHook, (PVOID*)&SetLoadedAmmo);
		VirtualHook(DefaultInventoryOwnerInterface->VTable, 0x23, SetPhantomReserveAmmoHook, (PVOID*)&SetPhantomReserveAmmo);
	}
}