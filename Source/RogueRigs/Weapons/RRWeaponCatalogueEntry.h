// Rogue Rigs - weapon catalogue entry: one weapon the miner can own, offered by the "New weapon" upgrade cards.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "RRWeaponCatalogueEntry.generated.h"

class ARRWeapon;

/**
 * One weapon of the run's catalogue (ARRMiner::WeaponCatalogue). New-weapon cards offer the entries the miner does not own yet
 * (sys-upgrades); the card shows the weapon's CSV DisplayName (ARRWeapon::GetClassDisplayName) and Description.
 */
USTRUCT(BlueprintType)
struct ROGUERIGS_API FRRWeaponCatalogueEntry
{
	GENERATED_BODY()

	/** Weapon class spawned when the card is picked (a Blueprint subclass in BP_Miner; native classes as code defaults). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<ARRWeapon> WeaponClass;

	/** Short French description shown on the new-weapon card. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (MultiLine = true))
	FText Description;
};
