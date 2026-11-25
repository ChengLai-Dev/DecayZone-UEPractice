#include "Data/Item/FDZ_ItemData.h"

#include "Data/ItemType/Armor/DZ_ArmorData.h"
#include "Data/ItemType/DZ_EquipmentData.h"
#include "Data/ItemType/Weapon/DZ_WeaponData.h"

EItemType FDZ_ItemData::GetItemType() const
{
	if (GetWeaponData()) return EItemType::Weapon;
	if (Cast<UArmorData>(Extension)) return EItemType::Armor;
	return EItemType::Normal;
}

EEquipSlot FDZ_ItemData::GetEquipSlot() const
{
	const UEquipmentData* EquipmentData = Cast<UEquipmentData>(Extension);
	return EquipmentData ? EquipmentData->EquipSlot : EEquipSlot::None;
}

float FDZ_ItemData::GetArmorBonus() const
{
	const UArmorData* ArmorData = Cast<UArmorData>(Extension);
	return ArmorData ? ArmorData->ArmorBonus : 0.f;
}

const UWeaponData* FDZ_ItemData::GetWeaponData() const
{
	return Cast<UWeaponData>(Extension);
}
