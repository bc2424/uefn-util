#pragma once

#include "Includes.h"
#include "FortInventory.h"
#include "AbilitySystemComponent_Abilities.h"
bool bRanPossess = false;

static void (*SetWorldInventory)(AFortPlayerController*, void*) = decltype(SetWorldInventory)(BaseAddress() + 0x152842F0);
static void (*InitializePlayerGameplayAbilities)(void*) = decltype(InitializePlayerGameplayAbilities)(BaseAddress() + 0x142CBD00);

static ABuildingSMActor* (*ReplaceBuildingActor)(ABuildingSMActor* BuildingSMActor, int32 a2, TSubclassOf<class ABuildingSMActor> BuildingClass, int32 BuildingLevel, int32 RotationIterations, bool bMirrored, class AFortPlayerController* PlayerController) = decltype(ReplaceBuildingActor)(BaseAddress() + 0x137A8330);

static void (*ServerAttemptInventoryDrop)(UObject*, FFrame&, void*);
void ServerAttemptInventoryDropHook(UObject* Context, FFrame& Stack, void* Ret)
{
    auto PlayerController = (AFortPlayerController*)Context;
    auto Pawn = (AFortPlayerPawn*)PlayerController->Pawn;

    auto Params = (Params::FortPlayerController_ServerAttemptInventoryDrop*)Stack.Locals;

    auto ItemGuid = Params->ItemGuid;
    auto Count = Params->Count;
    auto ItemEntry = FindItemEntryByGUID(PlayerController, ItemGuid);

    if (!ItemEntry)
        return ServerAttemptInventoryDrop(Context, Stack, Ret);

    if (!Pawn)
        return ServerAttemptInventoryDrop(Context, Stack, Ret);

    if (!PlayerController)
        return ServerAttemptInventoryDrop(Context, Stack, Ret);

    if (!Cast<UFortWorldItemDefinition>(ItemEntry->ItemDefinition)->bCanBeDropped)
        return;

    auto Pickup = SpawnPickup(PlayerController, Pawn->K2_GetActorLocation(), ItemEntry->ItemDefinition, Count, ItemEntry->LoadedAmmo);
    Pickup->TossPickup(Pawn->K2_GetActorLocation(), Pawn, 0, true, true, EFortPickupSourceTypeFlag::Tossed, EFortPickupSpawnSource::TossedByPlayer);

    RemoveItem(PlayerController, ItemGuid, Count);

    return ServerAttemptInventoryDrop(Context, Stack, Ret);
}

static bool (*RemoveInventoryItem)(void* InventoryOwner, const FGuid& ItemGuid, int32 Count, bool bForceRemoveFromQuickBars, bool bForceRemoval, bool bForcePersistWhenEmpty);
bool RemoveInventoryItemHook(void* InventoryOwner, const FGuid& ItemGuid, int32 Count, bool bForceRemoveFromQuickBars, bool bForceRemoval, bool bForcePersistWhenEmpty)
{
    AFortPlayerController* PlayerController = AFortPlayerController::GetPlayerControllerFromInventoryOwnerInterface(InventoryOwner);

	AFortInventory* WorldInventory = GetInventoryObject(PlayerController);

    if (!PlayerController) return false;

    if (Count == 0)
        return true;

    if (PlayerController->bInfiniteAmmo && !bForceRemoval)
        return true;

    UFortWorldItem* WorldItem = Cast<UFortWorldItem>(PlayerController->BP_GetInventoryItemWithGuid(ItemGuid));
    if (!WorldItem) return false;

    FFortItemEntry ItemEntry = WorldItem->ItemEntry;

    if (Count == -1)
    {
        RemoveItem(PlayerController, ItemGuid, Count);
        return true;
    }

    if (Count >= ItemEntry.Count)
    {
        UFortWeaponRangedItemDefinition* WeaponRangedItemDefinition = Cast<UFortWeaponRangedItemDefinition>(ItemEntry.ItemDefinition);

        if (WeaponRangedItemDefinition && (WeaponRangedItemDefinition->bPersistInInventoryWhenFinalStackEmpty || bForcePersistWhenEmpty))
        {
            if (WeaponRangedItemDefinition->DropBehavior != EWorldItemDropBehavior::DropAsPickupEvenWhenEmpty && false)
            {
                int32 ItemQuantity = UFortKismetLibrary::K2_GetItemQuantityOnPlayer(PlayerController, WeaponRangedItemDefinition, FGuid());

                if (ItemQuantity == 1)
                {
                    RemoveItem(PlayerController, ItemGuid, Count);
                    return true;
                }
            }
        }

        RemoveItem(PlayerController, ItemGuid, Count);
    }
    else if (Count < ItemEntry.Count)
    {
        int32 NewCount = ItemEntry.Count - Count;

        RemoveItem(PlayerController, ItemGuid, Count);
    }
    else
        return false;

    return true;
}

static void (*ServerExecuteInventoryItem)(UObject*, FFrame&, void*);
void ServerExecuteInventoryItemHook(UObject* Context, FFrame& Stack, void* Ret)
{
    auto PlayerController = (AFortPlayerController*)Context;

    auto Params = (Params::FortPlayerController_ServerExecuteInventoryItem*)Stack.Locals;
    auto Pawn = (AFortPlayerPawn*)PlayerController->Pawn;

    if (!Pawn)
        return ServerExecuteInventoryItem(Context, Stack, Ret);

    auto Item = (UFortWorldItem*)PlayerController->BP_GetInventoryItemWithGuid(Params->ItemGuid);
    auto ItemDef = Item->GetItemDefinitionBP();

    if (!ItemDef || !Item)
        return ServerExecuteInventoryItem(Context, Stack, Ret);

    if (ItemDef->IsA(SDK::UFortGadgetItemDefinition::StaticClass()))
    {
        auto GdgetItemDef = (UFortGadgetItemDefinition*)ItemDef;
        GdgetItemDef->ServerExecute(Item, PlayerController);

        return ServerExecuteInventoryItem(Context, Stack, Ret);
    }

    if (ItemDef->IsA(SDK::UFortEditToolItemDefinition::StaticClass()))
    {
        auto EditToolItemDef = (UFortEditToolItemDefinition*)ItemDef;
        EditToolItemDef->ServerExecute(Item, PlayerController);

        return ServerExecuteInventoryItem(Context, Stack, Ret);
    }

    if (ItemDef->IsA(SDK::UFortBuildingItemDefinition::StaticClass()))
    {
        auto BuildingItemDef = (UFortBuildingItemDefinition*)ItemDef;
        BuildingItemDef->ServerExecute(Item, PlayerController);

        return ServerExecuteInventoryItem(Context, Stack, Ret);
    }

    if (ItemDef->IsA(SDK::UFortDecoItemDefinition::StaticClass()))
    {
        auto DecoItemDef = (UFortDecoItemDefinition*)ItemDef;
        Pawn->PickUpActor(nullptr, DecoItemDef);

        DecoItemDef->ServerExecute(Item, PlayerController);

        if (ItemDef->IsA(SDK::UFortContextTrapItemDefinition::StaticClass()))
        {
            auto ContextTrapItemDef = (UFortContextTrapItemDefinition*)ItemDef;
            ContextTrapItemDef->ServerExecute(Item, PlayerController);
        }

        return ServerExecuteInventoryItem(Context, Stack, Ret);
    }

    if (ItemDef->IsA(SDK::UFortWeaponItemDefinition::StaticClass()))
    {
        auto WeaponItemDef = (UFortWeaponItemDefinition*)ItemDef;
        WeaponItemDef->ServerExecute(Item, PlayerController);
    }

    return ServerExecuteInventoryItem(Context, Stack, Ret);
}

static void(*RebuildInventoriesIfNeeded)(AFortPlayerController* PlayerController);
void RebuildInventoriesIfNeededHook(AFortPlayerController* PlayerController)
{
    if (!PlayerController->WorldInventory.GetObjectRef())
    {
        FTransform InventoryActorTransform;
        InventoryActorTransform.Scale3D = FVector{ 1,1,1 };
        InventoryActorTransform.Translation = FVector{ 0,0,0 };
        InventoryActorTransform.Rotation = UKismetMathLibrary::Conv_RotatorToQuaternion(FRotator{ 0,0,0 });

        static auto InventoryClass = PlayerController->WorldInventoryClass;

        auto InventoryActor = Cast<AFortInventory>(UGameplayStatics::BeginDeferredActorSpawnFromClass(UWorld::GetWorld(), InventoryClass, InventoryActorTransform, ESpawnActorCollisionHandlingMethod::AlwaysSpawn, PlayerController, ESpawnActorScaleMethod::SelectDefaultAtRuntime));

        TScriptInterface<IFortInventoryInterface> InventoryInterface{};
        InventoryInterface.ObjectPointer = InventoryActor;
        InventoryInterface.InterfacePointer = GetInterfaceAddress(InventoryActor, IFortInventoryInterface::StaticClass());
        SetWorldInventory(PlayerController, InventoryInterface.InterfacePointer);

        if (InventoryActor)
        {
            UGameplayStatics::FinishSpawningActor(InventoryActor, InventoryActorTransform, ESpawnActorScaleMethod::SelectDefaultAtRuntime);
        }
    }

    return RebuildInventoriesIfNeeded(PlayerController);
}

static void(*OnPossess)(AFortPlayerController* PlayerController, APawn* InPawn);
void OnPossessHook(AFortPlayerController* PlayerController, APawn* InPawn)
{

    return OnPossess(PlayerController, InPawn);
}

static void (*ServerCreateBuildingActor)(UObject*, FFrame&, void*);
void ServerCreateBuildingActorHook(UObject* Context, FFrame& Stack, void* Ret)
{
    auto PlayerController = (AFortPlayerControllerAthena*)Context;

    auto Params = (Params::FortPlayerController_ServerCreateBuildingActor*)Stack.Locals;

    if (!PlayerController)
        return ServerCreateBuildingActor(Context, Stack, Ret);

    auto GameState = (AFortGameStateZone*)UWorld::GetWorld()->GameState;

    TSubclassOf<ABuildingSMActor> BuildingClass;
    BuildingClass = Params->CreateBuildingData.BuildingClassData.BuildingClass;

    if (PlayerController->IsA(AFortPlayerControllerAthena::StaticClass()))
    {
        AFortPlayerControllerAthena* Controller = Cast<AFortPlayerControllerAthena>(PlayerController);
        if (Controller->BroadcastRemoteClientInfo && Controller->BroadcastRemoteClientInfo->RemoteBuildableClass.Get())
        {
            BuildingClass = Controller->BroadcastRemoteClientInfo->RemoteBuildableClass;
            Params->CreateBuildingData.BuildingClassData.BuildingClass = BuildingClass.Get();
        }

        if (!BuildingClass && Controller->BroadcastRemoteClientInfo)
        {
            Controller->BroadcastRemoteClientInfo->bActive = true;
            Controller->BroadcastRemoteClientInfo->OnRep_bActive();
        }
    }

    if (!PlayerController->CanAffordToPlaceBuildableClass(Params->CreateBuildingData.BuildingClassData))
    {
            return ServerCreateBuildingActor(Context, Stack, Ret);
    }

    TArray<ABuildingActor*> ExistingBuildings;
    UBuildingStructuralSupportSystem* StructuralSupportSystem;
    UFortKismetLibrary::GetBuildingStructuralSupportSystem(PlayerController->GetWorld(), &StructuralSupportSystem);

    const EFortStructuralGridQueryResults Result = StructuralSupportSystem->CanAddBuildingActorClassToGrid(PlayerController->GetWorld(), BuildingClass, Params->CreateBuildingData.BuildLoc, Params->CreateBuildingData.BuildRot, Params->CreateBuildingData.bMirrored, &ExistingBuildings, nullptr, false);
    if (Result == EFortStructuralGridQueryResults::CanAdd)
    {
        StructuralSupportSystem->StartActorRemovalBatch();
        for (ABuildingActor* BuildingActor : ExistingBuildings)
        {
            BuildingActor->K2_DestroyActor();
        }
        StructuralSupportSystem->StopActorRemovalBatch();

        FTransform BuildingTransform;
        BuildingTransform.Translation.X = Params->CreateBuildingData.BuildLoc.X;
        BuildingTransform.Translation.Y = Params->CreateBuildingData.BuildLoc.Y;
        BuildingTransform.Translation.Z = Params->CreateBuildingData.BuildLoc.Z;
        BuildingTransform.Scale3D.X = 1.0;
        BuildingTransform.Scale3D.Y = 1.0;
        BuildingTransform.Scale3D.Z = 1.0;
        BuildingTransform.Rotation = UKismetMathLibrary::Conv_RotatorToQuaternion(Params->CreateBuildingData.BuildRot);

        auto NewBuildingActor = Cast<ABuildingSMActor>(ABuildingActor::K2_SpawnBuildingActor(PlayerController->GetWorld(), BuildingClass, BuildingTransform, nullptr, nullptr, false, false));
        if (NewBuildingActor != nullptr)
        {
            const uint8 PlayerTeam = UFortKismetLibrary::GetActorTeam(PlayerController);
            NewBuildingActor->SetTeam(PlayerTeam);

            NewBuildingActor->InitializeKismetSpawnedBuildingActor(NewBuildingActor, PlayerController, true, nullptr, false);
            NewBuildingActor->bPlayerPlaced = true;

            UFortItemDefinition* ItemDefinition = UFortKismetLibrary::K2_GetResourceItemDefinition(NewBuildingActor->ResourceType);
            auto ResourceItem = FindInventoryInstance(PlayerController, ItemDefinition);

            if (ResourceItem && !PlayerController->bInfiniteAmmo)
                RemoveItem(PlayerController, ResourceItem->GetItemGuid(), 10);

            auto FortGameMode = (AFortGameMode*)PlayerController->GetWorld()->AuthorityGameMode;

            if (FortGameMode != nullptr)
            {
                FortGameMode->ScoreBuildingConstruction(PlayerController, NewBuildingActor);
            }
        }
    }

    return ServerCreateBuildingActor(Context, Stack, Ret);
}

static void (*ServerBeginEditingBuildingActor)(UObject*, FFrame&, void*);
void ServerBeginEditingBuildingActorHook(UObject* Context, FFrame& Stack, void* Ret)
{
    auto PlayerController = (AFortPlayerController*)Context;

    auto Params = (Params::FortPlayerController_ServerBeginEditingBuildingActor*)Stack.Locals;

    auto BuildingActor = Params->BuildingActorToEdit;

    if (!PlayerController)
        return ServerBeginEditingBuildingActor(Context, Stack, Ret);

    auto PlayerState = Cast<AFortPlayerStateZone>(PlayerController->PlayerState);
    auto Pawn = Cast<AFortPawn>(PlayerController->Pawn);

    auto EditToolItemDefinition = (UFortEditToolItemDefinition*)UKismetSystemLibrary::LoadAsset_Blocking((TSoftObjectPtr<class UObject>)UFortGameData::Get()->EditToolItem);

    auto EditToolItem = FindInventoryInstance(PlayerController, EditToolItemDefinition);

    if(!EditToolItem)
        return ServerBeginEditingBuildingActor(Context, Stack, Ret);

    BuildingActor->SetEditingPlayer(PlayerState);

    EditToolItemDefinition->ServerExecute(EditToolItem, PlayerController);

    return ServerBeginEditingBuildingActor(Context, Stack, Ret);
}

static void (*ServerEditBuildingActor)(AFortPlayerController* PlayerController, ABuildingSMActor* BuildingActorToEdit, TSubclassOf<ABuildingSMActor> NewBuildingClass, uint8 RotationIterations, bool bMirrored);
void ServerEditBuildingActorHook(AFortPlayerController* PlayerController, ABuildingSMActor* BuildingActorToEdit, TSubclassOf<ABuildingSMActor> NewBuildingClass, uint8 RotationIterations, bool bMirrored)
{
    auto Pawn = Cast<AFortPlayerPawn>(PlayerController->Pawn);
    auto PlayerState = (AFortPlayerState*)PlayerController->PlayerState;

    if (!BuildingActorToEdit)
        return ServerEditBuildingActor(PlayerController, BuildingActorToEdit, NewBuildingClass, RotationIterations, bMirrored);

    if (!PlayerState)
        return ServerEditBuildingActor(PlayerController, BuildingActorToEdit, NewBuildingClass, RotationIterations, bMirrored);

    if (!BuildingActorToEdit)
        return;

    BuildingActorToEdit->EditingPlayer = nullptr;

    int32 CurrentBuildingLevel = BuildingActorToEdit->CurrentBuildingLevel;
    
    BuildingActorToEdit->SetEditingPlayer(nullptr);

    if (auto NewBuildingActor = ReplaceBuildingActor(BuildingActorToEdit, 1, NewBuildingClass, CurrentBuildingLevel, RotationIterations, bMirrored, PlayerController))
    {
        NewBuildingActor->bPlayerPlaced = true;
    }
}

static void (*ServerEndEditingBuildingActor)(UObject*, FFrame&, void*);
void ServerEndEditingBuildingActorHook(UObject* Context, FFrame& Stack, void* Ret)
{
    auto PlayerController = (AFortPlayerController*)Context;

    auto Params = (Params::FortPlayerController_ServerEndEditingBuildingActor*)Stack.Locals;

    if (!PlayerController)
        return ServerEndEditingBuildingActor(Context, Stack, Ret);

    auto Pawn = Cast<AFortPawn>(PlayerController->Pawn);

    if (!Pawn)
        return ServerEndEditingBuildingActor(Context, Stack, Ret);

    auto BuildingActor = Params->BuildingActorToStopEditing;

    BuildingActor->SetEditingPlayer(nullptr);

    return ServerEndEditingBuildingActor(Context, Stack, Ret);
}

static void (*ServerDropAllItems)(UObject*, FFrame&, void*);
void ServerDropAllItemsHook(UObject* Context, FFrame& Stack, void* Ret)
{
    auto PlayerController = (AFortPlayerController*)Context;

    auto Params = (Params::FortPlayerController_ServerDropAllItems*)Stack.Locals;

    auto Pawn = (AFortPlayerPawn*)PlayerController->MyFortPawn;

    if (!PlayerController || !Pawn)
        return ServerDropAllItems(Context, Stack, Ret);

    if (Params->IgnoreItemDef == nullptr && GetDefaultObject<UAthenaDeveloperSettings>()->bAllowAthenaRespawning)
        return ServerDropAllItems(Context, Stack, Ret);

    auto Inventory = GetInventoryObject(PlayerController);
    if (!Inventory)
        return ServerDropAllItems(Context, Stack, Ret);

    TArray<UFortWorldItem*> WorldItemToDrops;

    auto& ItemInstances = Inventory->Inventory.ItemInstances;

    for (int i = 0; i < ItemInstances.Num(); i++)
    {
        UFortWorldItem* WorldItem = ItemInstances[i];
        if (!WorldItem) continue;

        FFortItemEntry* ItemEntry = &WorldItem->ItemEntry;
        if (!ItemEntry) continue;

        UFortWorldItemDefinition* ItemDefinition = Cast<UFortWorldItemDefinition>(ItemEntry->ItemDefinition);
        if (!ItemDefinition) continue;

        if (!WorldItem->CanBeDropped() || ItemDefinition == Params->IgnoreItemDef)
            continue;

        WorldItemToDrops.Add(WorldItem);
    }

    const float LootSpawnLocationX = 0.0f;
    const float LootSpawnLocationY = 0.0f;
    const float LootSpawnLocationZ = 70.0f;

    const FVector& SpawnLocation = Pawn->K2_GetActorLocation() +
        Pawn->GetActorForwardVector() * LootSpawnLocationX +
        Pawn->GetActorRightVector() * LootSpawnLocationY +
        Pawn->GetActorUpVector() * LootSpawnLocationZ;

    for (int32 i = 0; i < WorldItemToDrops.Num(); i++)
    {
        UFortWorldItem* WorldItemToDrop = WorldItemToDrops[i];
        if (!WorldItemToDrop) continue;

        FVector FinalLocation = SpawnLocation;
        FVector RandomDirection = UKismetMathLibrary::RandomUnitVector();

        FinalLocation.X += RandomDirection.X * 700.0f;
        FinalLocation.Y += RandomDirection.Y * 700.0f;

        PlayerController->ServerAttemptInventoryDrop(WorldItemToDrop->ItemEntry.ItemGuid, WorldItemToDrop->ItemEntry.Count, false);
    }

    if (WorldItemToDrops.IsValid())
        WorldItemToDrops.Clear();

    return ServerDropAllItems(Context, Stack, Ret);
}

static void (*ServerPlayEmoteItem)(UObject*, FFrame&, void*);
void ServerPlayEmoteItemHook(UObject* Context, FFrame& Stack, void* Ret)
{
    auto PlayerController = (AFortPlayerController*)Context;

    auto Params = (Params::FortPlayerController_ServerPlayEmoteItem*)Stack.Locals;

    auto Pawn = (AFortPlayerPawn*)PlayerController->Pawn;

    auto PlayerState = (AFortPlayerState*)PlayerController->PlayerState;

    UGameplayAbility* AbilityToUse = nullptr;

    if (Params->EmoteAsset->IsA(SDK::UAthenaToyItemDefinition::StaticClass()))
    {
        //do later
    }
    else if (Params->EmoteAsset->IsA(SDK::UAthenaSprayItemDefinition::StaticClass()))
    {
        AbilityToUse = StaticLoadObject<UGameplayAbility>("/Game/Abilities/Sprays/GAB_Spray_Generic.Default__GAB_Spray_Generic_C");
    }

    if (!AbilityToUse)
    {
        AbilityToUse = StaticLoadObject<UGameplayAbility>("/Game/Abilities/Emotes/GAB_Emote_Generic.Default__GAB_Emote_Generic_C");

    }

    if (Params->EmoteAsset->IsA(SDK::UAthenaDanceItemDefinition::StaticClass()))
    {
        auto DanceEmote = (UAthenaDanceItemDefinition*)Params->EmoteAsset;

        Pawn->bEmoteUsesSecondaryFire = DanceEmote->bUsesSecondaryFireInput;
        Pawn->bMovingEmote = DanceEmote->bMovingEmote;
        Pawn->bMovingEmoteSkipLandingFX = DanceEmote->bMovingEmoteSkipLandingFX;
        Pawn->bMovingEmoteForwardOnly = DanceEmote->bMoveForwardOnly;
        Pawn->bMovingEmoteFollowingOnly = DanceEmote->bMoveFollowingOnly;
        Pawn->EmoteWalkSpeed = DanceEmote->WalkForwardSpeed;
    }

    FGameplayAbilitySpec NewSpec = {};

    FGameplayAbilitySpec_Constructor(&NewSpec, AbilityToUse, 1, -1, (UObject*)Params->EmoteAsset);

    FGameplayAbilitySpecHandle handle;

    GiveAbilityAndActivateOnce(PlayerState->AbilitySystemComponent, &handle, &NewSpec, nullptr);

    return ServerPlayEmoteItem(Context, Stack, Ret);
}

static void (*PostInitializeComponents)(class APlayerController* Controller);
void PostInitializeComponentsHook(class APlayerController* Controller)
{
    PostInitializeComponents(Controller);

    // Abort if cheat manager exists or there is no cheat class
    if (Controller->CheatManager || !Controller->CheatClass)
    {
        return;
    }

    auto CheatManager = (UCheatManager*)UGameplayStatics::SpawnObject(Controller->CheatClass, Controller);

    Controller->CheatManager = CheatManager;
    CheatManager->InitCheatManager();
}

static void (*SetPhantomReserveAmmo)(void* InventoryOwner, const FGuid& ItemGuid, int32 Count);
void SetPhantomReserveAmmoHook(void* InventoryOwner, const FGuid& ItemGuid, int32 Count)
{
    AFortPlayerController* PlayerController = AFortPlayerController::GetPlayerControllerFromInventoryOwnerInterface(InventoryOwner);
    AFortInventory* WorldInventory = GetInventoryObject(PlayerController);

    if (!WorldInventory)
        return(SetPhantomReserveAmmo(InventoryOwner, ItemGuid, Count));

    auto ItemInstance = FindItemInstanceByGUID(PlayerController, ItemGuid);

    if (ItemInstance)
    {
        ItemInstance->ItemEntry.PhantomReserveAmmo = Count;
        ItemInstance->ItemEntry.SetToDirty();
    }

    for (int32 i = 0; i < WorldInventory->Inventory.ReplicatedEntries.Num(); i++)
    {
        FFortItemEntry* ReplicatedItemEntry = &WorldInventory->Inventory.ReplicatedEntries[i];

        if (ReplicatedItemEntry->ItemGuid == ItemGuid)
        {
            ReplicatedItemEntry->LoadedAmmo = Count;
            ReplicatedItemEntry->SetToDirty();
            break;
        }
    }

    return(SetPhantomReserveAmmo(InventoryOwner, ItemGuid, Count));
}

static void (*SetLoadedAmmo)(void* InventoryOwner, const FGuid& ItemGuid, int32 Count);
void SetLoadedAmmoHook(void* InventoryOwner, const FGuid& ItemGuid, int32 Count)
{
    AFortPlayerController* PlayerController = AFortPlayerController::GetPlayerControllerFromInventoryOwnerInterface(InventoryOwner);
    AFortInventory* WorldInventory = GetInventoryObject(PlayerController);

    if (!WorldInventory)
        return(SetLoadedAmmo(InventoryOwner, ItemGuid, Count));

    auto ItemInstance = FindItemInstanceByGUID(PlayerController, ItemGuid);

    if (ItemInstance)
    {
        ItemInstance->ItemEntry.LoadedAmmo = Count;
        ItemInstance->ItemEntry.SetToDirty();
    }

    for (int32 i = 0; i < WorldInventory->Inventory.ReplicatedEntries.Num(); i++)
    {
        FFortItemEntry* ReplicatedItemEntry = &WorldInventory->Inventory.ReplicatedEntries[i];

        if (ReplicatedItemEntry->ItemGuid == ItemGuid)
        {
            ReplicatedItemEntry->LoadedAmmo = Count;
            ReplicatedItemEntry->SetToDirty();
            break;
        }
    }

    return(SetLoadedAmmo(InventoryOwner, ItemGuid, Count));
}

static void (*ServerCheat)(AFortPlayerController* PlayerController, const FString& Msg);
void ServerCheatHook(AFortPlayerController* PlayerController, const FString& Msg)
{
    if (PlayerController->CheatManager)
    {
        UKismetSystemLibrary::ExecuteConsoleCommand(PlayerController->GetWorld(), Msg, PlayerController);
    }

    return ServerCheat(PlayerController, Msg);
}

static void (*ServerCheatAll)(AFortPlayerController* PlayerController, const FString& Msg);
void ServerCheatAllHook(AFortPlayerController* PlayerController, const FString& Msg)
{
    if (PlayerController->CheatManager)
    {
        for (auto& PlayerController : UFortKismetLibrary::GetAllFortPlayerControllers(PlayerController->GetWorld(), true, true))
        {
            if (PlayerController)
            {
                UKismetSystemLibrary::ExecuteConsoleCommand(PlayerController->GetWorld(), Msg, PlayerController);
            }
        }
    }

    return ServerCheatAll(PlayerController, Msg);
}

namespace FortPlayerController
{
    void InitHooks()
    {
        auto DefaultFortPlayerController = AFortPlayerController::StaticClass()->DefaultObject;

        auto ServerPlayEmoteItemFn = DefaultFortPlayerController->Class->GetFunction("FortPlayerController", "ServerPlayEmoteItem");
        HookExec(ServerPlayEmoteItemFn, ServerPlayEmoteItemHook, (PVOID*)&ServerPlayEmoteItem);

        auto ServerDropAllItemsFn = DefaultFortPlayerController->Class->GetFunction("FortPlayerController", "ServerDropAllItems");
        HookExec(ServerDropAllItemsFn, ServerDropAllItemsHook, (PVOID*)&ServerDropAllItems);

        auto ServerCreateBuildingActorFn = DefaultFortPlayerController->Class->GetFunction("FortPlayerController", "ServerCreateBuildingActor");
        HookExec(ServerCreateBuildingActorFn, ServerCreateBuildingActorHook, (PVOID*)&ServerCreateBuildingActor);

        auto ServerBeginEditingBuildingActorFn = DefaultFortPlayerController->Class->GetFunction("FortPlayerController", "ServerBeginEditingBuildingActor");
        HookExec(ServerBeginEditingBuildingActorFn, ServerBeginEditingBuildingActorHook, (PVOID*)&ServerBeginEditingBuildingActor);

        auto ServerEndEditingBuildingActorFn = DefaultFortPlayerController->Class->GetFunction("FortPlayerController", "ServerEndEditingBuildingActor");
        HookExec(ServerEndEditingBuildingActorFn, ServerEndEditingBuildingActorHook, (PVOID*)&ServerEndEditingBuildingActor);

        auto ServerExecuteInventoryItemFn = DefaultFortPlayerController->Class->GetFunction("FortPlayerController", "ServerExecuteInventoryItem");
        HookExec(ServerExecuteInventoryItemFn, ServerExecuteInventoryItemHook, (PVOID*)&ServerExecuteInventoryItem);

        auto ServerAttemptInventoryDropFn = DefaultFortPlayerController->Class->GetFunction("FortPlayerController", "ServerAttemptInventoryDrop");
        HookExec(ServerAttemptInventoryDropFn, ServerAttemptInventoryDropHook, (PVOID*)&ServerAttemptInventoryDrop);

        CREATEHOOK(BaseAddress() + 0xC585440, PostInitializeComponentsHook, &PostInitializeComponents);

        CREATEHOOK(BaseAddress() + 0x15271DF0, RemoveInventoryItemHook, &RemoveInventoryItem);
        CREATEHOOK(BaseAddress() + 0x1526C4E0, RebuildInventoriesIfNeededHook, &RebuildInventoriesIfNeeded);
    }
}