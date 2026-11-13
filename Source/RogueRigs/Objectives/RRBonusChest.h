// Rogue Rigs - bonus chest objective: an optional console-like hack target that opens (latch, lid slam, inner light) and rewards XP chips, ore and a lucky upgrade choice.

#pragma once

#include "CoreMinimal.h"
#include "Objectives/RRConsole.h"
#include "Templates/SubclassOf.h"
#include "RRBonusChest.generated.h"

class ARRPickup;
class UCameraShakeBase;
class UFXSystemComponent;
class UMaterialInstanceDynamic;
class UPointLightComponent;
class UStaticMeshComponent;
class URRBlobShadowComponent;

/**
 * Bonus chest (objective-bonus-chest). Dropped by URRRunFlowSubsystem once per level when the alert reaches
 * BonusChestAlertThreshold; optional: it never pauses the alert bar. Hacked with the console rules it inherits from ARRConsole
 * (miner alive within ZoneRadiusCells -> ARRDrone::StartHack, progress dt / HackDuration, DrainPerSecond while out), but marks no
 * rock (MarkedCellTypes empty), so it starts Idle. When the hack completes (CompleteHack, or ForceComplete cheat) it plays its
 * opening sequence (task-309-01, below) and grants its reward once at the lid opening (LidOpenDelay after the completion):
 * XP = XPNeedFraction x the miner's current level need (URRProgressionComponent::AddXP, so the
 * XPGain stat applies like any XP; skipped when the amount is 0, the default since owner 2026-10-23), the ARRConsole XP burst
 * (XPBurstChipCount 3 big chips of XPBurstChipXP 8, XPBurstSpawnRadius out, each with ChipScalePop; bXPBurstOnComplete false),
 * OreChunkCount ore pickups popped around it (OrePickupClass, none = the grid's ore pickup),
 * and one upgrade choice drawn with the miner's luck + BonusLuck for that draw only (URRLevelUpComponent::QueueBonusChoice).
 * Blueprint events: the console ones (OnHackProgress, OnHackStateChanged) plus OnLidSlam, OnLidOpened and OnRewardGranted.
 * Not abstract: works headless (without Lid, Latch or Interior slot the timeline, events and reward still run).
 * Opening sequence (task-309-01, owner 2026-10-23 provisional), timed by the chest tick from the completion, chest only (no time
 * dilation, the rest of the game runs): the Blueprint component LatchComponentName turns about its local X (its top edge) by
 * LatchRollSign x LatchOpenAngle over LatchSwingDuration (ease-out); at LidOpenDelay comes the lid opening: InnerLight shown,
 * InteriorGlowParameterName = InteriorGlowOnValue on the Mesh InteriorSlotName slot, OnLidOpened, then the reward; the component
 * LidComponentName turns about its local X (the hinge) by LidRollSign x angle: to LidPeakAngle over LidRiseDuration (ease-out),
 * held LidHoldDuration (OnLidSlam at the hold start, camera shake LidSlamShakeClass x LidSlamShakeScale at its end), then to
 * LidOpenAngle over LidSettleDuration (ease-in-out), and stays open. InnerLight keeps its Blueprint intensity for
 * InnerLightOnDuration, then fades to 0 over InnerLightFadeDuration and hides; the interior glow stays on. Roll signs from the
 * task-309-07 cut report (derived, not measured). The tick turns off once every step ran.
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

	/** Advances the opening sequence after the completion (ARRConsole::Tick first). */
	virtual void Tick(float DeltaSeconds) override;

	/** True from the completion until every opening step (lid settled, light faded) ran. */
	UFUNCTION(BlueprintPure, Category = "Bonus Chest|Opening")
	bool IsOpening() const { return bOpeningStarted && !bOpeningFinished; }

	/** True once the lid opening moment passed (InnerLight on, OnLidOpened, reward). */
	UFUNCTION(BlueprintPure, Category = "Bonus Chest|Opening")
	bool IsLidOpened() const { return bLidOpened; }

	/** Seconds since the hack completed (0 before). */
	UFUNCTION(BlueprintPure, Category = "Bonus Chest|Opening")
	float GetOpeningElapsed() const { return OpeningElapsed; }

	/** Current lid opening angle in degrees (magnitude, before LidRollSign). */
	UFUNCTION(BlueprintPure, Category = "Bonus Chest|Opening")
	float GetLidAngle() const { return LidAngle; }

	/** Current latch opening angle in degrees (magnitude, before LatchRollSign). */
	UFUNCTION(BlueprintPure, Category = "Bonus Chest|Opening")
	float GetLatchAngle() const { return LatchAngle; }

	/** Lid angle (degrees, magnitude) SecondsSinceLidStart after the lid opening: rise, hold, settle, then open. */
	UFUNCTION(BlueprintPure, Category = "Bonus Chest|Opening")
	float GetLidAngleAt(float SecondsSinceLidStart) const;

	/** Latch angle (degrees, magnitude) SecondsSinceCompletion after the hack completed. */
	UFUNCTION(BlueprintPure, Category = "Bonus Chest|Opening")
	float GetLatchAngleAt(float SecondsSinceCompletion) const;

	/** Seconds from the completion to the lid opening (the reward time). */
	UFUNCTION(BlueprintPure, Category = "Bonus Chest|Opening")
	float GetLidOpenDelay() const { return LidOpenDelay; }

	/** Seconds from the lid opening to the start of the hold at LidPeakAngle (OnLidSlam). */
	UFUNCTION(BlueprintPure, Category = "Bonus Chest|Opening")
	float GetLidRiseDuration() const { return LidRiseDuration; }

	/** Point light inside the chest, hidden until the lid opening. */
	UFUNCTION(BlueprintPure, Category = "Bonus Chest|Opening")
	UPointLightComponent* GetInnerLight() const { return InnerLight; }

	/** Scale pop given to the reward chips. */
	const FRRPickupScalePop& GetChipScalePop() const { return ChipScalePop; }

	/** Blink rate (Hz) of the Hacking lock light at Progress (0..1): LockHackBlinkRateSteps band when the list is set, else linear from LockHackBlinkRateStart to LockHackBlinkRateEnd. */
	UFUNCTION(BlueprintPure, Category = "Bonus Chest|Lock Light")
	float GetHackingBlinkRate(float Progress) const;

	/**
	 * Removes what the opening left in the world (task-388-01): every effect component the Blueprint spawned in OnLidOpened and
	 * OnLidSlam (world-owned, so not destroyed with the chest; a paused world freezes them and they resume in the next level) is
	 * deactivated at once and destroyed (or returned to the Niagara pool), and InnerLight is hidden. Call it before the chest is
	 * destroyed between two levels in the same world (URRRunFlowSubsystem::ResetForNextLevel); EndPlay also calls it.
	 * Returns how many were still alive: active effects plus the light when it was visible.
	 */
	int32 ClearOpeningEffects();

	/** Number of effects spawned by the opening events that are still tracked (alive or not). */
	int32 GetTrackedOpeningEffectCount() const { return OpeningEffects.Num(); }

	/** Component tag put on every effect the opening events spawned, so a later check can find them without the chest. */
	static const FName OpeningEffectTag;

	/** Opening effects alive in World: active effect components tagged OpeningEffectTag plus visible InnerLights of bonus chests. */
	static int32 CountLiveOpeningEffects(const UWorld* World);

protected:
	/** Clears the opening effects (ClearOpeningEffects) whatever ends the chest, then ARRConsole::EndPlay. */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** True when Effect is active, or is a Niagara component whose activation still waits for its system (task-526-01). */
	static bool IsOpeningEffectAlive(const UFXSystemComponent* Effect);

	/** Fills Out with every effect component (Niagara or Cascade) of this world, active or not, mapped to whether it is active now. */
	void CollectWorldEffects(TMap<const UFXSystemComponent*, bool>& Out) const;

	/**
	 * Tracks and tags the effect components spawned by EventName: those absent from Before (new, active or not: a Niagara system
	 * still compiling or waiting for its PSOs activates a few frames later, task-526-01), and those inactive in Before and active
	 * now (a pooled component reused). Returns how many.
	 */
	int32 TrackEffectsSpawnedSince(const TMap<const UFXSystemComponent*, bool>& Before, const TCHAR* EventName);

	/** Completes the hack (ARRConsole), then starts the opening sequence (the reward comes at the lid opening). */
	virtual void CompleteHack() override;

	/** Reads the InnerLight Blueprint intensity (the on value) and hides the light; fits BlobShadow to the chest meshes (task-472-01). */
	virtual void BeginPlay() override;

	/** No mesh of the chest or its Blueprint casts a light shadow (task-472-01, owner 2026-11-06 Q2). */
	virtual void PostInitializeComponents() override;

	/** Finds the Lid and Latch components by name and keeps their closed relative rotations (once, at the completion). */
	void SetupOpeningComponents();

	/** Advances the timeline by DeltaSeconds: lid opening, poses, slam, shake and light fade when their time is reached. */
	void UpdateOpening(float DeltaSeconds);

	/** Lid opening moment: InnerLight on, interior glow on, OnLidOpened, then the reward. Once. */
	void OpenLid();

	/** Writes InteriorGlowOnValue on a dynamic instance of the Mesh Interior slot (skipped without the slot). */
	void ApplyInteriorGlow();

	/** Turns Component about its local X from ClosedRotation by Sign x AngleDegrees (relative Roll). No-op for a null component. */
	static void ApplyRoll(USceneComponent* Component, const FQuat& ClosedRotation, float Sign, float AngleDegrees);

	/** Drives the lock light from the state (ARRConsole calls it on every state change). */
	virtual void ApplyStateVisuals() override;

	/** Updates the Hacking blink rate from the progress (ARRConsole calls it on every progress change). */
	virtual void ApplyProgressVisuals() override;

	/** Finds the Latch component and creates the LockLight dynamic instance once (a missing component or slot is skipped). */
	void SetupLockLight();

	/** Computes the lock light colour and rate of the current state and progress, and writes them on the instance when they changed. */
	void RefreshLockLight();

	/** Grants XP (none by default), the XP chip burst, the ore chunks and the lucky upgrade choice on the miner. Once, at the lid opening. */
	void GrantReward();

	/** Spawns OreChunkCount ore pickups at the chest, ejected in evenly spread directions. Returns how many spawned. */
	int32 PopOreChunks();

	/** Display event at the lid slam (start of the hold at LidPeakAngle): Location = the actor location (floor). BP_BonusChest spawns the ground ring. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Bonus Chest|Opening")
	void OnLidSlam(FVector Location);

	/** Display event at the lid opening (LidOpenDelay after the completion, before OnRewardGranted): Location = the actor location. BP_BonusChest spawns the rays and sparks. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Bonus Chest|Opening")
	void OnLidOpened(FVector Location);

	/** Display event: the reward was granted at the lid opening (XP given, chips and ore chunks spawned, upgrade choice queued). */
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

	/** Point light inside the chest (attached to Mesh), hidden until the lid opening; colour, intensity, radius and place set in Blueprint (task-309-05). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UPointLightComponent> InnerLight;

	/** Light-independent shadow under the chest (task-472-01): fitted at BeginPlay to the closed chest; opacity and size on this component. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<URRBlobShadowComponent> BlobShadow;

	/** Name of the Blueprint StaticMeshComponent of the lid (BP_BonusChest: Lid, origin on the hinge axis). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bonus Chest|Opening")
	FName LidComponentName = TEXT("Lid");

	/** Latch opening angle in degrees about its top edge (task-309 Q9: 90, provisional owner 2026-10-23). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bonus Chest|Opening", meta = (ClampMin = 0, Units = "Degrees"))
	float LatchOpenAngle = 90.f;

	/** Latch swing time from the completion, seconds (task-309 Q9: 0.2, provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bonus Chest|Opening", meta = (ClampMin = 0, Units = "s"))
	float LatchSwingDuration = 0.2f;

	/** Ease-out exponent of the latch swing (implementation choice: 2; 1 = linear). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bonus Chest|Opening", meta = (ClampMin = 1))
	float LatchEaseExponent = 2.f;

	/**
	 * Sign of the latch relative Roll that swings its bottom outward and up: -1 (task-309-07 cut report, derived from the Y mirror
	 * of the import and the left-handed rule, not measured in the editor; task-309-05 checks it and flips it here if wrong).
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bonus Chest|Opening", meta = (ClampMin = -1, ClampMax = 1))
	float LatchRollSign = -1.f;

	/** Seconds from the completion (latch start) to the lid opening (task-309 Q10: 0.5, provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bonus Chest|Opening", meta = (ClampMin = 0, Units = "s"))
	float LidOpenDelay = 0.5f;

	/** Lid overshoot angle in degrees (task-309 Q18: 115, provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bonus Chest|Opening", meta = (ClampMin = 0, Units = "Degrees"))
	float LidPeakAngle = 115.f;

	/** Lid rise time from closed to LidPeakAngle, seconds (task-309 Q18: 0.4, provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bonus Chest|Opening", meta = (ClampMin = 0, Units = "s"))
	float LidRiseDuration = 0.4f;

	/** Ease-out exponent of the lid rise (implementation choice: 2; 1 = linear). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bonus Chest|Opening", meta = (ClampMin = 1))
	float LidRiseEaseExponent = 2.f;

	/** Chest-only freeze at LidPeakAngle, seconds (task-309 Q19: 0.06, provisional; no global time dilation). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bonus Chest|Opening", meta = (ClampMin = 0, Units = "s"))
	float LidHoldDuration = 0.06f;

	/** Lid resting open angle in degrees (task-309 Q10 and Q18: 100, provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bonus Chest|Opening", meta = (ClampMin = 0, Units = "Degrees"))
	float LidOpenAngle = 100.f;

	/** Settle time from LidPeakAngle back to LidOpenAngle, seconds (task-309 Q18: 0.15, provisional; ease-in-out). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bonus Chest|Opening", meta = (ClampMin = 0, Units = "s"))
	float LidSettleDuration = 0.15f;

	/**
	 * Sign of the lid relative Roll that opens it up and backward: -1 (task-309-07 cut report: Roll -105 lifts +Y of Mesh space
	 * into +Z; derived, not measured in the editor; task-309-05 checks it and flips it here if wrong).
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bonus Chest|Opening", meta = (ClampMin = -1, ClampMax = 1))
	float LidRollSign = -1.f;

	/** Camera shake played at the end of the hold (task-310-01); None = URRCameraShake (chest preset). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bonus Chest|Opening")
	TSubclassOf<UCameraShakeBase> LidSlamShakeClass;

	/** Amplitude multiplier of the slam shake (1 = preset strength, the light shake of task-309 Q19; 0 = none). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bonus Chest|Opening", meta = (ClampMin = 0))
	float LidSlamShakeScale = 1.f;

	/** Seconds InnerLight stays at its Blueprint intensity after the lid opening (task-309 Q22: 3, provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bonus Chest|Opening", meta = (ClampMin = 0, Units = "s"))
	float InnerLightOnDuration = 3.f;

	/** Seconds of the InnerLight fade to 0 after InnerLightOnDuration (implementation choice: 0.5). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bonus Chest|Opening", meta = (ClampMin = 0, Units = "s"))
	float InnerLightFadeDuration = 0.5f;

	/** Material slot of Mesh (SM_Chest_Body) lit at the lid opening (MI_ChestInterior, task-309-02). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bonus Chest|Opening")
	FName InteriorSlotName = TEXT("Interior");

	/** Scalar parameter of the Interior material driven at the lid opening (MI_ChestInterior: 0 by default = off). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bonus Chest|Opening")
	FName InteriorGlowParameterName = TEXT("InteriorGlow");

	/** InteriorGlow value written at the lid opening and kept (implementation choice: 1 = the full emissive of MI_ChestInterior). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bonus Chest|Opening", meta = (ClampMin = 0))
	float InteriorGlowOnValue = 1.f;

	/** Scale pop of the reward chips: 0 to 1.3 to 1.0 in 0.25 s (task-309 Q25, provisional; peak at 60 % of the time, implementation choice). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bonus Chest|Opening")
	FRRPickupScalePop ChipScalePop = FRRPickupScalePop(0.f, 1.3f, 0.25f, 0.6f);

	/** Lid component found by LidComponentName (null headless). */
	UPROPERTY(Transient)
	TObjectPtr<USceneComponent> LidComp;

	/** Latch component found by LatchComponentName (null headless). */
	UPROPERTY(Transient)
	TObjectPtr<USceneComponent> LatchComp;

	/** Dynamic instance of the Interior slot (null without the slot). */
	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> InteriorMID;

	/** Lid relative rotation when the opening started (closed pose). */
	FQuat LidClosedRotation = FQuat::Identity;

	/** Latch relative rotation when the opening started (closed pose). */
	FQuat LatchClosedRotation = FQuat::Identity;

	/** InnerLight intensity set in Blueprint (read at BeginPlay), restored at the lid opening and faded from. */
	float InnerLightOnIntensity = 0.f;

	/** Seconds since the completion. */
	float OpeningElapsed = 0.f;

	/** Current lid angle, degrees (magnitude). */
	float LidAngle = 0.f;

	/** Current latch angle, degrees (magnitude). */
	float LatchAngle = 0.f;

	/** True once the completion started the opening. */
	bool bOpeningStarted = false;

	/** True once the lid opening moment ran. */
	bool bLidOpened = false;

	/** True once OnLidSlam fired. */
	bool bLidSlamFired = false;

	/** True once the slam shake was played. */
	bool bSlamShakePlayed = false;

	/** True once InnerLight faded out. */
	bool bInnerLightFaded = false;

	/** True once every opening step ran (the tick is then turned off). */
	bool bOpeningFinished = false;

	/**
	 * Effect components spawned by OnLidOpened and OnLidSlam (BP_BonusChest: NS_BonusChestOpen, NS_HeavySlamImpact through
	 * SpawnSystemAtLocation, owned by the world, not by the chest). Weak: auto-destroyed one-shots disappear on their own.
	 */
	TArray<TWeakObjectPtr<UFXSystemComponent>> OpeningEffects;

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
