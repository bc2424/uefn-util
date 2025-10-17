#pragma once

#include "Includes.h"

static bool (*ServerTravel)(UWorld*, FString& FURL, bool bAbsolute, bool bShouldSkipGameNotify) = decltype(ServerTravel)(BaseAddress() + 0xCB22610);
static bool (*MakeSureMapNameIsValid)(UEngine*, FString&) = decltype(MakeSureMapNameIsValid)(BaseAddress() + 0xCA48380);
static bool (*Exec)(UEngine* World, UWorld* InWorld, const TCHAR* Cmd, __int64& Ar);
static bool (*FParse_Command)(const TCHAR** Stream, const TCHAR* Match, bool bParseMightTriggerExecution) = decltype(FParse_Command)(BaseAddress() + 0xD29C7C0);
static bool (*FParse_Token)(const TCHAR*& Str, FString& Arg, bool bUseEscape) = decltype(FParse_Token)(BaseAddress() + 0xD2BE550);

static bool (*IsServer)(UNetDriver* NetDriver) = decltype(IsServer)(BaseAddress() + 0xC2E41E0);


static bool (*Listen)(UWorld*, FURL&);
bool ListenHook(UWorld* World, FURL& InURL)
{
    UEngine* GEngine = UEngine::GetEngine();

    if (World->NetDriver)
    {
        return false;
    }

    static auto NAME_GameNetDriver = UKismetStringLibrary::Conv_StringToName(L"GameNetDriver");

    World->NetDriver = CreateNetDriver(UEngine::GetEngine(), World, NAME_GameNetDriver);

    if (World->NetDriver)
    {
        World->NetDriver->World = World;
        World->NetDriver->NetDriverName = NAME_GameNetDriver;

        auto URL = InURL;
        URL.Port = GetDefaultObject<ULevelEditorPlaySettings>()->ServerPort;

        FString Error;

        InitListen(World->NetDriver, World, URL, true, Error);
        SetWorld(World->NetDriver, World);

        UNetDriver* NetDriver = World->NetDriver;

        FLevelCollection* const SourceCollection = FindCollectionByType(World, 0);
        if (SourceCollection)
        {
            SourceCollection->NetDriver = NetDriver;
        }
        FLevelCollection* const StaticCollection = FindCollectionByType(World, 2);
        if (StaticCollection)
        {
            StaticCollection->NetDriver = NetDriver;
        }

        if (NetDriver->MaxInternetClientRate < NetDriver->MaxClientRate)
        {
            NetDriver->MaxClientRate = 200000;
        }

    }

    //World->NextSwitchCountdown = World->NetDriver->ServerTravelPause;

    return true;
}

bool HandleServerTravelCommand(const TCHAR* Cmd, __int64& Ar, UWorld* InWorld)
{
    FString MapName(Cmd);

    if (MakeSureMapNameIsValid(UEngine::GetEngine(), MapName))
    {
        ServerTravel(InWorld, MapName, false, false);
        return true;
    }

    return false;
}

bool ExecHook(UEngine* Engine, UWorld* InWorld, const TCHAR* Cmd, __int64& Ar)
{
    if (FParse_Command(&Cmd, TEXT("SERVERTRAVEL"), false))
    {
        return HandleServerTravelCommand(Cmd, Ar, InWorld);
    }

    return Exec(Engine, InWorld, Cmd, Ar);
}

namespace World
{
	void InitHooks()
	{
		CREATEHOOK(BaseAddress() + 0xCB0CE70, ListenHook, &Listen);
        CREATEHOOK(BaseAddress() + 0xCA29E00, ExecHook, &Exec);
	}
}