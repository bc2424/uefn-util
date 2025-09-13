#pragma once

#include "Includes.h"

enum class EConsiderPending : uint8
{
	/** Don't consider any Pending actions (such as Pending Abilities Added or Removed) */
	None = 0,

	/** Consider Pending Adds when performing the action */
	PendingAdd = (1 << 0),

	/** Consider Pending Removes when performing the action */
	PendingRemove = (1 << 1),

	All = PendingAdd | PendingRemove
};


static bool (*InternalTryActivateAbility)(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAbilitySpecHandle Handle, FPredictionKey InPredictionKey, UGameplayAbility** OutInstancedAbility, void* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData) = decltype(InternalTryActivateAbility)(BaseAddress() + 0xEA2BB70);
static FGameplayAbilitySpecHandle(*GiveAbility)(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayAbilitySpec& Spec) = decltype(GiveAbility)(BaseAddress() + 0xEA24C50);
static FGameplayAbilitySpec* (*FindAbilitySpecFromHandle)(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAbilitySpecHandle Handle, int ConsiderPending) = decltype(FindAbilitySpecFromHandle)(BaseAddress() + 0xEA1AF40);

static FGameplayAbilitySpecHandle(*GiveAbilityAndActivateOnce)(UAbilitySystemComponent* AbilitySystemComponent, void* outHandle, void* Spec, FGameplayEventData* TriggerEventData) = decltype(GiveAbilityAndActivateOnce)(BaseAddress() + 0xEA252B0);

static __int64 (*FGameplayAbilitySpec_Constructor)(void* Spec, void* InAbility, char InLevel, int InInputID, void* InSourceObject) = decltype(FGameplayAbilitySpec_Constructor)(BaseAddress() + 0xE9FFAB0);

//static FGameplayAbilitySpec(*BuildAbilitySpecFromClass)(UAbilitySystemComponent*, TSubclassOf<UClass>, int, int) = decltype(BuildAbilitySpecFromClass)(BaseAddress() + 0xD963E40);

static void (*InternalServerTryActivateAbility)(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAbilitySpecHandle Handle, bool InputPressed, const FPredictionKey& PredictionKey, FGameplayEventData* TriggerEventData);
void InternalServerTryActivateAbilityHook(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAbilitySpecHandle Handle, bool InputPressed, const FPredictionKey& PredictionKey, FGameplayEventData* TriggerEventData)
{

	/*if (DenyClientActivation > 0)
	{
		DenyClientActivation--;
		AbilitySystemComponent->ClientActivateAbilityFailed(Handle, PredictionKey.Current);
		return;
	}*/

	FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(AbilitySystemComponent, Handle, 2);
	if (!Spec)
	{
		// Can potentially happen in race conditions where client tries to activate ability that is removed server side before it is received.
		AbilitySystemComponent->ClientActivateAbilityFailed(Handle, PredictionKey.Current);
		return;
	}

	const UGameplayAbility* AbilityToActivate = Spec->Ability;

	if (!(AbilityToActivate))
	{
		AbilitySystemComponent->ClientActivateAbilityFailed(Handle, PredictionKey.Current);
		return;
	}

	// Ignore a client trying to activate an ability requiring server execution
	if (AbilityToActivate->NetSecurityPolicy == EGameplayAbilityNetSecurityPolicy::ServerOnlyExecution ||
		AbilityToActivate->NetSecurityPolicy == EGameplayAbilityNetSecurityPolicy::ServerOnly)
	{
		AbilitySystemComponent->ClientActivateAbilityFailed(Handle, PredictionKey.Current);
		return;
	}

	// Consume any pending target info, to clear out cancels from old executions
	//ConsumeAllReplicatedData(Handle, PredictionKey);

	UGameplayAbility* InstancedAbility = nullptr;
	Spec->InputPressed = true;

	// Attempt to activate the ability (server side) and tell the client if it succeeded or failed.
	if (InternalTryActivateAbility(AbilitySystemComponent, Handle, PredictionKey, &InstancedAbility, nullptr, TriggerEventData))
	{
		// TryActivateAbility handles notifying the client of success, but let's still log it
	}
	else
	{
		AbilitySystemComponent->ClientActivateAbilityFailed(Handle, PredictionKey.Current);
		Spec->InputPressed = false;
		//AbilitySystemComponent->ActivatableAbilities.MarkItemDirty(*Spec);
	}
}

namespace AbilitySystemComponent
{
	void InitHooks()
	{
		CREATEHOOK(BaseAddress() + 0xEA2BB60, InternalServerTryActivateAbilityHook, &InternalServerTryActivateAbility);
	}
}