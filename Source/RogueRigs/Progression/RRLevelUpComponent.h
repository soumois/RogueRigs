// Rogue Rigs - level-up flow of the miner: heal, effect event, delayed pause on the upgrade offer, chained choice screens.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/TimerHandle.h"
#include "Upgrades/RRUpgradeCard.h"
#include "RRLevelUpComponent.generated.h"

class APlayerController;
class UGameplayEffect;
class UNiagaraComponent;
class UNiagaraSystem;
class URRProgressionComponent;
class URRRunFlowSubsystem;
class URRUpgradeComponent;

/** Fired once per level gained, right after the heal: play the level-up feedback (ring, sound). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRROnLevelUpEffectSignature, int32, NewLevel);

/**
 * Fired when a choice screen must show (game paused), again after a reroll and for each chained screen: the cards, and
 * bCanReroll = a reroll is affordable now (URRLevelUpComponent::CanReroll; its cost: GetRerollCost).
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRROnUpgradeOfferShownSignature, const TArray<FRRUpgradeCard>&, Cards, bool, bCanReroll);

/** Fired after the last queued choice: hide the choice screen (the game resumes). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRROnUpgradeOfferClosedSignature);

/**
 * Level-up flow of the miner (default subobject LevelUp of ARRMiner; xp-level-up). At BeginPlay it listens to the owner's
 * URRProgressionComponent::OnLevelUp. For each level gained: applies HealEffectClass (URRGE_Heal, SetByCaller Data.Heal =
 * HealFraction x MaxHealth, clamped to MaxHealth), broadcasts OnLevelUpEffect and queues one choice. The first queued
 * choice arms a ChoiceDelay game-time timer (the game keeps running); when it elapses the game is paused through the owner's
 * player controller, the input mode switches to game-and-UI with the cursor visible, an offer of CardCount cards is built
 * by the owner's URRUpgradeComponent (BuildOfferWithOwnerLuck) and OnUpgradeOfferShown fires. ChooseUpgrade applies a
 * card; the next queued choice shows at once (still paused) or, after the last, OnUpgradeOfferClosed fires, the game
 * resumes and game input returns. RequestReroll pays the reroll in ore (URRRunFlowSubsystem::SpendOre, carried then secured;
 * cost from URRRerollCostLibrary: 15, 20, 25... per reroll count) and forwards to the offer builder; the count returns to 0
 * when a level-up screen opens (chained ones included) and is kept by a bonus chest screen. Levels gained during the delay or while a
 * screen is open join the queue. No pause when the miner died before the delay ended.
 * Level-up burst (xp-level-up, task-441): once per level-up moment, whatever the levels gained in it (BurstMinInterval), the gold
 * LevelUpTextSystem plays above the miner. It does not go through URRDamageNumberSubsystem: the player toggle does not hide it.
 * The same moment plays LevelUpWaveSystem at his feet (WaveRadiusParameter = PushRadius) and launches every living robot within
 * PushRadius away from him (ARREnemy::ApplyKnockback, PushSpeed, following KnockbackResistance), with no damage. The push is
 * one blow from where he stands then; the text stays where it appeared; the wave follows him while it plays (task-449): the
 * component ticks only then (TG_PostPhysics, after his movement) and moves the wave back under his feet each frame.
 */
UCLASS(ClassGroup = (RogueRigs), meta = (BlueprintSpawnableComponent))
class ROGUERIGS_API URRLevelUpComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/** Niagara user parameter (float) of LevelUpWaveSystem: radius (uu) the wave reaches, PushRadius. */
	static const FName WaveRadiusParameter;

	/** Components each burst effect may hold in its pool: bursts are at least BurstMinInterval apart, so few play at once. */
	static constexpr int32 BurstPoolSize = 2;

	/** Tick off by default; on only while a level-up wave plays (TG_PostPhysics). */
	URRLevelUpComponent();

	/** Keeps the playing level-up wave under the owner's feet; turns the tick off once the wave finished. */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** The pooled wave component that follows the owner now, or null when no wave plays (tests, debug). Do not keep it. */
	UNiagaraComponent* GetFollowedWave() const;

	/** World location of the wave: under the owner's capsule, LevelUpWaveFloorOffset above the floor. */
	FVector GetWaveLocation() const;

	/**
	 * Applies card Index of the current offer (URRUpgradeComponent::ApplyUpgrade), then shows the next queued choice or ends
	 * the pause. Returns false (nothing changes) when no screen is open, Index is out of range or the card could not apply.
	 */
	UFUNCTION(BlueprintCallable, Category = "LevelUp")
	bool ChooseUpgrade(int32 Index);

	/**
	 * Pays GetRerollCost ore (URRRunFlowSubsystem::SpendOre: carried first, then secured), rerolls the current offer
	 * (URRUpgradeComponent::Reroll), counts the reroll and broadcasts OnUpgradeOfferShown with the new cards and CanReroll() for
	 * the next cost. Returns false (nothing spent) when no screen is open or the miner cannot pay.
	 */
	UFUNCTION(BlueprintCallable, Category = "LevelUp")
	bool RequestReroll();

	/** True while a choice screen is open (game paused for the choice). */
	UFUNCTION(BlueprintPure, Category = "LevelUp")
	bool IsAwaitingChoice() const { return bAwaitingChoice; }

	/**
	 * The run ended: clears the choice delay and the queue, closes an open screen (OnUpgradeOfferClosed, unpauses, restores the
	 * input) and blocks every later screen (level-ups still heal). Called by URRRunFlowSubsystem::EndRun before its end pause.
	 */
	void CancelChoices();

	/** Next run level started in the same world (task-318-01): lifts the CancelChoices block, as a new level's miner starts. */
	void ResumeChoices() { bChoicesBlocked = false; }

	/**
	 * Queues one extra choice that is not a level-up (bonus chest, objective-bonus-chest): offer level 0 (no weapon card), drawn with
	 * the owner luck + BonusLuck for that offer only (its reroll keeps it). No heal, no OnLevelUpEffect; same delay, pause and
	 * chaining as a level-up choice. Returns false when choices are blocked (run over) or the owner is dead.
	 */
	UFUNCTION(BlueprintCallable, Category = "LevelUp")
	bool QueueBonusChoice(float BonusLuck);

	/** True once CancelChoices ran: no more choice screens. */
	UFUNCTION(BlueprintPure, Category = "LevelUp")
	bool AreChoicesBlocked() const { return bChoicesBlocked; }

	/** True while the ChoiceDelay timer runs (level gained, game not paused yet). */
	UFUNCTION(BlueprintPure, Category = "LevelUp")
	bool IsChoiceDelayActive() const;

	/** Choices still owed, the open screen included. */
	UFUNCTION(BlueprintPure, Category = "LevelUp")
	int32 GetPendingChoices() const { return PendingChoices; }

	/** Cards of the open screen (the upgrade component's current offer; empty when none). */
	UFUNCTION(BlueprintPure, Category = "LevelUp")
	TArray<FRRUpgradeCard> GetOfferCards() const;

	/** True while a screen is open and the miner can pay GetRerollCost (carried plus secured ore). */
	UFUNCTION(BlueprintPure, Category = "LevelUp")
	bool CanReroll() const;

	/** Ore cost of the next reroll: URRRerollCostLibrary::GetRerollCostForCount(GetRerollCount()). */
	UFUNCTION(BlueprintPure, Category = "LevelUp")
	int32 GetRerollCost() const;

	/** Rerolls paid since the last level-up screen opened (a bonus chest screen keeps the count). */
	UFUNCTION(BlueprintPure, Category = "LevelUp")
	int32 GetRerollCount() const { return RerollCount; }

	/** Game-time delay between a level-up and the pause on its choice screen, in seconds. */
	float GetChoiceDelay() const { return ChoiceDelay; }

	/** Fraction of MaxHealth restored per level gained. */
	float GetHealFraction() const { return HealFraction; }

	/** Level-up bursts played since BeginPlay: one per level-up moment, several levels gained at once count once. */
	UFUNCTION(BlueprintPure, Category = "LevelUp")
	int32 GetLevelUpBurstCount() const { return LevelUpBurstCount; }

	/** Broadcast once per level gained, after the heal. Bind for the ring, the sound and any world-space feedback. */
	UPROPERTY(BlueprintAssignable, Category = "LevelUp")
	FRROnLevelUpEffectSignature OnLevelUpEffect;

	/** Broadcast when a choice screen opens (game paused), after a reroll and for each chained screen. Bind to show the cards. */
	UPROPERTY(BlueprintAssignable, Category = "LevelUp")
	FRROnUpgradeOfferShownSignature OnUpgradeOfferShown;

	/** Broadcast after the last queued choice, just before the game resumes. Bind to hide the choice screen. */
	UPROPERTY(BlueprintAssignable, Category = "LevelUp")
	FRROnUpgradeOfferClosedSignature OnUpgradeOfferClosed;

protected:
	/** Binds the owner's progression OnLevelUp and finds the owner's upgrade component. */
	virtual void BeginPlay() override;

	/** Unbinds, clears the delay timer and ends a pause still held by the flow. */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Progression OnLevelUp callback: heal, OnLevelUpEffect, queue one choice (arms the delay when none is running or open). */
	UFUNCTION()
	void HandleLevelUp(int32 NewLevel);

	/** Adds one owed choice (offer level, bonus luck) and arms the delay when none is running or open. */
	void QueueChoice(int32 OfferLevel, float BonusLuck);

	/** Applies HealEffectClass with SetByCaller Data.Heal = HealFraction x MaxHealth to the owner's ASC (skipped when dead). */
	void ApplyLevelUpHeal();

	/** One level-up moment (skipped inside BurstMinInterval of the last one, when dead or once the run is over): text, wave, robot push. */
	void PlayLevelUpBurst();

	/** Launches every living robot within PushRadius (2D) of the owner away from him; a robot exactly on him has no direction and stays. */
	void PushNearbyRobots();

	/** Delay timer callback: pauses and shows the first queued choice (drops the queue when the owner died). */
	void HandleChoiceDelayElapsed();

	/** Builds an offer and broadcasts OnUpgradeOfferShown; with no card (tables missing) drops the queue and ends the pause. */
	void ShowNextOffer();

	/** Pauses the game and switches the owner's player controller to game-and-UI input with the cursor shown. */
	void EnterChoicePause();

	/** Unpauses (only a pause the flow set) and restores game-only input and the previous cursor visibility. */
	void ExitChoicePause();

	/** Clears the queue, broadcasts OnUpgradeOfferClosed and ends the pause. */
	void CloseChoice();

	/** The run flow of this world (pays the rerolls), or null outside Game and PIE worlds. */
	URRRunFlowSubsystem* GetRunFlow() const;

	/** True when the owner is an ARRMiner that died. */
	bool IsOwnerDead() const;

	/** The player controller of the owning pawn, or null (headless tests have none). */
	APlayerController* GetOwnerPlayerController() const;

	/** Game-time delay between a level-up and the pause on its choice screen, in seconds (xp-level-up, about 1.8 s, provisional, task-459). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LevelUp", meta = (ClampMin = 0, Units = "s"))
	float ChoiceDelay = 1.8f;

	/** Fraction of MaxHealth restored per level gained (xp-level-up, 20 %, provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LevelUp", meta = (ClampMin = 0, ClampMax = 1))
	float HealFraction = 0.2f;

	/** Cards per choice screen (xp-level-up: 3). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LevelUp", meta = (ClampMin = 1))
	int32 CardCount = 3;

	/** Instant GE of the level-up heal; reads SetByCaller Data.Heal. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LevelUp")
	TSubclassOf<UGameplayEffect> HealEffectClass;

	/** One-shot gold "LEVEL UP" text (NS_LevelUpText, set in BP_Miner): null = no text. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LevelUp|Burst")
	TObjectPtr<UNiagaraSystem> LevelUpTextSystem;

	/** Height (uu) of the text above the capsule top (as the heal and ore numbers: 40, provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LevelUp|Burst", meta = (Units = "cm"))
	float LevelUpTextHeight = 40.f;

	/** One-shot gold floor wave (NS_LevelUpWave, set in BP_Miner), played at the owner's feet: null = no wave, the push still applies. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LevelUp|Burst")
	TObjectPtr<UNiagaraSystem> LevelUpWaveSystem;

	/** Height (uu) of the wave above the floor under the miner, so its sprites do not fight with the floor (provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LevelUp|Burst", meta = (Units = "cm"))
	float LevelUpWaveFloorOffset = 5.f;

	/** Radius (uu) of the wave and of the push (xp-level-up: about 300, provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LevelUp|Burst", meta = (ClampMin = 0, Units = "cm"))
	float PushRadius = 300.f;

	/** Launch speed (uu/s) of a robot with no knockback resistance (xp-level-up: a slight push, provisional, the owner tunes it in game). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LevelUp|Burst", meta = (ClampMin = 0))
	float PushSpeed = 800.f;

	/** Level-ups closer than this to the last burst belong to the same moment: no second burst (the text life, 0.7 s, provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LevelUp|Burst", meta = (ClampMin = 0, Units = "s"))
	float BurstMinInterval = 0.7f;

	/** Progression whose OnLevelUp is bound. */
	TWeakObjectPtr<URRProgressionComponent> BoundProgression;

	/** Upgrade component that builds and applies the offers. */
	TWeakObjectPtr<URRUpgradeComponent> UpgradeComponent;

	/** Controller paused by the flow (restored at the end). */
	TWeakObjectPtr<APlayerController> PausingController;

	/** ChoiceDelay timer. */
	FTimerHandle ChoiceDelayTimer;

	/** Pooled wave component of the last burst while it plays; the tick moves it under the owner. */
	TWeakObjectPtr<UNiagaraComponent> FollowedWave;

	/** World time of the last burst; negative before the first one. */
	double LastBurstTime = -1.0;

	/** Bursts played (GetLevelUpBurstCount). */
	int32 LevelUpBurstCount = 0;

	/** Choices still owed, the open screen included. */
	int32 PendingChoices = 0;

	/** Level of each owed choice, oldest first (the open screen's level first): weapon cards come on levels 5, 10, 15... */
	TArray<int32> PendingLevels;

	/** Bonus luck of each owed choice, parallel to PendingLevels (0 for a level-up, the chest bonus for a chest choice). */
	TArray<float> PendingBonusLuck;

	/** Rerolls paid since the last level-up screen opened; reset by ShowNextOffer for a level-up offer (OfferLevel > 0). */
	int32 RerollCount = 0;

	/** True while a choice screen is open. */
	bool bAwaitingChoice = false;

	/** True when the flow's SetPause succeeded (only then does it unpause). */
	bool bPausedByFlow = false;

	/** True once CancelChoices ran (run over): level-ups queue no choice. */
	bool bChoicesBlocked = false;

	/** Cursor visibility before the pause, restored after it. */
	bool bCursorShownBeforePause = true;
};
