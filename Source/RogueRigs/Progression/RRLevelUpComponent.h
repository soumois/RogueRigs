// Rogue Rigs - level-up flow of the miner: heal, effect event, delayed pause on the upgrade offer, chained choice screens.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/TimerHandle.h"
#include "Upgrades/RRUpgradeCard.h"
#include "RRLevelUpComponent.generated.h"

class APlayerController;
class UGameplayEffect;
class URRProgressionComponent;
class URRUpgradeComponent;

/** Fired once per level gained, right after the heal: play the level-up feedback (ring, sound). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRROnLevelUpEffectSignature, int32, NewLevel);

/** Fired when a choice screen must show (game paused), again after a reroll and for each chained screen: the cards, reroll available. */
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
 * resumes and game input returns. RequestReroll forwards to the offer builder. Levels gained during the delay or while a
 * screen is open join the queue. No pause when the miner died before the delay ended.
 */
UCLASS(ClassGroup = (RogueRigs), meta = (BlueprintSpawnableComponent))
class ROGUERIGS_API URRLevelUpComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/** No tick. */
	URRLevelUpComponent();

	/**
	 * Applies card Index of the current offer (URRUpgradeComponent::ApplyUpgrade), then shows the next queued choice or ends
	 * the pause. Returns false (nothing changes) when no screen is open, Index is out of range or the card could not apply.
	 */
	UFUNCTION(BlueprintCallable, Category = "LevelUp")
	bool ChooseUpgrade(int32 Index);

	/**
	 * Rerolls the current offer once (URRUpgradeComponent::Reroll) and broadcasts OnUpgradeOfferShown with the new cards and
	 * bCanReroll false. Returns false when no screen is open or the reroll was used.
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

	/** True while the open screen can still be rerolled. */
	UFUNCTION(BlueprintPure, Category = "LevelUp")
	bool CanReroll() const;

	/** Game-time delay between a level-up and the pause on its choice screen, in seconds. */
	float GetChoiceDelay() const { return ChoiceDelay; }

	/** Fraction of MaxHealth restored per level gained. */
	float GetHealFraction() const { return HealFraction; }

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

	/** True when the owner is an ARRMiner that died. */
	bool IsOwnerDead() const;

	/** The player controller of the owning pawn, or null (headless tests have none). */
	APlayerController* GetOwnerPlayerController() const;

	/** Game-time delay between a level-up and the pause on its choice screen, in seconds (xp-level-up, about 0.8 s, provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LevelUp", meta = (ClampMin = 0, Units = "s"))
	float ChoiceDelay = 0.8f;

	/** Fraction of MaxHealth restored per level gained (xp-level-up, 20 %, provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LevelUp", meta = (ClampMin = 0, ClampMax = 1))
	float HealFraction = 0.2f;

	/** Cards per choice screen (xp-level-up: 3). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LevelUp", meta = (ClampMin = 1))
	int32 CardCount = 3;

	/** Instant GE of the level-up heal; reads SetByCaller Data.Heal. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LevelUp")
	TSubclassOf<UGameplayEffect> HealEffectClass;

	/** Progression whose OnLevelUp is bound. */
	TWeakObjectPtr<URRProgressionComponent> BoundProgression;

	/** Upgrade component that builds and applies the offers. */
	TWeakObjectPtr<URRUpgradeComponent> UpgradeComponent;

	/** Controller paused by the flow (restored at the end). */
	TWeakObjectPtr<APlayerController> PausingController;

	/** ChoiceDelay timer. */
	FTimerHandle ChoiceDelayTimer;

	/** Choices still owed, the open screen included. */
	int32 PendingChoices = 0;

	/** Level of each owed choice, oldest first (the open screen's level first): weapon cards come on levels 5, 10, 15... */
	TArray<int32> PendingLevels;

	/** Bonus luck of each owed choice, parallel to PendingLevels (0 for a level-up, the chest bonus for a chest choice). */
	TArray<float> PendingBonusLuck;

	/** True while a choice screen is open. */
	bool bAwaitingChoice = false;

	/** True when the flow's SetPause succeeded (only then does it unpause). */
	bool bPausedByFlow = false;

	/** True once CancelChoices ran (run over): level-ups queue no choice. */
	bool bChoicesBlocked = false;

	/** Cursor visibility before the pause, restored after it. */
	bool bCursorShownBeforePause = true;
};
