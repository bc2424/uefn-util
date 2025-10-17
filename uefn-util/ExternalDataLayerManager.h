#pragma once

#include "Includes.h"

static FString* (*GetExternalStreamingObjectPackageName)(const FString* a2, const UExternalDataLayerAsset* a3) = decltype(GetExternalStreamingObjectPackageName)(0xCB82A90 + uintptr_t(GetModuleHandle(0)));

static bool (*InjectIntoGameWorld)(UExternalDataLayerManager* DataLayerManager, const UExternalDataLayerAsset* InExternalDataLayerAsset);
bool InjectIntoGameWorldHook(UExternalDataLayerManager* DataLayerManager, const UExternalDataLayerAsset* InExternalDataLayerAsset)
{
    //removed

    return InjectIntoGameWorld(DataLayerManager, InExternalDataLayerAsset);
}

namespace ExternalDataLayerManager
{
    void InitHooks()
    {
        //CREATEHOOK(BaseAddress() + 0xCB8DE60, InjectIntoGameWorldHook, &InjectIntoGameWorld);
    }
}