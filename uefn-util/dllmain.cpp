
#include <Windows.h>
#include <iostream>
#include <detours.h>

#include "Cobalt/curlhook.h"
#include "Cobalt/cobalt.h"
#include "Cobalt/exithook.h"
#include "minhook/MinHook.h"

#include "CoreGlobals.h"
#include "PluginManager.h"
#include "Util.h"
#include "Includes.h"

static inline void (*ValkyrieInit)();
static void ValkyrieInitHook() { return; }

DWORD WINAPI Main(LPVOID)
{
    MH_Initialize();
    Memcury::VEHHook::Init();
    
    //kill valkyrie
    CREATEHOOK(BaseAddress() + 0x1EA23E00, ValkyrieInitHook, &ValkyrieInit); //I think valkyriefortniteengine::init
    CREATEHOOK(BaseAddress() + 0x1A348E0, TryLoadModulesForPluginHook, &TryLoadModulesForPlugin);

    //do hooks once we have reached this state
    CREATEHOOK(BaseAddress() + 0xD287500, SetEngineStartupModuleLoadingCompleteHook, &SetEngineStartupModuleLoadingComplete);

    WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(BaseAddress() + 0x1A7D780), "\xc3", 1, NULL); //gameplay cues need to be hooked before asset search is finished

    MH_EnableHook(MH_ALL_HOOKS);

    bool curlResult = InitializeCurlHook();
    InitializeEOSCurlHook();

    bool result = curlResult;

    return 0;
}
BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(0, 0, Main, 0, 0, 0);
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

