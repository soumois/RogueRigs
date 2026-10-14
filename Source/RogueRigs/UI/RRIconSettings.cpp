// Rogue Rigs - icon settings: default icon paths.

#include "UI/RRIconSettings.h"

#include "Engine/Texture2D.h"
#include "Weapons/RRWeapon.h"

namespace RRIconSettingsDefaults
{
	/** Soft reference to /Game/RogueRigs/UI/Icons/<AssetName>. */
	static TSoftObjectPtr<UTexture2D> IconPath(const FString& AssetName)
	{
		return TSoftObjectPtr<UTexture2D>(FSoftObjectPath(FString::Printf(TEXT("/Game/RogueRigs/UI/Icons/%s.%s"), *AssetName, *AssetName)));
	}
}

URRIconSettings::URRIconSettings()
{
	using namespace RRIconSettingsDefaults;

	const TPair<ERRUpgradeStat, const TCHAR*> Stats[] = {
		{ ERRUpgradeStat::Damage, TEXT("Damage") },
		{ ERRUpgradeStat::FireRate, TEXT("FireRate") },
		{ ERRUpgradeStat::ReloadSpeed, TEXT("ReloadSpeed") },
		{ ERRUpgradeStat::CritChance, TEXT("CritChance") },
		{ ERRUpgradeStat::CritDamage, TEXT("CritDamage") },
		{ ERRUpgradeStat::Pierce, TEXT("Piercing") },
		{ ERRUpgradeStat::MaxHealth, TEXT("MaxHealth") },
		{ ERRUpgradeStat::MoveSpeed, TEXT("MoveSpeed") },
		{ ERRUpgradeStat::PickupRadius, TEXT("PickupRadius") },
		{ ERRUpgradeStat::XPGain, TEXT("XPGain") },
		{ ERRUpgradeStat::MiningSpeed, TEXT("MiningSpeed") },
		{ ERRUpgradeStat::Luck, TEXT("Luck") },
	};
	for (const TPair<ERRUpgradeStat, const TCHAR*>& Stat : Stats)
	{
		StatIcons.Add(Stat.Key, IconPath(FString(TEXT("T_Icon_Stat_")) + Stat.Value));
	}

	WeaponIcons.Add(TEXT("Burst rifle"), IconPath(TEXT("T_Icon_burst_rifle")));
	WeaponIcons.Add(TEXT("Shotgun"), IconPath(TEXT("T_Icon_shotgun")));
	WeaponIcons.Add(TEXT("Turret launcher"), IconPath(TEXT("T_Icon_turret_launcher")));

	LockedSlotIcon = IconPath(TEXT("T_Icon_padlock"));
}
