#pragma once

#include "Includes.h"

static bool (*FPlugin_IsEnabled)(__int64 FPlugin) = decltype(FPlugin_IsEnabled)(BaseAddress() + 0x1A21320);
static FString& (*FPlugin_GetName)(__int64 FPlugin) = decltype(FPlugin_GetName)(BaseAddress() + 0x1A1FB10);
static inline bool (*TryLoadModulesForPlugin)(__int64 FPluginManager, __int64 Plugin, const byte LoadingPhase);
static inline bool (*TryUnloadModulesForPlugin)(__int64 FPluginManager, __int64 Plugin, const byte LoadingPhase, FText* OutFailureMessage /*= nullptr*/, bool bSkipUnload /*= false*/, bool bAllowUnloadCode /*= true*/) = decltype(TryUnloadModulesForPlugin)(BaseAddress() + 0x1A362B0);

static bool TryLoadModulesForPluginHook(__int64 FPluginManager, __int64 Plugin, const byte LoadingPhase)
{
    if (FPlugin_GetName(Plugin) == L"ValkyrieFortnite" || L"AtomVK" && LoadingPhase == 2)
    {
        return true;
    }

    return (TryLoadModulesForPlugin(FPluginManager, Plugin, LoadingPhase));
}