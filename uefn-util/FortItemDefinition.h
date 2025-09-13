#pragma once

static EFortRarity(*FortItem_GetRarity)(UFortItem* Item);
EFortRarity FortItem_GetRarityHook(UFortItem* Item)
{
    if (auto ItemDef = Item->GetItemDefinitionBP())
    {
        return ItemDef->Rarity;
    }

    return FortItem_GetRarity(Item);
}

static EFortRarity(*FortItemDefinition_GetRarity)(UFortItemDefinition* Item);
EFortRarity FortItemDefinition_GetRarityHook(UFortItemDefinition* Item)
{
    return Item->Rarity;
}

namespace FortItemDefinition
{
	void InitHooks()
	{
        auto DefaultAthenaBackpackItem = UAthenaBackpackItem::StaticClass()->DefaultObject;
        auto DefaultAthenaCharacterItem = UAthenaCharacterItem::StaticClass()->DefaultObject;
        auto DefaultAthenaCosmeticAccountItem = UAthenaCosmeticAccountItem::StaticClass()->DefaultObject;
        auto DefaultAthenaGliderItem = UAthenaGliderItem::StaticClass()->DefaultObject;

        VirtualHook(DefaultAthenaBackpackItem->VTable, 0xB7, FortItem_GetRarityHook, (PVOID*)&FortItem_GetRarity);
        VirtualHook(DefaultAthenaCharacterItem->VTable, 0xB7, FortItem_GetRarityHook, (PVOID*)&FortItem_GetRarity);
        VirtualHook(DefaultAthenaCosmeticAccountItem->VTable, 0xB7, FortItem_GetRarityHook, (PVOID*)&FortItem_GetRarity);
        VirtualHook(DefaultAthenaGliderItem->VTable, 0xB7, FortItem_GetRarityHook, (PVOID*)&FortItem_GetRarity);

        auto DefaultAthenaBackpackItemDefinition = UAthenaBackpackItemDefinition::StaticClass()->DefaultObject;
        auto DefaultAthenaBattleBusItemDefinition = UAthenaBattleBusItemDefinition::StaticClass()->DefaultObject;
        auto DefaultAthenaCharacterItemDefinition = UAthenaCharacterItemDefinition::StaticClass()->DefaultObject;
        auto DefaultAthenaCosmeticItemDefinition = UAthenaCosmeticItemDefinition::StaticClass()->DefaultObject;
        auto DefaultAthenaDanceItemDefinition = UAthenaDanceItemDefinition::StaticClass()->DefaultObject;
        auto DefaultAthenaEmojiItemDefinition = UAthenaEmojiItemDefinition::StaticClass()->DefaultObject;
        auto DefaultAthenaGliderItemDefinition = UAthenaGliderItemDefinition::StaticClass()->DefaultObject;
        auto DefaultAthenaItemWrapDefinition = UAthenaItemWrapDefinition::StaticClass()->DefaultObject;
        auto DefaultAthenaLoadingScreenItemDefinition = UAthenaLoadingScreenItemDefinition::StaticClass()->DefaultObject;
        auto DefaultAthenaMultiStateEmoteItemDefinition = UAthenaMultiStateEmoteItemDefinition::StaticClass()->DefaultObject;
        auto DefaultAthenaMusicPackItemDefinition = UAthenaMusicPackItemDefinition::StaticClass()->DefaultObject;
        auto DefaultAthenaPetCarrierItemDefinition = UAthenaPetCarrierItemDefinition::StaticClass()->DefaultObject;
        auto DefaultAthenaPickaxeItemDefinition = UAthenaPickaxeItemDefinition::StaticClass()->DefaultObject;
        auto DefaultAthenaRewardEventGraphCosmeticItemDefinition = UAthenaRewardEventGraphCosmeticItemDefinition::StaticClass()->DefaultObject;
        auto DefaultAthenaShoutItemDefinition = UAthenaShoutItemDefinition::StaticClass()->DefaultObject;
        auto DefaultAthenaSkyDiveContrailItemDefinition = UAthenaSkyDiveContrailItemDefinition::StaticClass()->DefaultObject;
        auto DefaultAthenaSprayItemDefinition = UAthenaSprayItemDefinition::StaticClass()->DefaultObject;
        auto DefaultAthenaToyItemDefinition = UAthenaToyItemDefinition::StaticClass()->DefaultObject;

        auto DefaultFortWeaponMeleeItemDefinition = UFortWeaponMeleeItemDefinition::StaticClass()->DefaultObject;
        auto DefaultFortWeaponMeleeDualWieldItemDefinition = UFortWeaponMeleeDualWieldItemDefinition::StaticClass()->DefaultObject;

        VirtualHook(DefaultAthenaBackpackItemDefinition->VTable, 0x8D, FortItemDefinition_GetRarityHook, (PVOID*)&FortItemDefinition_GetRarity);
        VirtualHook(DefaultAthenaBattleBusItemDefinition->VTable, 0x8D, FortItemDefinition_GetRarityHook, (PVOID*)&FortItemDefinition_GetRarity);
        VirtualHook(DefaultAthenaCharacterItemDefinition->VTable, 0x8D, FortItemDefinition_GetRarityHook, (PVOID*)&FortItemDefinition_GetRarity);
        VirtualHook(DefaultAthenaCosmeticItemDefinition->VTable, 0x8D, FortItemDefinition_GetRarityHook, (PVOID*)&FortItemDefinition_GetRarity);
        VirtualHook(DefaultAthenaDanceItemDefinition->VTable, 0x8D, FortItemDefinition_GetRarityHook, (PVOID*)&FortItemDefinition_GetRarity);
        VirtualHook(DefaultAthenaEmojiItemDefinition->VTable, 0x8D, FortItemDefinition_GetRarityHook, (PVOID*)&FortItemDefinition_GetRarity);
        VirtualHook(DefaultAthenaGliderItemDefinition->VTable, 0x8D, FortItemDefinition_GetRarityHook, (PVOID*)&FortItemDefinition_GetRarity);
        VirtualHook(DefaultAthenaItemWrapDefinition->VTable, 0x8D, FortItemDefinition_GetRarityHook, (PVOID*)&FortItemDefinition_GetRarity);
        VirtualHook(DefaultAthenaLoadingScreenItemDefinition->VTable, 0x8D, FortItemDefinition_GetRarityHook, (PVOID*)&FortItemDefinition_GetRarity);
        VirtualHook(DefaultAthenaMultiStateEmoteItemDefinition->VTable, 0x8D, FortItemDefinition_GetRarityHook, (PVOID*)&FortItemDefinition_GetRarity);
        VirtualHook(DefaultAthenaMusicPackItemDefinition->VTable, 0x8D, FortItemDefinition_GetRarityHook, (PVOID*)&FortItemDefinition_GetRarity);
        VirtualHook(DefaultAthenaPetCarrierItemDefinition->VTable, 0x8D, FortItemDefinition_GetRarityHook, (PVOID*)&FortItemDefinition_GetRarity);
        VirtualHook(DefaultAthenaPickaxeItemDefinition->VTable, 0x8D, FortItemDefinition_GetRarityHook, (PVOID*)&FortItemDefinition_GetRarity);
        VirtualHook(DefaultAthenaRewardEventGraphCosmeticItemDefinition->VTable, 0x8D, FortItemDefinition_GetRarityHook, (PVOID*)&FortItemDefinition_GetRarity);
        VirtualHook(DefaultAthenaShoutItemDefinition->VTable, 0x8D, FortItemDefinition_GetRarityHook, (PVOID*)&FortItemDefinition_GetRarity);
        VirtualHook(DefaultAthenaSkyDiveContrailItemDefinition->VTable, 0x8D, FortItemDefinition_GetRarityHook, (PVOID*)&FortItemDefinition_GetRarity);
        VirtualHook(DefaultAthenaSprayItemDefinition->VTable, 0x8D, FortItemDefinition_GetRarityHook, (PVOID*)&FortItemDefinition_GetRarity);
        VirtualHook(DefaultAthenaToyItemDefinition->VTable, 0x8D, FortItemDefinition_GetRarityHook, (PVOID*)&FortItemDefinition_GetRarity);

        VirtualHook(DefaultFortWeaponMeleeItemDefinition->VTable, 0x8D, FortItemDefinition_GetRarityHook, (PVOID*)&FortItemDefinition_GetRarity);
        VirtualHook(DefaultFortWeaponMeleeDualWieldItemDefinition->VTable, 0x8D, FortItemDefinition_GetRarityHook, (PVOID*)&FortItemDefinition_GetRarity);
	}
}