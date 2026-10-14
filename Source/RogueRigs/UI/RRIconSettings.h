// Rogue Rigs - icon settings: the editable mapping from an upgrade stat or a weapon to its UI icon texture (DefaultGame.ini section [/Script/RogueRigs.RRIconSettings]).

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Upgrades/RRUpgradeCard.h"
#include "RRIconSettings.generated.h"

class UTexture2D;

/**
 * The single place that maps stats and weapons to icon textures (sys-upgrades). Soft references; the constructor gives the defaults
 * under /Game/RogueRigs/UI/Icons/, overridable without code in Config/DefaultGame.ini, section [/Script/RogueRigs.RRIconSettings].
 * Plain config object (not UDeveloperSettings: the module does not link DeveloperSettings). Read through URRIconLibrary.
 */
UCLASS(Config = Game, DefaultConfig)
class ROGUERIGS_API URRIconSettings : public UObject
{
	GENERATED_BODY()

public:
	/** Fills the default maps (T_Icon_Stat_<Stat> per offered stat, T_Icon_<weapon> per weapon display name). */
	URRIconSettings();

	/** Icon of each upgrade stat; a stat without an entry (MagazineSize) has no icon. */
	UPROPERTY(Config)
	TMap<ERRUpgradeStat, TSoftObjectPtr<UTexture2D>> StatIcons;

	/** Icon of each weapon, keyed by its display name (WeaponStats.csv column DisplayName, case-insensitive). A new weapon needs an entry. */
	UPROPERTY(Config)
	TMap<FString, TSoftObjectPtr<UTexture2D>> WeaponIcons;

	/** Icon of a locked weapon slot (padlock). */
	UPROPERTY(Config)
	TSoftObjectPtr<UTexture2D> LockedSlotIcon;
};
