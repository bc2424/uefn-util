#pragma once

#include "Includes.h"

static void (*GiveWeapon)(UObject*, FFrame&, void*);
void GiveWeaponHook(UObject* Context, FFrame& Stack, void* Ret)
{
    auto CheatManager = (UFortCheatManager*)Context;
    auto Params = (Params::FortCheatManager_GiveWeapon*)Stack.Locals;

    auto PlayerController = (AFortPlayerController*)CheatManager->GetPlayerController();

    auto Inventory = GetInventoryObject(PlayerController);

    if (PlayerController->GetLocalRole() == ENetRole::ROLE_Authority && Inventory)
    {
        FPrimaryAssetType WeaponAsset = {};
        WeaponAsset.Name = UKismetStringLibrary::Conv_StringToName(L"Weapon");

        FPrimaryAssetId PrimaryAsset = {};
        PrimaryAsset.PrimaryAssetType = WeaponAsset;
        PrimaryAsset.PrimaryAssetName = UKismetStringLibrary::Conv_StringToName(Params->WeaponName);

        auto Item = UKismetSystemLibrary::GetSoftObjectReferenceFromPrimaryAssetId(PrimaryAsset);

        if (UKismetSystemLibrary::IsValidSoftObjectReference(Item))
        {
            auto ItemLoaded = (UFortWeaponItemDefinition*)UKismetSystemLibrary::LoadAsset_Blocking((TSoftObjectPtr<class UObject>)Item);

            if (ItemLoaded->IsA(UFortWeaponItemDefinition::StaticClass()))
            {
                auto LoadedAmmo = 0;
                FFortBaseWeaponStats Stats;

                if (UFortKismetLibrary::GetWeaponStatsRow((ItemLoaded)->WeaponStatHandle, &Stats))
                {
                    LoadedAmmo = Stats.ClipSize;
                }

                AddItem(PlayerController, ItemLoaded, Params->Count, LoadedAmmo);
                PlayerController->ClientMessage(L"Added Weapon Item!", FName{}, 1);
            }
        }
    }

    return GiveWeapon(Context, Stack, Ret);
}

static void (*GiveAthenaGadget)(UObject*, FFrame&, void*);
void GiveAthenaGadgetHook(UObject* Context, FFrame& Stack, void* Ret)
{
    auto CheatManager = (UFortCheatManager*)Context;
    auto Params = (Params::FortCheatManager_GiveAthenaGadget*)Stack.Locals;

    auto PlayerController = (AFortPlayerController*)CheatManager->GetPlayerController();

    auto Inventory = GetInventoryObject(PlayerController);

    if (PlayerController->GetLocalRole() == ENetRole::ROLE_Authority && Inventory)
    {
        FPrimaryAssetType WeaponAsset = {};
        WeaponAsset.Name = UKismetStringLibrary::Conv_StringToName(L"AthenaGadget");

        FPrimaryAssetId PrimaryAsset = {};
        PrimaryAsset.PrimaryAssetType = WeaponAsset;
        PrimaryAsset.PrimaryAssetName = UKismetStringLibrary::Conv_StringToName(Params->GadgetName);

        auto Item = UKismetSystemLibrary::GetSoftObjectReferenceFromPrimaryAssetId(PrimaryAsset);

        if (UKismetSystemLibrary::IsValidSoftObjectReference(Item))
        {
            auto ItemLoaded = (UAthenaGadgetItemDefinition*)UKismetSystemLibrary::LoadAsset_Blocking((TSoftObjectPtr<class UObject>)Item);

            if (ItemLoaded->IsA(UAthenaGadgetItemDefinition::StaticClass()))
            {
                AddItem(PlayerController, ItemLoaded, 1, 999);
                PlayerController->ClientMessage(L"Added Athena Gadget Item!", FName{}, 1);
            }
        }
    }

    return GiveAthenaGadget(Context, Stack, Ret);
}

static void (*GiveGadget)(UObject*, FFrame&, void*);
void GiveGadgetHook(UObject* Context, FFrame& Stack, void* Ret)
{
    auto CheatManager = (UFortCheatManager*)Context;
    auto Params = (Params::FortCheatManager_GiveGadget*)Stack.Locals;

    auto PlayerController = (AFortPlayerController*)CheatManager->GetPlayerController();

    auto Inventory = GetInventoryObject(PlayerController);

    if (PlayerController->GetLocalRole() == ENetRole::ROLE_Authority && Inventory)
    {
        FPrimaryAssetType WeaponAsset = {};
        WeaponAsset.Name = UKismetStringLibrary::Conv_StringToName(L"Gadget");

        FPrimaryAssetId PrimaryAsset = {};
        PrimaryAsset.PrimaryAssetType = WeaponAsset;
        PrimaryAsset.PrimaryAssetName = UKismetStringLibrary::Conv_StringToName(Params->GadgetName);

        auto Item = UKismetSystemLibrary::GetSoftObjectReferenceFromPrimaryAssetId(PrimaryAsset);

        if (UKismetSystemLibrary::IsValidSoftObjectReference(Item))
        {
            auto ItemLoaded = (UFortGadgetItemDefinition*)UKismetSystemLibrary::LoadAsset_Blocking((TSoftObjectPtr<class UObject>)Item);

            if (ItemLoaded->IsA(UFortGadgetItemDefinition::StaticClass()))
            {
                AddItem(PlayerController, ItemLoaded, 1, 999);
                PlayerController->ClientMessage(L"Added Gadget Item!", FName{}, 1);
            }
        }
    }

    return GiveGadget(Context, Stack, Ret);
}

static void (*GiveConsumable)(UObject*, FFrame&, void*);
void GiveConsumableHook(UObject* Context, FFrame& Stack, void* Ret)
{
    auto CheatManager = (UFortCheatManager*)Context;
    auto Params = (Params::FortCheatManager_GiveConsumable*)Stack.Locals;

    auto PlayerController = (AFortPlayerController*)CheatManager->GetPlayerController();

    auto Inventory = GetInventoryObject(PlayerController);

    if (PlayerController->GetLocalRole() == ENetRole::ROLE_Authority && Inventory)
    {
        FPrimaryAssetType WeaponAsset = {};
        WeaponAsset.Name = UKismetStringLibrary::Conv_StringToName(L"Consumable");

        FPrimaryAssetId PrimaryAsset = {};
        PrimaryAsset.PrimaryAssetType = WeaponAsset;
        PrimaryAsset.PrimaryAssetName = UKismetStringLibrary::Conv_StringToName(Params->ConsumableName);

        auto Item = UKismetSystemLibrary::GetSoftObjectReferenceFromPrimaryAssetId(PrimaryAsset);

        if (UKismetSystemLibrary::IsValidSoftObjectReference(Item))
        {
            auto ItemLoaded = (UFortWeaponItemDefinition*)UKismetSystemLibrary::LoadAsset_Blocking((TSoftObjectPtr<class UObject>)Item);

            if (ItemLoaded->IsA(UFortWeaponItemDefinition::StaticClass()))
            {
                auto LoadedAmmo = 0;
                FFortBaseWeaponStats Stats;

                if (UFortKismetLibrary::GetWeaponStatsRow((ItemLoaded)->WeaponStatHandle, &Stats))
                {
                    LoadedAmmo = Stats.ClipSize;
                }

                AddItem(PlayerController, ItemLoaded, Params->Count, LoadedAmmo);
                PlayerController->ClientMessage(L"Added Consumable Item!", FName{}, 1);
            }
        }
    }

    return GiveConsumable(Context, Stack, Ret);
}

static void (*GiveTrap)(UObject*, FFrame&, void*);
void GiveTrapHook(UObject* Context, FFrame& Stack, void* Ret)
{
    auto CheatManager = (UFortCheatManager*)Context;
    auto Params = (Params::FortCheatManager_GiveTrap*)Stack.Locals;

    auto PlayerController = (AFortPlayerController*)CheatManager->GetPlayerController();

    auto Inventory = GetInventoryObject(PlayerController);

    if (PlayerController->GetLocalRole() == ENetRole::ROLE_Authority && Inventory)
    {
        FPrimaryAssetType WeaponAsset = {};
        WeaponAsset.Name = UKismetStringLibrary::Conv_StringToName(L"Trap");

        FPrimaryAssetId PrimaryAsset = {};
        PrimaryAsset.PrimaryAssetType = WeaponAsset;
        PrimaryAsset.PrimaryAssetName = UKismetStringLibrary::Conv_StringToName(Params->TrapName);

        auto Item = UKismetSystemLibrary::GetSoftObjectReferenceFromPrimaryAssetId(PrimaryAsset);

        if (UKismetSystemLibrary::IsValidSoftObjectReference(Item))
        {
            auto ItemLoaded = (UFortTrapItemDefinition*)UKismetSystemLibrary::LoadAsset_Blocking((TSoftObjectPtr<class UObject>)Item);

            if (ItemLoaded->IsA(UFortTrapItemDefinition::StaticClass()))
            {
                AddItem(PlayerController, ItemLoaded, Params->NumTrapsToGive, 1);
                PlayerController->ClientMessage(L"Added Trap Item!", FName{}, 1);
            }
        }
    }

    return GiveTrap(Context, Stack, Ret);
}

static void (*GiveSpecificItem)(UObject*, FFrame&, void*);
void GiveSpecificItemHook(UObject* Context, FFrame& Stack, void* Ret)
{
    auto CheatManager = (UFortCheatManager*)Context;
    auto Params = (Params::FortCheatManager_GiveSpecificItem*)Stack.Locals;

    auto PlayerController = (AFortPlayerController*)CheatManager->GetPlayerController();

    auto Inventory = GetInventoryObject(PlayerController);

    if (PlayerController->GetLocalRole() == ENetRole::ROLE_Authority && Inventory)
    {
        auto AssetString = Params->ItemPath;
        auto AssetPath = UKismetSystemLibrary::MakeSoftObjectPath(AssetString);

        auto SoftObjectAsset = UKismetSystemLibrary::Conv_SoftObjPathToSoftObjRef(AssetPath);

        if (UKismetSystemLibrary::IsValidSoftObjectReference(SoftObjectAsset))
        {
            auto ItemLoaded = (UFortWorldItemDefinition*)UKismetSystemLibrary::LoadAsset_Blocking((TSoftObjectPtr<class UObject>)SoftObjectAsset);
            auto LoadedAmmo = 0;

            if (!ItemLoaded->IsA(UFortWorldItemDefinition::StaticClass()))
                return GiveSpecificItem(Context, Stack, Ret);

            if (ItemLoaded->IsA(UFortWeaponItemDefinition::StaticClass()))
            {
                auto ItemLoadedWeapon = (UFortWeaponItemDefinition*)ItemLoaded;
                FFortBaseWeaponStats Stats;

                if (UFortKismetLibrary::GetWeaponStatsRow((ItemLoadedWeapon)->WeaponStatHandle, &Stats))
                {
                    LoadedAmmo = Stats.ClipSize;
                }
            }

            AddItem(PlayerController, ItemLoaded, Params->ItemAmount, LoadedAmmo);
            PlayerController->ClientMessage(L"Added Specific Item!", FName{}, 1);
        }
    }

    return GiveSpecificItem(Context, Stack, Ret);
}

static void (*ToggleInfiniteAmmo)(UObject*, FFrame&, void*);
void ToggleInfiniteAmmoHook(UObject* Context, FFrame& Stack, void* Ret)
{
    auto CheatManager = (UFortCheatManager*)Context;

    auto PlayerController = (AFortPlayerControllerAthena*)CheatManager->GetPlayerController();

    if (PlayerController && PlayerController->GetLocalRole() == ENetRole::ROLE_Authority)
    {
        if (!PlayerController->bInfiniteAmmo)
        {
            PlayerController->SetHasInfiniteAmmo(true);
            PlayerController->ClientMessage(L"Infinite Ammo and Free Build on", FName{}, 1);
        }
        else {
            PlayerController->SetHasInfiniteAmmo(false);
            PlayerController->ClientMessage(L"Infinite Ammo and Free Build off", FName{}, 1);
        }
    }

    return ToggleInfiniteAmmo(Context, Stack, Ret);
}

static void (*SetShieldPercent)(UObject*, FFrame&, void*);
void SetShieldPercentHook(UObject* Context, FFrame& Stack, void* Ret)
{
    auto CheatManager = (UFortCheatManager*)Context;
    auto Params = (Params::FortCheatManager_SetShieldPercent*)Stack.Locals;

    auto PlayerController = (AFortPlayerController*)CheatManager->GetPlayerController();
    auto Pawn = PlayerController->MyFortPawn;

    if (PlayerController->GetLocalRole() == ENetRole::ROLE_Authority)
    {
        Pawn->SetMaxShield(Params->Percent);
        Pawn->SetShield(Params->Percent);
        PlayerController->ClientMessage(L"Set Shield!", FName{}, 1);
    }

    return SetShieldPercent(Context, Stack, Ret);
}

static void (*SetHealthPercent)(UObject*, FFrame&, void*);
void SetHealthPercentHook(UObject* Context, FFrame& Stack, void* Ret)
{
    auto CheatManager = (UFortCheatManager*)Context;
    auto Params = (Params::FortCheatManager_SetHealthPercent*)Stack.Locals;

    auto PlayerController = (AFortPlayerController*)CheatManager->GetPlayerController();
    auto Pawn = PlayerController->MyFortPawn;

    if (PlayerController->GetLocalRole() == ENetRole::ROLE_Authority)
    {
        Pawn->SetMaxHealth(Params->Percent);
        Pawn->SetHealth(Params->Percent);
        PlayerController->ClientMessage(L"Set Health!", FName{}, 1);
    }

    return SetHealthPercent(Context, Stack, Ret);
}

namespace FortCheatManager
{
    void InitHooks()
    {
        auto DefaultFortCheatManager = UFortCheatManager::StaticClass()->DefaultObject;

        auto ToggleInfiniteAmmoFn = DefaultFortCheatManager->Class->GetFunction("FortCheatManager", "ToggleInfiniteAmmo");
        HookExec(ToggleInfiniteAmmoFn, ToggleInfiniteAmmoHook, (PVOID*)&ToggleInfiniteAmmo);

        auto GiveWeaponFn = DefaultFortCheatManager->Class->GetFunction("FortCheatManager", "GiveWeapon");
        HookExec(GiveWeaponFn, GiveWeaponHook, (PVOID*)&GiveWeapon);

        auto GiveAthenaGadgetFn = DefaultFortCheatManager->Class->GetFunction("FortCheatManager", "GiveAthenaGadget");
        HookExec(GiveAthenaGadgetFn, GiveAthenaGadgetHook, (PVOID*)&GiveAthenaGadget);

        auto GiveGadgetFn = DefaultFortCheatManager->Class->GetFunction("FortCheatManager", "GiveGadget");
        HookExec(GiveGadgetFn, GiveGadgetHook, (PVOID*)&GiveGadget);

        auto GiveConsumableFn = DefaultFortCheatManager->Class->GetFunction("FortCheatManager", "GiveConsumable");
        HookExec(GiveConsumableFn, GiveConsumableHook, (PVOID*)&GiveConsumable);

        auto GiveSpecificItemFn = DefaultFortCheatManager->Class->GetFunction("FortCheatManager", "GiveSpecificItem");
        HookExec(GiveSpecificItemFn, GiveSpecificItemHook, (PVOID*)&GiveSpecificItem);

        auto GiveTrapFn = DefaultFortCheatManager->Class->GetFunction("FortCheatManager", "GiveTrap");
        HookExec(GiveTrapFn, GiveTrapHook, (PVOID*)&GiveTrap);

        auto SetHealthPercentFn = DefaultFortCheatManager->Class->GetFunction("FortCheatManager", "SetHealthPercent");
        HookExec(SetHealthPercentFn, SetHealthPercentHook, (PVOID*)&SetHealthPercent);

        auto SetShieldPercentFn = DefaultFortCheatManager->Class->GetFunction("FortCheatManager", "SetShieldPercent");
        HookExec(SetShieldPercentFn, SetShieldPercentHook, (PVOID*)&SetShieldPercent);
    }
}