#pragma once

#include "Includes.h"

class FOutputDevice;

static bool (*ServerTravel)(UWorld*, FString& FURL, bool bAbsolute, bool bShouldSkipGameNotify) = decltype(ServerTravel)(BaseAddress() + 0xCB22610);
static bool (*MakeSureMapNameIsValid)(UEngine*, FString&) = decltype(MakeSureMapNameIsValid)(BaseAddress() + 0xCA48380);
static bool (*Exec)(UWorld* World, UWorld* InWorld, const TCHAR* Cmd, __int64& Ar);
static bool (*FParse_Command)(const TCHAR** Stream, const TCHAR* Match, bool bParseMightTriggerExecution) = decltype(FParse_Command)(BaseAddress() + 0xD29C7C0);
static bool (*FParse_Token)(const TCHAR*& Str, FString& Arg, bool bUseEscape) = decltype(FParse_Token)(BaseAddress() + 0xD2BE550);

static bool (*Listen)(FURL&);
bool ListenHook(FURL& InURL)
{
    UEngine* GEngine = UEngine::GetEngine();

    if (UWorld::GetWorld()->NetDriver)
    {
        return false;
    }

    static auto NAME_GameNetDriver = UKismetStringLibrary::Conv_StringToName(L"GameNetDriver");

    UWorld::GetWorld()->NetDriver = CreateNetDriver(UEngine::GetEngine(), UWorld::GetWorld(), NAME_GameNetDriver);

    if (UWorld::GetWorld()->NetDriver)
    {
        UWorld::GetWorld()->NetDriver->World = UWorld::GetWorld();
        UWorld::GetWorld()->NetDriver->NetDriverName = NAME_GameNetDriver;

        auto URL = InURL;
        URL.Port = GetDefaultObject<ULevelEditorPlaySettings>()->ServerPort;

        FString Error;

        InitListen(UWorld::GetWorld()->NetDriver, UWorld::GetWorld(), URL, true, Error);
        SetWorld(UWorld::GetWorld()->NetDriver, UWorld::GetWorld());

        UNetDriver* NetDriver = UWorld::GetWorld()->NetDriver;

        FLevelCollection* const SourceCollection = FindCollectionByType(UWorld::GetWorld(), 0);
        if (SourceCollection)
        {
            SourceCollection->NetDriver = NetDriver;
        }
        FLevelCollection* const StaticCollection = FindCollectionByType(UWorld::GetWorld(), 2);
        if (StaticCollection)
        {
            StaticCollection->NetDriver = NetDriver;
        }

        if (NetDriver->MaxInternetClientRate < NetDriver->MaxClientRate)
        {
            NetDriver->MaxClientRate = 200000;
        }

    }

    return true;
}

bool HandleServerTravelCommand(const TCHAR* Cmd)
{
    FString MapName;
    FParse_Token(Cmd, MapName, 0);

    if (MakeSureMapNameIsValid(UEngine::GetEngine(), MapName))
    {
        ServerTravel(UWorld::GetWorld(), MapName, true, false);
        return true;
    }
    else
    {
        return false;
    }
}

bool ExecHook(UWorld* World, UWorld* InWorld, const TCHAR* Cmd, __int64& Ar)
{
    if (FParse_Command(&Cmd, TEXT("SERVERTRAVEL"), false))
    {
        return HandleServerTravelCommand(Cmd);
    }

    return Exec(World, InWorld, Cmd, Ar);
}

namespace World
{
	void InitHooks()
	{
		CREATEHOOK(BaseAddress() + 0xCB0CE70, ListenHook, &Listen);
        CREATEHOOK(BaseAddress() + 0xCAFC7C0, ExecHook, &Exec);
	}
}