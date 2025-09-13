#pragma once

#include "Includes.h"

static UFortIrisHelper* (*GetIrisHelper)() = decltype(GetIrisHelper)(BaseAddress() + 0x14FBAF20);