#pragma once

#include "Includes.h"

static void (*MovingEmoteStopped)(UObject*, FFrame&, void*);
void MovingEmoteStoppedHook(UObject* Context, FFrame& Stack, void* Ret)
{
	auto Pawn = (AFortPlayerPawn*)Context;

	Pawn->bMovingEmote = false;
	Pawn->bMovingEmoteSkipLandingFX = false;
	Pawn->bMovingEmoteForwardOnly = false;
	Pawn->bMovingEmoteFollowingOnly = false;

	return MovingEmoteStopped(Context, Stack, Ret);
}

static void (*ServerHandlePickupInfo)(class AFortPlayerPawn* Pawn, class AFortPickup* Pickup, const struct FFortPickupRequestInfo& Params_0);
void ServerHandlePickupInfoHook(class AFortPlayerPawn* Pawn, class AFortPickup* Pickup, const struct FFortPickupRequestInfo& Params_0)
{
	float FlyTime = Params_0.FlyTime / Pawn->PickupSpeedMultiplier;

	auto PlayerController = Cast<AFortPlayerController>(Pawn->GetController());

	if (!PlayerController)
		return;

	auto WorldInventory = GetInventoryObject(PlayerController);

	if (!WorldInventory)
		return;

	auto ItemDefinition = Cast<UFortItemDefinition>(Pickup->PrimaryPickupItemEntry.ItemDefinition);

	if (IsInventoryFull(PlayerController) && IsPrimaryQuickbar(ItemDefinition) && Pawn->CurrentWeapon != nullptr)
	{
		Pickup->PickupLocationData.PickupGuid = Pawn->CurrentWeapon->ItemEntryGuid;
	}

	Pickup->SetPickupTarget(Pawn, FlyTime, Params_0.Direction, Params_0.bPlayPickupSound);

	return ServerHandlePickupInfo(Pawn, Pickup, Params_0);
}

void (*OnDeathServer)(AFortPawn* Pawn, float Damage, const FGameplayTagContainer& DamageTags, const FVector& Momentum, const FHitResult& HitInfo, AController* InstigatedBy, AActor* DamageCauser, const FGameplayEffectContextHandle& EffectContext);
void OnDeathServerHook(AFortPawn* Pawn, float Damage, const FGameplayTagContainer& DamageTags, const FVector& Momentum, const FHitResult& HitInfo, AController* InstigatedBy, AActor* DamageCauser, const FGameplayEffectContextHandle& EffectContext)
{
	OnDeathServer(Pawn, Damage, DamageTags, Momentum, HitInfo, InstigatedBy, DamageCauser, EffectContext);

	auto PlayerControllerAthena = (AFortPlayerControllerAthena*)Pawn->Controller;
	if (!PlayerControllerAthena) return;

	auto GameStateAthena = (AFortGameStateAthena*)Pawn->GetWorld()->GameState;
	if (!GameStateAthena) return;

	if (!Pawn->IsA(SDK::AFortPlayerPawnAthena::StaticClass())) return;

	if (PlayerControllerAthena->IsA(AFortPlayerControllerAthena::StaticClass()))
	{
		if (PlayerControllerAthena && !GameStateAthena->IsRespawningAllowed((AFortPlayerStateAthena*)Pawn->PlayerState));
		{
			PlayerControllerAthena->ServerDropAllItems(nullptr);
		}
	}

}

namespace FortPlayerPawnAthena
{
	void InitHooks()
	{
		auto DefaultFortPlayerPawnAthena = AFortPlayerPawnAthena::StaticClass()->DefaultObject;

		VirtualHook(DefaultFortPlayerPawnAthena->VTable, 0x2A6, ServerHandlePickupInfoHook, (PVOID*)&ServerHandlePickupInfo);
	}
}

namespace FortPlayerPawn
{
	void InitHooks()
	{
		auto DefaultFortPlayerPawn = AFortPlayerPawn::StaticClass()->DefaultObject;

		VirtualHook(DefaultFortPlayerPawn->VTable, 0x2A6, ServerHandlePickupInfoHook, (PVOID*)&ServerHandlePickupInfo);
	}
}

namespace FortPawn
{
	void InitHooks()
	{
		auto DefaultFortPawn = AFortPawn::StaticClass()->DefaultObject;

		CREATEHOOK(BaseAddress() + 0x1503A990, OnDeathServerHook, &OnDeathServer);

		auto MovingEmoteStoppedFn = DefaultFortPawn->Class->GetFunction("FortPawn", "MovingEmoteStopped");
		HookExec(MovingEmoteStoppedFn, MovingEmoteStoppedHook, (PVOID*)&MovingEmoteStopped);
	}
}
