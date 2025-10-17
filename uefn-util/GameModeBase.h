#pragma once

#include "Includes.h"

static void (*ProcessServerTravel)(class AGameModeBase* GameMode, FString& URL, bool bAbsolute);
void ProcessServerTravelHook(class AGameModeBase* GameModeBase, FString& URL, bool bAbsolute)
{
	if (GameModeBase->IsA(AGameMode::StaticClass()))
	{
		auto GameMode = (AGameMode*)GameModeBase;
		GameMode->StartToLeaveMap();
	}

	UWorld* World = GameModeBase->GetWorld();
	auto GEngine = UEngine::GetEngine();

	FWorldContext& WorldContext = GEngine->GetWorldContextFromWorldChecked(World);

	// Use game mode setting but default to full load screen if the server has been up for a long time so that TimeSeconds doesn't overflow and break everything
	bool bSeamless = false;

	APlayerController* LocalPlayer = GameModeBase->ProcessClientTravel(URL, bSeamless, bAbsolute);

	if (GameModeBase->bUseSeamlessTravel)
	{
		World->SeamlessTravel(URL, bAbsolute);
	}

	//GEngine->IncrementGlobalNetTravelCount();
	//GEngine->SaveConfig();

	return ProcessServerTravel(GameModeBase, URL, bAbsolute);
}

namespace GameModeBase
{
	void InitHooks()
	{
		auto GameModeBase = AGameModeBase::StaticClass()->DefaultObject;

		CREATEHOOK(BaseAddress() + 0xBDC7FB0, ProcessServerTravelHook, &ProcessServerTravel);
	}
}