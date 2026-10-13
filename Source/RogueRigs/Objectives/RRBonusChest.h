// Rogue Rigs - bonus chest objective: an optional console-like hack target that rewards XP, ore and a lucky upgrade choice.

#pragma once

#include "CoreMinimal.h"
#include "Objectives/RRConsole.h"
#include "Templates/SubclassOf.h"
#include "RRBonusChest.generated.h"

class ARRPickup;

/**
 * Bonus chest (objective-bonus-chest). Dropped by URRRunFlowSubsystem once per level when the alert reaches
 * BonusChestAlertThreshold; optional: it never pauses the alert bar. Hacked with the console rules it inherits from ARRConsole
 * (miner alive within ZoneRadiusCells -> ARRDrone::StartHack, progress dt / HackDuration, DrainPerSecond while out), but marks no
 * rock (MarkedCellTypes empty), so it starts Idle. When the hack completes (CompleteHack, or ForceComplete
 * cheat) it grants its reward once: XP = XPNeedFraction x the miner's current level need (URRProgressionComponent::AddXP, so the
 * XPGain stat applies like any XP), OreChunkCount ore pickups popped around it (OrePickupClass, none = the grid's ore pickup),
 * and one upgrade choice drawn with the miner's luck + BonusLuck for that draw only (URRLevelUpComponent::QueueBonusChoice).
 * Blueprint events: the console ones (OnHackProgress, OnHackStateChanged) plus OnRewardGranted. Not abstract: works headless.
 */
UCLASS(Blueprintable)
class ROGUERIGS_API ARRBonusChest : public ARRConsole
{
	GENERATED_BODY()

public:
	/** Sets the chest defaults: hack 8 s, zone 6 cells, no marked rocks. */
	ARRBonusChest();

	/** True once the reward was granted (the hack completed). */
	UFUNCTION(BlueprintPure, Category = "Bonus Chest")
	bool WasRewardGranted() const { return bRewardGranted; }

	/** XP given by the last reward (after nothing: 0). Raw amount passed to AddXP, before the XPGain stat. */
	UFUNCTION(BlueprintPure, Category = "Bonus Chest")
	float GetRewardXP() const { return RewardXP; }

	/** Fraction of the current level XP need given as XP. */
	UFUNCTION(BlueprintPure, Category = "Bonus Chest")
	float GetXPNeedFraction() const { return XPNeedFraction; }

	/** Ore pickups popped by the reward. */
	UFUNCTION(BlueprintPure, Category = "Bonus Chest")
	int32 GetOreChunkCount() const { return OreChunkCount; }

	/** Luck added to the reward's upgrade draw only. */
	UFUNCTION(BlueprintPure, Category = "Bonus Chest")
	float GetBonusLuck() const { return BonusLuck; }

protected:
	/** Completes the hack (ARRConsole), then grants the reward once. */
	virtual void CompleteHack() override;

	/** Grants XP, pops the ore chunks and queues the lucky upgrade choice on the miner. Once. */
	void GrantReward();

	/** Spawns OreChunkCount ore pickups at the chest, ejected in evenly spread directions. Returns how many spawned. */
	int32 PopOreChunks();

	/** Display event: the reward was granted (XP given, ore chunks spawned, upgrade choice queued). Implement in Blueprint (open lid, burst). */
	UFUNCTION(BlueprintImplementableEvent, Category = "Bonus Chest")
	void OnRewardGranted(float XP, int32 OreChunks, bool bUpgradeChoiceQueued);

	/** Fraction of the miner's current level XP need given as XP (objective-bonus-chest: 50 %, provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bonus Chest", meta = (ClampMin = 0))
	float XPNeedFraction = 0.5f;

	/** Ore pickups popped when the hack completes (objective-bonus-chest: 5, provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bonus Chest", meta = (ClampMin = 0))
	int32 OreChunkCount = 5;

	/** Ore pickup class of the reward. None = the grid's ore pickup (ARRArenaGrid::GetDropPickupClass(Ore)). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bonus Chest")
	TSubclassOf<ARRPickup> OrePickupClass;

	/** Luck added to the reward's upgrade draw only (objective-bonus-chest: +3, provisional; rarities). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bonus Chest")
	float BonusLuck = 3.f;

	/** True once the reward was granted. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Bonus Chest")
	bool bRewardGranted = false;

	/** Raw XP given by the reward. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Bonus Chest")
	float RewardXP = 0.f;
};
