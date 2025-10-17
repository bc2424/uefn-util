#pragma once

#include "Includes.h"

static UPackage* (*LoadPackage)(UPackage* InOuter, const TCHAR* InLongPackageName, uint32 LoadFlags, void* InReaderOverride, void* InstancingContext) = decltype(LoadPackage)(0x1DB9430 + uintptr_t(GetModuleHandle(0)));

FString* (*GetExternalStreamingObjectPackagePath)(__int64 ContentBundle, const FString* a2) = decltype(GetExternalStreamingObjectPackagePath)(0xCB82AC0 + uintptr_t(GetModuleHandle(0)));
static void (*OnStreamingObjectLoaded)(URuntimeHashExternalStreamingObjectBase* ExternalStreamingObject, UWorld* InjectedWorld) = decltype(OnStreamingObjectLoaded)(BaseAddress() + 0xCCB86C0);

UWorld* (*GetInjectedWorld)(__int64 ContentBundle) = decltype(GetInjectedWorld)(0xCB82F30 + uintptr_t(GetModuleHandle(0)));

static void(*GetObjectsWithOuter)(const class UObject* Outer, TArray<UObject*>& Results, bool bIncludeNestedObjects, byte ExclusionFlags, byte ExclusionInternalFlags) = decltype(GetObjectsWithOuter)(0x1DE4660 + uintptr_t(GetModuleHandle(0)));

static void (*InitializeForPIE)(__int64 ContentBundle);
void InitializeForPIEHook(__int64 ContentBundle)
{
	FString ExternalStreamingObjectPackagePathString = {};

	GetExternalStreamingObjectPackagePath(ContentBundle, &ExternalStreamingObjectPackagePathString);

	if (UFortKismetLibrary::InEditorOrPIE(GetInjectedWorld(ContentBundle))) 
	{
		UWorld::RemovePIEPrefix(&ExternalStreamingObjectPackagePathString, ExternalStreamingObjectPackagePathString);
	}

	*(UPackage**)((uintptr_t)ContentBundle + 48) = LoadPackage(nullptr, ExternalStreamingObjectPackagePathString.CStr(), 0, nullptr, nullptr);

	if (UKismetSystemLibrary::IsValid(*(UPackage**)((uintptr_t)ContentBundle + 48)))
	{
		*(URuntimeHashExternalStreamingObjectBase**)((uintptr_t)ContentBundle + 56) = Cast<URuntimeHashExternalStreamingObjectBase>((UObject*)FindObjectWithOuter(*(UPackage**)((uintptr_t)ContentBundle + 48), URuntimeHashExternalStreamingObjectBase::StaticClass(), FName()));

		if (UKismetSystemLibrary::IsValid(*(URuntimeHashExternalStreamingObjectBase**)((uintptr_t)ContentBundle + 56)))
		{
			OnStreamingObjectLoaded(*(URuntimeHashExternalStreamingObjectBase**)((uintptr_t)ContentBundle + 56), GetInjectedWorld(ContentBundle));
		}
	}
}

static void (*OnStreamingObjectLoadedOG)(URuntimeHashExternalStreamingObjectBase* RuntimeHashExternalStreamingObject, UWorld* InjectedWorld);
void OnStreamingObjectLoadedHook(URuntimeHashExternalStreamingObjectBase* RuntimeHashExternalStreamingObjectBase, UWorld* InjectedWorld)
{
	if (RuntimeHashExternalStreamingObjectBase->IsA(URuntimeHashSetExternalStreamingObject::StaticClass()))
	{
		auto RuntimeHashExternalStreamingObject = (URuntimeHashSetExternalStreamingObject*)&RuntimeHashExternalStreamingObjectBase;
		/*
		if (RuntimeHashExternalStreamingObjectBase->CellToStreamingData.Num() >= 1)
		{
			TArray<UObject*> Objects;

			GetObjectsWithOuter(RuntimeHashExternalStreamingObjectBase, Objects);

			for (UObject* Object : Objects)
			{
				if (Object->IsA(UWorldPartitionRuntimeLevelStreamingCell::StaticClass()))
				{
					UWorldPartitionRuntimeLevelStreamingCell* RuntimeCell = (UWorldPartitionRuntimeLevelStreamingCell*)&Object;

					const FWorldPartitionRuntimeCellStreamingData& CellStreamingData = *RuntimeHashExternalStreamingObject->CellToStreamingData.FindByFName(RuntimeCell->Name);
					RuntimeCell->CreateAndSetLevelStreaming(CellStreamingData.PackageName, CellStreamingData.WorldAsset);
				}
			}
		}*/

		//gather all cells from every package
		for (int32 i = 0; i < RuntimeHashExternalStreamingObject->RuntimeStreamingData.Num(); i++)
		{
			FRuntimePartitionStreamingData* StreamingData = &RuntimeHashExternalStreamingObject->RuntimeStreamingData[i];

			for (int32 i = 0; i < StreamingData->SpatiallyLoadedCells.Num(); i++)
			{
				UWorldPartitionRuntimeLevelStreamingCell* RuntimeCell = (UWorldPartitionRuntimeLevelStreamingCell*)&StreamingData->SpatiallyLoadedCells[i];

				const FWorldPartitionRuntimeCellStreamingData& CellStreamingData = *RuntimeHashExternalStreamingObject->CellToStreamingData.Find(RuntimeCell->Name);
				RuntimeCell->CreateAndSetLevelStreaming(CellStreamingData.PackageName, CellStreamingData.WorldAsset);
			}

			for (int32 i = 0; i < StreamingData->SpatiallyLoadedCells.Num(); i++)
			{
				UWorldPartitionRuntimeLevelStreamingCell* RuntimeCell = (UWorldPartitionRuntimeLevelStreamingCell*)&StreamingData->SpatiallyLoadedCells[i];

				const FWorldPartitionRuntimeCellStreamingData& CellStreamingData = *RuntimeHashExternalStreamingObject->CellToStreamingData.Find(RuntimeCell->Name);
				RuntimeCell->CreateAndSetLevelStreaming(CellStreamingData.PackageName, CellStreamingData.WorldAsset);
			}
		}
	}

	return OnStreamingObjectLoadedOG(RuntimeHashExternalStreamingObjectBase, InjectedWorld);
}

namespace ContentBundle
{
	void InitHooks()
	{
		CREATEHOOK(BaseAddress() + 0xCB8D5E0, InitializeForPIEHook, &InitializeForPIE);
		CREATEHOOK(BaseAddress() + 0xCCB86C0, OnStreamingObjectLoadedHook, &OnStreamingObjectLoadedOG);
		//CREATEHOOK(BaseAddress() + 0xCB71E90, DoInjectContentHook, &DoInjectContent);
		//CREATEHOOK(BaseAddress() + 0xCB72A20, DoRemoveContentHook, &DoRemoveContent);

	}
}