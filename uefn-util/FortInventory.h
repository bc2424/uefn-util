#pragma once

#include "Util.h"
#include "Globals.h"

AFortInventory* GetInventoryObject(AFortPlayerController* PlayerController)
{
	return Cast<AFortInventory>(PlayerController->WorldInventory.GetObjectRef());
}

bool IsPrimaryQuickbar(UFortItemDefinition* Item)
{
	EFortItemType Type = Item->ItemType;
	return Type == EFortItemType::WeaponHarvest || Type == EFortItemType::AthenaGadget || Type == EFortItemType::WeaponMelee
		|| Type == EFortItemType::WeaponRanged || Type == EFortItemType::Gadget || Type == EFortItemType::Consumable || Type == EFortItemType::AthenaGadget;
}

bool IsInventoryFull(AFortPlayerController* PlayerController)
{
	auto WorldInventory = GetInventoryObject(PlayerController)->Inventory;

	if (!PlayerController)
		return false;

	int PrimarySlots = 0;
	for (auto& ItemEntry : WorldInventory.ReplicatedEntries)
	{
		if (auto ItemDefinition = Cast<UFortItemDefinition>(ItemEntry.ItemDefinition))
		{
			if (ItemDefinition->FilterOverride == EFortInventoryFilter::Invisible) continue;

			if (IsPrimaryQuickbar(ItemDefinition))
			{
				PrimarySlots++;
			}
		}
	}

	return PrimarySlots >= 6;
}

UFortWorldItem* FindInventoryInstance(AFortPlayerController* PlayerController, UFortItemDefinition* ItemDefinition)
{
	for (int i = 0; i < GetInventoryObject(PlayerController)->Inventory.ItemInstances.Num(); i++)
	{
		if (GetInventoryObject(PlayerController)->Inventory.ItemInstances[i]->ItemEntry.ItemDefinition == ItemDefinition)
			return GetInventoryObject(PlayerController)->Inventory.ItemInstances[i];
	}
	return nullptr;
}

UFortWorldItem* FindItemInstanceByGUID(AFortPlayerController* PlayerController, FGuid GUID)
{
	auto WorldInventory = GetInventoryObject(PlayerController);
	if (PlayerController && WorldInventory)
	{
		auto WorldItem = (UFortWorldItem*)PlayerController->BP_GetInventoryItemWithGuid(GUID);

		if (WorldItem)
			return WorldItem;
	}

	return nullptr;
}

FFortItemEntry* FindItemEntryByGUID(AFortPlayerController* PlayerController, FGuid GUID)
{
	auto WorldInventory = GetInventoryObject(PlayerController);
	for (FFortItemEntry& ItemEntry : WorldInventory->Inventory.ReplicatedEntries)
	{
		if (ItemEntry.ItemGuid == GUID)
		{
			return &ItemEntry;
		}
	}

	return nullptr;
}


AFortPickup* SpawnPickupFromEntry(AFortPlayerController* PlayerController = nullptr, FVector Location = FVector{}, FFortItemEntry* ItemEntry = 0, int Count = 1)
{
	FTransform PickupTransform;
	PickupTransform.Scale3D.X = 1.0;
	PickupTransform.Scale3D.Y = 1.0;
	PickupTransform.Scale3D.Z = 1.0;
	PickupTransform.Translation = Location;
	PickupTransform.Rotation = GetDefaultObject<UKismetMathLibrary>()->Conv_RotatorToQuaternion(FRotator{ 0, GetDefaultObject<UKismetMathLibrary>()->RandomFloatInRange(0.0, 360.0), 0 });

	static auto PickupClass = StaticLoadObject<UClass>("/Script/FortniteGame.FortPickupAthena"); //get subgame broken?

	auto PickupActor = Cast<AFortPickup>(GetDefaultObject<UGameplayStatics>()->BeginDeferredActorSpawnFromClass(UWorld::GetWorld(), PickupClass, PickupTransform, ESpawnActorCollisionHandlingMethod::AlwaysSpawn, nullptr, ESpawnActorScaleMethod::SelectDefaultAtRuntime));

	if (PickupActor)
	{
		GetDefaultObject<UGameplayStatics>()->FinishSpawningActor(PickupActor, PickupTransform, ESpawnActorScaleMethod::SelectDefaultAtRuntime);
	}

	ItemEntry->Count = Count;
	ItemEntry->SetToDirty();

	PickupActor->PrimaryPickupItemEntry = *ItemEntry;
	PickupActor->OnRep_PrimaryPickupItemEntry();

	auto Pawn = Cast<AFortPlayerPawn>(PlayerController->Pawn);
	if (PlayerController && Pawn)
	{
		PickupActor->PickupLocationData.FlyTime = PickupActor->PickupLocationData.FlyTime / Pawn->PickupSpeedMultiplier;
		PickupActor->PickupLocationData.StartDirection.X = Pawn->GetActorForwardVector().X;
		PickupActor->PickupLocationData.StartDirection.Y = Pawn->GetActorForwardVector().Y;
		PickupActor->PickupLocationData.StartDirection.Z = Pawn->GetActorForwardVector().Z;
		PickupActor->OnRep_PickupLocationData();
		return PickupActor;
	}
}

AFortPickup* SpawnPickup(AFortPlayerController* PlayerController = nullptr, FVector Location = FVector{}, UItemDefinitionBase* ItemDefinition = nullptr, int Count = 1, int LoadedAmmo = -1)
{
	FTransform PickupTransform;
	PickupTransform.Scale3D.X = 1.0;
	PickupTransform.Scale3D.Y = 1.0;
	PickupTransform.Scale3D.Z = 1.0;
	PickupTransform.Translation = Location;
	PickupTransform.Rotation = GetDefaultObject<UKismetMathLibrary>()->Conv_RotatorToQuaternion(FRotator{ 0, GetDefaultObject<UKismetMathLibrary>()->RandomFloatInRange(0.0, 360.0), 0 });

	static auto PickupClass = StaticLoadObject<UClass>("/Script/FortniteGame.FortPickupAthena"); //get subgame broken?

	auto PickupActor = Cast<AFortPickup>(GetDefaultObject<UGameplayStatics>()->BeginDeferredActorSpawnFromClass(UWorld::GetWorld(), PickupClass, PickupTransform, ESpawnActorCollisionHandlingMethod::AlwaysSpawn, nullptr, ESpawnActorScaleMethod::SelectDefaultAtRuntime));

	if (PickupActor)
	{
		GetDefaultObject<UGameplayStatics>()->FinishSpawningActor(PickupActor, PickupTransform, ESpawnActorScaleMethod::SelectDefaultAtRuntime);
	}

	FFortItemEntry ItemEntry = UFortKismetLibrary::CreateItemEntry(ItemDefinition, Count, 0);
	ItemEntry.LoadedAmmo = LoadedAmmo;
	ItemEntry.Count = Count;
	ItemEntry.SetToDirty();

	PickupActor->PrimaryPickupItemEntry = ItemEntry;
	PickupActor->OnRep_PrimaryPickupItemEntry();

	auto Pawn = Cast<AFortPlayerPawn>(PlayerController->Pawn);
	if (PlayerController && Pawn)
	{
		PickupActor->PickupLocationData.FlyTime = PickupActor->PickupLocationData.FlyTime / Pawn->PickupSpeedMultiplier;
		PickupActor->PickupLocationData.StartDirection.X = Pawn->GetActorForwardVector().X;
		PickupActor->PickupLocationData.StartDirection.Y = Pawn->GetActorForwardVector().Y;
		PickupActor->PickupLocationData.StartDirection.Z = Pawn->GetActorForwardVector().Z;
		PickupActor->OnRep_PickupLocationData();
		return PickupActor;
	}
	return nullptr;
}


UFortWorldItem* AddItem(AFortPlayerController* PlayerController, UFortItemDefinition* ItemDefinition, int Count, int LoadedAmmo = -1)
{
	auto WorldInventory = GetInventoryObject(PlayerController);

	if (!PlayerController)
		return nullptr;

	if (!WorldInventory)
		return nullptr;

	bool ItemExists = false;

	for (auto& ItemEntry : WorldInventory->Inventory.ReplicatedEntries)
	{
		if (ItemEntry.ItemDefinition == ItemDefinition)
		{
			ItemExists = true;

			UFortWorldItem* ExistingInstance = nullptr;
			for (auto Instance : WorldInventory->Inventory.ItemInstances)
			{
				if (!Instance)
					continue;

				if (Instance->ItemEntry.ItemGuid == ItemEntry.ItemGuid)
				{
					ExistingInstance = Instance;
					break;
				}
			}
			if (!ExistingInstance)
			{
				break;
			}
			float MaxStackSize = ItemDefinition->GetMaxStackSize(nullptr);
			if (Count + ItemEntry.Count > MaxStackSize)
			{
				Count -= MaxStackSize - ItemEntry.Count;
				ItemEntry.Count = MaxStackSize;
				ExistingInstance->ItemEntry.Count = ItemEntry.Count;
			}
			else
			{
				ItemEntry.Count += Count;
				ExistingInstance->ItemEntry.Count = ItemEntry.Count;
				Count = 0;
			}
			//ItemEntry.SetToDirty();
			ExistingInstance->ItemEntry.GenericAttributeValues.Clear();
			ExistingInstance->ItemEntry.StateValues.Clear();

			ItemEntry.GenericAttributeValues.Clear();
			ItemEntry.StateValues.Clear();

			ExistingInstance->ItemEntry.SetToDirty();
			ItemEntry.SetToDirty();

			PlayerController->ClientForceUpdateQuickbar(EFortQuickBars::Primary);

			if (Count <= 0)
			{
				return nullptr;
			}
		}
	}

	if (LoadedAmmo == -1)
	{
		LoadedAmmo = 0;

		if (ItemDefinition->IsA(SDK::UFortWeaponItemDefinition::StaticClass()))
		{
			FFortBaseWeaponStats Stats;

			if (UFortKismetLibrary::GetWeaponStatsRow(Cast<UFortWeaponItemDefinition>(ItemDefinition)->WeaponStatHandle, &Stats))
			{
				LoadedAmmo = Stats.ClipSize;
			}
		}
	}

	if (!IsPrimaryQuickbar(ItemDefinition) && ItemExists && PlayerController->MyFortPawn)
	{
		auto SpawnedPickup = SpawnPickup(PlayerController, PlayerController->AcknowledgedPawn->K2_GetActorLocation(), ItemDefinition, Count, LoadedAmmo);
		SpawnedPickup->TossPickup(PlayerController->MyFortPawn->K2_GetActorLocation(), PlayerController->MyFortPawn, 0, true, true, EFortPickupSourceTypeFlag::Tossed, EFortPickupSpawnSource::TossedByPlayer);

		return nullptr;
	}

	if (!IsInventoryFull(PlayerController) || !IsPrimaryQuickbar(ItemDefinition) || ItemDefinition->FilterOverride == EFortInventoryFilter::Invisible)
	{
		UFortWorldItem* NewItem = Cast<UFortWorldItem>(ItemDefinition->CreateTemporaryItemInstanceBP(Count, 1));
		if (NewItem)
		{
			NewItem->ItemEntry.LoadedAmmo = LoadedAmmo;
			NewItem->ItemEntry.Count = Count;

			NewItem->SetOwningControllerForTemporaryItem(PlayerController);

			WorldInventory->InitializeExistingItem(NewItem);

			if (ItemDefinition->IsA(SDK::UFortGadgetItemDefinition::StaticClass()) || ItemDefinition->IsA(SDK::UAthenaGadgetItemDefinition::StaticClass()))
			{
				auto GadgetItemDefinition = (UFortGadgetItemDefinition*)(ItemDefinition);
				GadgetItemDefinition->ApplyGadgetData(PlayerController->GetInventoryOwnerInterface(), NewItem);
			}

			return NewItem;
		}
	}
	else
	{
		if (PlayerController->MyFortPawn)
		{
			auto SpawnedPickup = SpawnPickup(PlayerController, PlayerController->MyFortPawn->K2_GetActorLocation(), ItemDefinition, Count, LoadedAmmo);
			SpawnedPickup->TossPickup(PlayerController->MyFortPawn->K2_GetActorLocation(), PlayerController->MyFortPawn, 0, true, true, EFortPickupSourceTypeFlag::Tossed, EFortPickupSpawnSource::TossedByPlayer);
		}

	}

	return nullptr;
}

UFortWorldItem* AddItemForced(AFortPlayerController* PlayerController, UFortItemDefinition* ItemDefinition, int Count, int LoadedAmmo = 0)
{
	auto WorldInventory = GetInventoryObject(PlayerController);

	if (!PlayerController)
		return nullptr;

	if (!WorldInventory)
		return nullptr;

	UFortWorldItem* NewItem = Cast<UFortWorldItem>(ItemDefinition->CreateTemporaryItemInstanceBP(Count, 1));

	NewItem->ItemEntry.LoadedAmmo = LoadedAmmo;
	NewItem->ItemEntry.Count = Count;

	NewItem->SetOwningControllerForTemporaryItem(PlayerController);

	WorldInventory->InitializeExistingItem(NewItem);

	if (ItemDefinition->IsA(SDK::UFortGadgetItemDefinition::StaticClass()) || ItemDefinition->IsA(SDK::UAthenaGadgetItemDefinition::StaticClass()))
	{
		auto GadgetItemDefinition = (UFortGadgetItemDefinition*)(ItemDefinition);
		GadgetItemDefinition->ApplyGadgetData(PlayerController->GetInventoryOwnerInterface(), NewItem);
	}

	return NewItem;
}


UFortWorldItem* AddItemFromEntry(AFortPlayerController* PlayerController, FFortItemEntry ItemEntry)
{
	auto WorldInventory = GetInventoryObject(PlayerController);

	if (!PlayerController || !WorldInventory)
		return nullptr;

	auto ItemDefinition = Cast<UFortItemDefinition>(ItemEntry.ItemDefinition);

	if (ItemDefinition)
	{
		UFortWorldItem* NewItem = Cast<UFortWorldItem>(ItemDefinition->CreateTemporaryItemInstanceBP(ItemEntry.Count, 1));

		if (&ItemEntry)
		{
			NewItem->ItemEntry.CopyItemEntry(&ItemEntry);
		}

		NewItem->SetOwningControllerForTemporaryItem(PlayerController);

		WorldInventory->InitializeExistingItem(NewItem);

		if (ItemEntry.ItemDefinition->IsA(SDK::UFortGadgetItemDefinition::StaticClass()) || ItemEntry.ItemDefinition->IsA(SDK::UAthenaGadgetItemDefinition::StaticClass()))
		{
			auto GadgetItemDefinition = (UFortGadgetItemDefinition*)(ItemEntry.ItemDefinition);
			GadgetItemDefinition->ApplyGadgetData(PlayerController->GetInventoryOwnerInterface(), NewItem);
		}

		return NewItem;
	}

	return nullptr;
}

void RemoveItem(AFortPlayerController* PlayerController, FGuid Guid, int Count)
{
	if (!PlayerController)
		return;

	auto WorldInventory = GetInventoryObject(PlayerController);

	FFortItemEntry* ReplicatedItemEntry = FindItemEntryByGUID(PlayerController, Guid);
	if (!ReplicatedItemEntry) return;

	int NewCount = ReplicatedItemEntry->Count - Count;

	if (NewCount <= 0)
	{
		for (int i = 0; i < WorldInventory->Inventory.ItemInstances.Num(); i++)
		{
			UFortWorldItem* ItemInstance = WorldInventory->Inventory.ItemInstances[i];
			FGuid CurrentGuid = ItemInstance->GetItemGuid();

			if (CurrentGuid == Guid)
			{
				ItemInstance->ItemEntry.GenericAttributeValues.Clear();
				ItemInstance->ItemEntry.StateValues.Clear();
				WorldInventory->Inventory.ItemInstances.Remove(i);

				ReplicatedItemEntry->GenericAttributeValues.Clear();
				ReplicatedItemEntry->StateValues.Clear();
				WorldInventory->Inventory.ReplicatedEntries.Remove(i);

				ItemInstance->ItemEntry.SetToDirty();
				ReplicatedItemEntry->SetToDirty();

				WorldInventory->RecentlyRemoved.Add(ItemInstance);
				PlayerController->ClientForceUpdateQuickbar(EFortQuickBars::Primary);
				break;
			}
		}

		WorldInventory->HandleInventoryLocalUpdate();
	}
	else {
		UFortWorldItem* ItemInstance = FindItemInstanceByGUID(PlayerController, Guid);

		ItemInstance->ItemEntry.Count = NewCount;
		ReplicatedItemEntry->Count = NewCount;
		PlayerController->ClientForceUpdateQuickbar(EFortQuickBars::Primary);

		ItemInstance->ItemEntry.SetToDirty();
		ReplicatedItemEntry->SetToDirty();
	}
}

UItemDefinitionBase* FindItemDefFromGuid(AFortPlayerController* PlayerController, FGuid GUID)
{
	auto WorldInventory = GetInventoryObject(PlayerController);
	for (int i = 0; i < WorldInventory->Inventory.ReplicatedEntries.Num(); i++)
	{
		if (WorldInventory->Inventory.ReplicatedEntries[i].ItemGuid == GUID)
			return WorldInventory->Inventory.ReplicatedEntries[i].ItemDefinition;
	}

	return nullptr;
}
