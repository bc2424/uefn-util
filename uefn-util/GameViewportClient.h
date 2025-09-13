#pragma once

#include "Includes.h"

static ULocalPlayer* (*SetupInitialLocalPlayer)(UGameViewportClient* GameViewportClient, FString& OutError);
ULocalPlayer* SetupInitialLocalPlayerHook(UGameViewportClient* GameViewportClient, FString& OutError)
{
    auto Console = (UConsole*)UGameplayStatics::SpawnObject(UEngine::GetEngine()->ConsoleClass, GameViewportClient);

    if (Console)
    {
        GameViewportClient->ViewportConsole = Console;
    }

    return(SetupInitialLocalPlayer)(GameViewportClient, OutError);
}

namespace GameViewportClient
{
    void InitHooks()
    {
        CREATEHOOK(BaseAddress() + 0xBE58E90, SetupInitialLocalPlayerHook, &SetupInitialLocalPlayer);
    }
}