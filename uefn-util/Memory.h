#pragma once
#include "Includes.h"

namespace Sigs
{

}

static void HookExec(UFunction* Function, void* Hook, void** OG = nullptr)
{
	auto& Exec = Function->GetFunc();

	if (OG)
		*OG = Exec;

	Exec = Hook;
}

static void VirtualHook(void** vft, int idx, void* newfunc, void** OG = nullptr)
{
	if (OG)
		*OG = vft[idx];

	DWORD dwProt;
	VirtualProtect(&vft[idx], 8, PAGE_EXECUTE_READWRITE, &dwProt);
	vft[idx] = newfunc;
	DWORD dwTemp;
	VirtualProtect(&vft[idx], 8, dwProt, &dwTemp);
}
