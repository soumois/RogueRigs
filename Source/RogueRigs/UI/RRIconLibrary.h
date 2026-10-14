// Rogue Rigs - icon lookups for the UI: stat, weapon and upgrade card to icon texture, read from URRIconSettings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Templates/SubclassOf.h"
#include "Upgrades/RRUpgradeCard.h"
#include "Weapons/RRWeapon.h"
#include "RRIconLibrary.generated.h"

class UTexture2D;

/**
 * Blueprint-callable icon lookups shared by every widget (WBP_Weapons, WBP_UpgradeChoice). Each returns null, without loading or
 * crashing, when the mapping has no entry or the texture asset does not exist (e.g. a stat icon not imported yet).
 */
UCLASS()
class ROGUERIGS_API URRIconLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Icon of an upgrade stat (URRIconSettings::StatIcons); null when unmapped or not imported. */
	UFUNCTION(BlueprintPure, Category = "Rogue Rigs|Icons")
	static UTexture2D* GetStatIcon(ERRUpgradeStat Stat);

	/** Icon of a weapon from its display name ("Burst rifle", "Shotgun", "Turret launcher"; case-insensitive); null when unmapped. */
	UFUNCTION(BlueprintPure, Category = "Rogue Rigs|Icons")
	static UTexture2D* GetWeaponIconByName(const FText& WeaponName);

	/** Icon of a weapon class, through its CSV display name (ARRWeapon::GetClassDisplayName); null for no class or no mapping. */
	UFUNCTION(BlueprintPure, Category = "Rogue Rigs|Icons")
	static UTexture2D* GetWeaponIconByClass(TSubclassOf<ARRWeapon> WeaponClass);

	/** Icon of a level-up card: the weapon icon for a NewWeapon card, the stat icon for a stat card. */
	UFUNCTION(BlueprintPure, Category = "Rogue Rigs|Icons")
	static UTexture2D* GetUpgradeCardIcon(const FRRUpgradeCard& Card);

	/** Icon of a locked weapon slot (padlock); null when unmapped. */
	UFUNCTION(BlueprintPure, Category = "Rogue Rigs|Icons")
	static UTexture2D* GetLockedSlotIcon();

protected:
	/** Loads a mapped texture; null for an empty reference or a package that does not exist (checked first, so no load error). */
	static UTexture2D* LoadIcon(const TSoftObjectPtr<UTexture2D>& Icon);
};
