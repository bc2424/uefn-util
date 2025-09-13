#pragma once

#include "Includes.h"

#include "CoreGlobals.h"
#include "PluginManager.h"
#include "NetDriver.h"
#include "AbilitySystemComponent_Abilities.h"
#include "World.h"
#include "GameViewportClient.h"

#include "FortPlayerController.h"
#include "FortPlayerControllerAthena.h"
#include "FortGameModeAthena.h"
#include "FortGameStateComponent_BattleRoyaleGamePhaseLogic.h"
#include "FortGameModeBR.h"
#include "FortPlayerPawn.h"
#include "FortCheatManager.h"
#include "FortItemDefinition.h"
#include "FortPickup.h"
#include "ObjectReplicationBridge.h"

static inline void (*RefreshBakedData)(UPaperSprite*);
static void RefreshBakedDataHook(UPaperSprite*) { return; }

static __int64 (*DispatchRequest)(__int64 a1, __int64* a2, int a3);
static __int64 DispatchRequestHook(__int64 a1, __int64* a2, int a3)
{
    return DispatchRequest(a1, a2, 3);
}

static inline void (*SetEngineStartupModuleLoadingComplete)();
void SetEngineStartupModuleLoadingCompleteHook()
{
    //server hooks
    NetDriver::InitHooks();
    AbilitySystemComponent::InitHooks();
    World::InitHooks();

    //mcp
    CREATEHOOK(BaseAddress() + 0xEB98560, DispatchRequestHook, &DispatchRequest);

    //funny hook
    CREATEHOOK(BaseAddress() + 0x14BA96B0, KickPlayerHook, &KickPlayer); //fortgamesession::kickplayer

    //uefn hooks
    CREATEHOOK(BaseAddress() + 0x11C02B70, RefreshBakedDataHook, &RefreshBakedData);
    GameViewportClient::InitHooks();

    //server code
    FortPlayerController::InitHooks();
    //FortPlayerControllerZone::InitHooks(); // no hooks rn
    FortPlayerControllerAthena::InitHooks();
    FortGameModeBR::InitHooks();
    FortPawn::InitHooks();
    FortPlayerPawn::InitHooks();
    FortItemDefinition::InitHooks();
    FortPlayerPawnAthena::InitHooks();
    FortCheatManager::InitHooks();
    FortPickup::InitHooks();
    FortPickupAthena::InitHooks();
    //FortGameModePvE::InitHooks(); // no stw rn....
    //FortGameStateAthena::InitHooks(); // obsolete
    FortGameStateComponent_BattleRoyaleGamePhaseLogic::InitHooks();
    //FortWeapon::InitHooks(); // unneeded atm
    //ObjectReplicationBridge::InitHooks();

    //uefn memory patching
    WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(BaseAddress() + 0x106ACCBA), "\x90\x90", 2, NULL); //cannot modify cooked
    WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(BaseAddress() + 0x106ACCB1), "\xeb", 1, NULL);
    WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(BaseAddress() + 0xA0F69E8), "\xb3\x01", 2, NULL); //unable to edit
    WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(BaseAddress() + 0x9E96FBF), "\x90\x90\x90\x90\x90\x90", 6, NULL); //copy objects
    WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(BaseAddress() + 0x9E975D5), "\xeb", 1, NULL); //package is cooked or...
    WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(BaseAddress() + 0x786C94E), "\x90\x90\x90\x90\x90\x90", 6, NULL); //the requested operation could not... I don't use this one lol
    WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(BaseAddress() + 0x9548037), "\xe9\xa5\x00", 3, NULL); //
    WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(BaseAddress() + 0xFF0AE57), "\xeb", 1, NULL); //the binding is from a different library
    WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(BaseAddress() + 0xFF0E353), "\xeb", 1, NULL);
    WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(BaseAddress() + 0xEFA3A6C), "\x30\xc0", 2, NULL); //pie server connect
    WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(BaseAddress() + 0x103AA4FA), "\x90\x90\x90\x90\x90", 5, NULL); //proxy table anims

    WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(BaseAddress() + 0x1EA16AD0), "\xc3", 1, NULL); //valkyrie validation thing
    WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(BaseAddress() + 0xC3027C9), "\x90\x90\x90\x90\x90\xe9\x00\x02\x00", 9, NULL); //no net connection timeout, maybe do this w a proper hook?
    WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(BaseAddress() + 0xC302809), "\xe9\xc5\x01\x00", 4, NULL); //another net timeout patch smth calls this IDK
    WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(BaseAddress() + 0xB13FB32), "\xe9\xe8\x02\x00", 4, NULL); //helios crash
    WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(BaseAddress() + 0x18F26189), "\xe9\xed\x01\x00", 4, NULL); //conversations
    WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(BaseAddress() + 0x47FEB20), "\xc3", 1, NULL); //slate vector art validate

    //blueprint patching
    WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(BaseAddress() + 0x7C9A08B), "\xb0\x01", 2, NULL); //make struct
    WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(BaseAddress() + 0x7C152C1), "\xb0\x01", 2, NULL); //break struct
    WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(BaseAddress() + 0x7C5F60A), "\xe9\x0b\x03\x00", 4, NULL); //cannot construct object of type
    WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(BaseAddress() + 0x7D69024), "\xe9\x50\x02\x00", 4, NULL); //cannot be overriden/placed as event
    WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(BaseAddress() + 0x7D1616D), "\xe9\xdc\x06\x00", 4, NULL); //is not blueprint writable
    WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(BaseAddress() + 0x7D55DBC), "\xe9\xc5\x00", 3, NULL); //net flag's don't match
    WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(BaseAddress() + 0x7D69504), "\xe9\x4c\x01\x00", 4, NULL); //the function name in node is already used
    WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(BaseAddress() + 0x7D15F26), "\xe9\xb7\x00", 3, NULL); //is not blueprint visible
    WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(BaseAddress() + 0x7D55905), "\xe9\x7c\x05\x00", 4, NULL); //cannot override parent signature
    WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(BaseAddress() + 0x7D55010), "\xeb", 1, NULL); //specifier
    WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(BaseAddress() + 0x7BD1F5D), "\xe9\x15\x03\x00", 4, NULL); //cannot override parent signature
    WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(BaseAddress() + 0x7D1BD94), "\xe9\xac\x03\x00", 4, NULL); //cannot write to const
    WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(BaseAddress() + 0x7D1CD5B), "\xe9\x97\x00", 3, NULL); //blueprinttype structure

    //killing MMEs until later
    WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(BaseAddress() + 0x14A63619), "\xe9\x82\x02\x00", 4, NULL); //force turn off
    WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(BaseAddress() + 0x14A638A7), "\xeb", 1, NULL); //log too
    UKismetSystemLibrary::ExecuteConsoleCommand(UWorld::GetWorld(), L"Fort.MME.TacticalSprint 0", nullptr); //temp
    UKismetSystemLibrary::ExecuteConsoleCommand(UWorld::GetWorld(), L"Fort.MME.Sliding 0", nullptr); //temp

    WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(BaseAddress() + 0xB815A13), "\xeb", 1, NULL); //animations

    // lol
    UKismetSystemLibrary::ExecuteConsoleCommand(UWorld::GetWorld(), L"a.Fort.EnableOldLocomotionData 1", nullptr);

    //preferred cvars
    UKismetSystemLibrary::ExecuteConsoleCommand(UWorld::GetWorld(), L"net.Iris.EnableFilterMappings 0", nullptr);
    UKismetSystemLibrary::ExecuteConsoleCommand(UWorld::GetWorld(), L"SupervisedSettings.UseEOSIntegration 0", nullptr);
    UKismetSystemLibrary::ExecuteConsoleCommand(UWorld::GetWorld(), L"log LogEOSSDK off", nullptr);
    UKismetSystemLibrary::ExecuteConsoleCommand(UWorld::GetWorld(), L"log LogUObjectBase off", nullptr);

    MH_EnableHook(MH_ALL_HOOKS);

    return SetEngineStartupModuleLoadingComplete();
}