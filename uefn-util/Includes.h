#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include <cstdio>
#pragma comment(lib,"minhook/minhook.lib")
#include "minhook/MinHook.h"

// Uncomment out once added SDK!
#include "CppSDK/SDK.hpp"
using namespace SDK;
//using namespace Params;

#define CREATEHOOK(Address, Hook, Og) \
MH_CreateHook((void*)(Address), Hook, (void**)(Og));
//#define Cast(Class, Object) Cast<Class>(Object)

#include "Util.h"
#include "Memory.h"
#include "Globals.h"