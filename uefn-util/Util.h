#pragma once
#include "Includes.h"

static __forceinline uintptr_t BaseAddress()
{
	static uintptr_t BaseAddr = 0;

	if (BaseAddr == 0) BaseAddr = reinterpret_cast<uintptr_t>(GetModuleHandle(0));

	return BaseAddr;
}

// probably should forceinline but wtv!
template <typename T, bool bCheckType = true>
static __forceinline T* Cast(UObject* Object)
{
	if (bCheckType)
	{
		if (Object && Object->IsA(T::StaticClass()))
		{
			return (T*)Object;
		}
	}
	else
	{
		return (T*)Object;
	}

	return nullptr;
}

template <typename T>
inline T* GetDefaultObject()
{
	return (T*)T::StaticClass()->DefaultObject;
}

static void* (*GetInterfaceAddress)(UObject*, UClass*) = decltype(GetInterfaceAddress)(BaseAddress() + 0x1DB1A20);

//#define LOG(string) UFortKismetLibrary::FortShippingLog(UWorld::GetWorld(), string, false)
#define LOG(logstring) UFortKismetLibrary::FortShippingLog(UWorld::GetWorld(), FString(std::wstring(std::string(logstring).begin(), std::string(logstring).end()).c_str()), false)
//#define LOG(logstring) UKismetSystemLibrary::ExecuteConsoleCommand(UWorld::GetWorld(), logstring, false)
//(UWorld::GetWorld(), FString(std::wstring(std::string(logstring).begin(), std::string(logstring).end()).c_str()), false)

#define ENUM_CLASS_FLAGS(Enum) \
	inline           Enum& operator|=(Enum& Lhs, Enum Rhs) { return Lhs = (Enum)((__underlying_type(Enum))Lhs | (__underlying_type(Enum))Rhs); } \
	inline           Enum& operator&=(Enum& Lhs, Enum Rhs) { return Lhs = (Enum)((__underlying_type(Enum))Lhs & (__underlying_type(Enum))Rhs); } \
	inline           Enum& operator^=(Enum& Lhs, Enum Rhs) { return Lhs = (Enum)((__underlying_type(Enum))Lhs ^ (__underlying_type(Enum))Rhs); } \
	inline constexpr Enum  operator| (Enum  Lhs, Enum Rhs) { return (Enum)((__underlying_type(Enum))Lhs | (__underlying_type(Enum))Rhs); } \
	inline constexpr Enum  operator& (Enum  Lhs, Enum Rhs) { return (Enum)((__underlying_type(Enum))Lhs & (__underlying_type(Enum))Rhs); } \
	inline constexpr Enum  operator^ (Enum  Lhs, Enum Rhs) { return (Enum)((__underlying_type(Enum))Lhs ^ (__underlying_type(Enum))Rhs); } \
	inline constexpr bool  operator! (Enum  E)             { return !(__underlying_type(Enum))E; } \
	inline constexpr Enum  operator~ (Enum  E)             { return (Enum)~(__underlying_type(Enum))E; }