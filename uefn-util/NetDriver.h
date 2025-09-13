#pragma once

#include "Includes.h"

enum class EReplicationSystemSendPass : unsigned
{
    Invalid,

    // Sending data directly after PostDispatch, this is a partial update only, not updating scope and filtering and will only process RPC/Attachmeents
    PostTickDispatch,

    // Sending data part of TickFlush, this will do a full update and replicate data to all connections
    TickFlush,
};

struct FSendUpdateParams
{
    // Type of SendPass we want to do @see EReplicationSystemSendPass
    EReplicationSystemSendPass SendPass = EReplicationSystemSendPass::TickFlush;

    // DeltaTime, only relevant for EReplicationSystemSendPass::TickFlush
    //float* DeltaSeconds = 0.f;
    float value = 0.f;
    float* DeltaSeconds = &value;
};

static void (*NetDriverForcePropertyCompare)(UNetDriver*, AActor*);

//a lot of these I think are in the wrong location but lol xd

static void (*KickPlayer)(AGameSession*, AController*);
static void KickPlayerHook(AGameSession*, AController*) { return; }

static UNetDriver* (*FindNamedNetDriver)(UEngine*, UWorld*, FName) = decltype(FindNamedNetDriver)(BaseAddress() + 0xCA2C7D0);
static bool (*CreateNamedNetDriver)(UObject*, UWorld*, FName, FName) = decltype(CreateNamedNetDriver)(BaseAddress() + 0xCA22180);
static FLevelCollection* (*FindCollectionByType)(UWorld*, byte) = decltype(FindCollectionByType)(BaseAddress() + 0xCAFEEA0);
static bool (*InitListen)(UNetDriver*, void*, FURL&, bool, FString& Error) = decltype(InitListen)(BaseAddress() + 0xD41CB00);
static void (*SetWorld)(UNetDriver*, UWorld*) = decltype(SetWorld)(BaseAddress() + 0xC2FD8C0);
static void (*InitHost)(AOnlineBeaconHost*) = decltype(InitHost)(BaseAddress() + 0xD41C660);
static void (*PauseBeaconRequests)(AOnlineBeacon*, bool) = decltype(PauseBeaconRequests)(BaseAddress() + 0x14BB3930);
static UNetDriver* (*CreateNetDriver)(UEngine*, UWorld*, FName) = decltype(CreateNetDriver)(BaseAddress() + 0xCA22490);
static UReplicationSystem* (*GetReplicationSystem)(AActor* Actor) = decltype(GetReplicationSystem)(BaseAddress() + 0xC34DDD0);

static void (*UpdateIrisReplicationViews)(UNetDriver*) = decltype(UpdateIrisReplicationViews)(BaseAddress() + 0xC307EC0);

static void (*PreSendUpdate)(UReplicationSystem*, FSendUpdateParams) = decltype(PreSendUpdate)(BaseAddress() + 0x2F5AD10);

static void (*SendClientAdjustment)(APlayerController*) = decltype(SendClientAdjustment)(BaseAddress() + 0xC58DDC0);
void SendClientMoveAdjustments()
{
    for (UNetConnection* Connection : UWorld::GetWorld()->NetDriver->ClientConnections)
    {
        if (Connection == nullptr || Connection->ViewTarget == nullptr)
        {
            continue;
        }

        if (APlayerController* PlayerController = Connection->PlayerController)
        {
            SendClientAdjustment(PlayerController);
        }

        for (UNetConnection* ChildConnection : Connection->Children)
        {
            if (ChildConnection == nullptr)
            {
                continue;
            }

            if (APlayerController* PlayerController = ChildConnection->PlayerController)
            {
                SendClientAdjustment(PlayerController);
            }
        }
    }
}

static void (*TickFlush)(UNetDriver*, float*);
void TickFlushHook(UNetDriver* NetDriver, float* DeltaSeconds)
{
    if (UWorld::GetWorld()->NetDriver)
    {
        if (!UWorld::GetWorld()->NetDriver->ServerConnection && UWorld::GetWorld()->NetDriver->ClientConnections.Num() > 0)
        {
            UReplicationSystem* ReplicationSystem = GetReplicationSystem(UWorld::GetWorld()->K2_GetWorldSettings());

            if (ReplicationSystem) {
                UpdateIrisReplicationViews(UWorld::GetWorld()->NetDriver);
                SendClientMoveAdjustments(); //later
                PreSendUpdate(ReplicationSystem, FSendUpdateParams{ .SendPass = EReplicationSystemSendPass::TickFlush, .DeltaSeconds = DeltaSeconds });
            }
        }
    }
    return TickFlush(NetDriver, DeltaSeconds);
}


namespace NetDriver
{
    void InitHooks()
    {
        CREATEHOOK(BaseAddress() + 0xC303160, TickFlushHook, &TickFlush);
    }
}