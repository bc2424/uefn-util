#pragma once

#include "Includes.h"

static bool (*RequestLevel)(UWorldPartitionLevelStreamingDynamic* LevelDynamic, UWorld* InPersistentWorld, bool bInAllowLevelLoadRequests, byte InBlockPolicy);
bool RequestLevelHook(UWorldPartitionLevelStreamingDynamic* LevelDynamic, UWorld* InPersistentWorld, bool bInAllowLevelLoadRequests, byte InBlockPolicy)
{
    LevelDynamic->bShouldPerformStandardLevelLoading = true;
	return RequestLevel(LevelDynamic, InPersistentWorld, bInAllowLevelLoadRequests, InBlockPolicy);
}

static int32(*HasActors)(UWorldPartitionRuntimeLevelStreamingCell* LevelStreamingCell);
int32 HasActorsHook(UWorldPartitionRuntimeLevelStreamingCell* LevelStreamingCell)
{
    if (HasActors(LevelStreamingCell) == 0)
        return 69420;

    else
        return HasActors(LevelStreamingCell);
}

namespace WorldPartitionLevelStreamingDynamic
{
    void InitHooks()
    {
        CREATEHOOK(BaseAddress() + 0xCC47380, RequestLevelHook, &RequestLevel);
        CREATEHOOK(BaseAddress() + 0xCCA79B0, HasActorsHook, &HasActors);
    }
}