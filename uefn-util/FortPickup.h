#pragma once

#include "Includes.h"

static bool (*GivePickupTo)(AFortPickup* Pickup, void* InventoryOwner, bool bDestroyPickup);
bool GivePickupToHook(AFortPickup* Pickup, void* InventoryOwner, bool bDestroyPickup)
{
	AFortPlayerController* PlayerController = AFortPlayerController::GetPlayerControllerFromInventoryOwnerInterface(InventoryOwner);

	auto WorldInventory = GetInventoryObject(PlayerController);

	auto Pawn = PlayerController->MyFortPawn;

	auto ItemDefinition = Cast<UFortItemDefinition>(Pickup->PrimaryPickupItemEntry.ItemDefinition);

	if (!ItemDefinition)
		return false;

	UFortWorldItem* ItemInstance = nullptr;

	bool Stacking = false;
	for (const auto Instance : GetInventoryObject(PlayerController)->Inventory.ItemInstances)
	{
		if (Instance->ItemEntry.Count < Cast<UFortItemDefinition>(Instance->ItemEntry.ItemDefinition)->GetMaxStackSize(Pawn->AbilitySystemComponent) && Instance->ItemEntry.ItemDefinition == Pickup->PrimaryPickupItemEntry.ItemDefinition)
		{
			Stacking = true;
			ItemInstance = Instance;
			break;
		}
	}
	bool IsSwapping = (IsInventoryFull(PlayerController) && !Stacking) || PlayerController->bTryPickupSwap;
	bool IsEquipping = Pawn->CurrentWeapon && Pawn->CurrentWeapon->ItemEntryGuid == Pickup->PickupLocationData.PickupGuid;

	if (IsSwapping)
	{
		if (PlayerController->bTryPickupSwap)
		{
			PlayerController->bTryPickupSwap = false;
		}
		for (int32 i = 0; i < WorldInventory->Inventory.ReplicatedEntries.Num(); i++)
		{
			FFortItemEntry& Entry = WorldInventory->Inventory.ReplicatedEntries[i];
			if (Pickup->PickupLocationData.PickupGuid == Entry.ItemGuid)
			{
				if(ItemInstance)
					if (!ItemInstance->CanBeDropped())
					{
						auto SpawnedPickup = SpawnPickup(PlayerController, PlayerController->MyFortPawn->K2_GetActorLocation(), ItemDefinition, Pickup->PrimaryPickupItemEntry.Count, Pickup->PrimaryPickupItemEntry.LoadedAmmo);
						SpawnedPickup->TossPickup(PlayerController->MyFortPawn->K2_GetActorLocation(), PlayerController->MyFortPawn, 0, true, true, EFortPickupSourceTypeFlag::Tossed, EFortPickupSpawnSource::TossedByPlayer);

						return false;
					}

				PlayerController->ServerAttemptInventoryDrop(Entry.ItemGuid, Entry.Count, false);
			}
		}
	}

	AddItem(PlayerController, ItemDefinition, Pickup->PrimaryPickupItemEntry.Count, Pickup->PrimaryPickupItemEntry.LoadedAmmo);

	if (IsSwapping && IsEquipping)
	{
		int size = WorldInventory->Inventory.ReplicatedEntries.Num();
		PlayerController->ClientEquipItem(WorldInventory->Inventory.ReplicatedEntries[size - 1].ItemGuid, false);
	}

	void(GivePickupTo(Pickup, InventoryOwner, bDestroyPickup));
	return true;
}

namespace FortPickup
{
	void InitHooks()
	{
		auto DefaultFortPickup = AFortPickup::StaticClass()->DefaultObject;

		VirtualHook(DefaultFortPickup->VTable, 0x143, GivePickupToHook, (PVOID*)&GivePickupTo);
	}
}

namespace FortPickupAthena
{
	void InitHooks()
	{
		auto DefaultFortPickupAthena = AFortPickupAthena::StaticClass()->DefaultObject;

		VirtualHook(DefaultFortPickupAthena->VTable, 0x143, GivePickupToHook, (PVOID*)&GivePickupTo);
	}
}