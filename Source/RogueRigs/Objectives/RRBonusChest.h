// Rogue Rigs - bonus chest objective: an optional console-like hack target that rewards ore and a lucky upgrade choice (direct XP off by default).

#pragma once

#include "CoreMinimal.h"
#include "Objectives/RRConsole.h"
#include "Templates/SubclassOf.h"
#include "RRBonusChest.generated.h"

class ARRPickup;
class UMaterialInstanceDynamic;
class UStaticMeshComponent;

/**
 * Bonus chest (objective-bonus-chest). Dropped by URRRunFlowSubsystem once per level when the alert reaches
 * BonusChestAlertThreshold; optional: it never pauses the alert bar. Hacked with the console rules it inherits from ARRConsole
 * (miner alive within ZoneRadiusCells -> ARRDrone::StartHack, progress dt / HackDuration, DrainPerSecond while out), but marks no
 * rock (MarkedCellTypes empty), so it starts Idle. When the hack completes (CompleteHack, or ForceComplete
 * cheat) it grants its reward once: XP = XPNeedFraction x the miner's current level need (URRProgressionComponent::AddXP, so the
 * XPGain stat applies like any XP; skipped when the amount is 0, the default since owner 2026-10-23), OreChunkCount ore pickups popped around it (OrePickupClass, none = the grid's ore pickup),
 * and one upgrade choice drawn with the miner's luck + BonusLuck for that draw only (URRLevelUpComponent::QueueBonusChoice).
 * Blueprint events: the console ones (OnHackProgress, OnHackStateChanged) plus OnRewardGranted. Not abstract: works headless.
 * Lock light (task-308-02): the LockLight slot of the Latch Blueprint component (MI_ChestLockLight, parent M_ConsoleLamp) gets a
 * dynamic instance; Idle (and Locked) red blinking LockIdleBlinkRate, Hacking orange blinking from LockHackBlinkRateStart to
 * LockHackBlinkRateEnd with the hack progress (in LockHackBlinkRateSteps bands when the list is set), Complete steady green.
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

	/** Lock light colour last computed for the current state (written on the LockLight instance when it exists). */
	UFUNCTION(BlueprintPure, Category = "Bonus Chest|Lock Light")
	FLinearColor GetLockLightColor() const { return LockLightColor; }

	/** Lock light blink rate (Hz) last computed for the current state and progress; 0 = steady. */
	UFUNCTION(BlueprintPure, Category = "Bonus Chest|Lock Light")
	float GetLockLightBlinkRate() const { return LockLightBlinkRate; }

	/** Blink rate (Hz) of the Hacking lock light at Progress (0..1): LockHackBlinkRateSteps band when the list is set, else linear from LockHackBlinkRateStart to LockHackBlinkRateEnd. */
	UFUNCTION(BlueprintPure, Category = "Bonus Chest|Lock Light")
	float GetHackingBlinkRate(float Progress) const;

protected:
	/** Completes the hack (ARRConsole), then grants the reward once. */
	virtual void CompleteHack() override;

	/** Drives the lock light from the state (ARRConsole calls it on every state change). */
	virtual void ApplyStateVisuals() override;

	/** Updates the Hacking blink rate from the progress (ARRConsole calls it on every progress change). */
	virtual void ApplyProgressVisuals() override;

	/** Finds the Latch component and creates the LockLight dynamic instance once (a missing component or slot is skipped). */
	void SetupLockLight();

	/** Computes the lock light colour and rate of the current state and progress, and writes them on the instance when they changed. */
	void RefreshLockLight();

	/** Grants XP, pops the ore chunks and queues the lucky upgrade choice on the miner. Once. */
	void GrantReward();

	/** Spawns OreChunkCount ore pickups at the chest, ejected in evenly spread directions. Returns how many spawned. */
	int32 PopOreChunks();

	/** Display event: the reward was granted (XP given, ore chunks spawned, upgrade choice queued). Implement in Blueprint (open lid, burst). */
	UFUNCTION(BlueprintImplementableEvent, Category = "Bonus Chest")
	void OnRewardGranted(float XP, int32 OreChunks, bool bUpgradeChoiceQueued);

	/**
	 * Fraction of the miner's current level XP need given as XP (objective-bonus-chest). Default 0: no direct XP, AddXP skipped.
	 * History: was 0.5 (50 % of the level need); owner 2026-10-23: one upgrade, not two (the XP could trigger a level-up on top of
	 * the bonus choice). Kept as a tunable.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bonus Chest", meta = (ClampMin = 0))
	float XPNeedFraction = 0.f;

	/** Ore pickups popped when the hack completes (objective-bonus-chest: 5, provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bonus Chest", meta = (ClampMin = 0))
	int32 OreChunkCount = 5;

	/** Ore pickup class of the reward. None = the grid's ore pickup (ARRArenaGrid::GetDropPickupClass(Ore)). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bonus Chest")
	TSubclassOf<ARRPickup> OrePickupClass;

	/** Luck added to the reward's upgrade draw only (objective-bonus-chest: +3, provisional; rarities). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bonus Chest")
	float BonusLuck = 3.f;

	/** Lock light colour while Idle (and Locked): red, the console Locked red (task-308, owner 2026-10-23, provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bonus Chest|Lock Light")
	FLinearColor LockIdleColor = FLinearColor(1.f, 0.05f, 0.03f);

	/** Lock light blink rate while Idle (and Locked), Hz (task-308: 2, provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bonus Chest|Lock Light", meta = (ClampMin = 0, Units = "Hz"))
	float LockIdleBlinkRate = 2.f;

	/** Lock light colour while Hacking (miner in the zone): orange, the console Idle orange (task-308, provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bonus Chest|Lock Light")
	FLinearColor LockHackColor = FLinearColor(1.f, 0.45f, 0.05f);

	/** Hacking blink rate at 0 % progress, Hz (task-308 default Q17: 2, provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bonus Chest|Lock Light", meta = (ClampMin = 0, Units = "Hz"))
	float LockHackBlinkRateStart = 2.f;

	/** Hacking blink rate at 100 % progress, Hz (task-308 default Q17: 6, provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bonus Chest|Lock Light", meta = (ClampMin = 0, Units = "Hz"))
	float LockHackBlinkRateEnd = 6.f;

	/**
	 * Hacking blink rates by equal progress bands (N rates: band = floor(progress x N), the last one includes 100 %). Empty =
	 * continuous linear Start..End. Default 2, 3, 4, 6 Hz (task-308 Q17 fallback): M_ConsoleLamp blinks on Sine(Time x BlinkRate),
	 * so every rate change shifts the phase by Time x delta rate; a per-frame linear rate makes the lamp strobe, a step jumps once.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bonus Chest|Lock Light")
	TArray<float> LockHackBlinkRateSteps = { 2.f, 3.f, 4.f, 6.f };

	/** Lock light colour once Complete: green, steady (task-308, provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bonus Chest|Lock Light")
	FLinearColor LockCompleteColor = FLinearColor(0.1f, 1.f, 0.15f);

	/** BlinkAmount written while the blink rate is above 0 (1 = full on/off; 0 is written when steady). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bonus Chest|Lock Light", meta = (ClampMin = 0, ClampMax = 1))
	float LockBlinkAmount = 1.f;

	/** Name of the Blueprint StaticMeshComponent holding the lock light (BP_BonusChest: Latch). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bonus Chest|Lock Light")
	FName LatchComponentName = TEXT("Latch");

	/** Material slot of the latch holding the lock light (SM_Chest_Latch: LockLight, MI_ChestLockLight). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bonus Chest|Lock Light")
	FName LockLightSlotName = TEXT("LockLight");

	/** Dynamic instance of the lock light slot (null without the Blueprint latch, e.g. headless tests). */
	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> LockLightMID;

	/** True once SetupLockLight ran. */
	bool bLockLightSetUp = false;

	/** True once a colour and rate were written (forces the first write). */
	bool bLockLightWritten = false;

	/** Lock light colour of the current state. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Bonus Chest|Lock Light")
	FLinearColor LockLightColor = FLinearColor::Black;

	/** Lock light blink rate of the current state and progress, Hz. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Bonus Chest|Lock Light")
	float LockLightBlinkRate = 0.f;

	/** True once the reward was granted. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Bonus Chest")
	bool bRewardGranted = false;

	/** Raw XP given by the reward. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Bonus Chest")
	float RewardXP = 0.f;
};
