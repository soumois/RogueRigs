// Rogue Rigs - run lost (death-screen gdd, task-484): death sequence (slow motion, then pause), run-lost recap and the Retry and
// Main Menu actions of the run-lost screen, for a miner death and a missed shuttle.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "RRRunLostSubsystem.generated.h"

/** What the run-lost screen shows (death-screen gdd, Texts and Recap). */
USTRUCT(BlueprintType)
struct ROGUERIGS_API FRRRunLostRecap
{
	GENERATED_BODY()

	/** True when the miner died; false when the shuttle left without him. */
	UPROPERTY(BlueprintReadOnly, Category = "Run Lost")
	bool bMinerDied = false;

	/** screen_texts.json item of the title and subtitle: run_lost_death or run_lost_shuttle. */
	UPROPERTY(BlueprintReadOnly, Category = "Run Lost")
	FName ScreenTextId;

	/** Title from the screen text file; "RUN LOST" when the item is missing. */
	UPROPERTY(BlueprintReadOnly, Category = "Run Lost")
	FText Title;

	/** Subtitle from the screen text file; empty when the item is missing. */
	UPROPERTY(BlueprintReadOnly, Category = "Run Lost")
	FText Subtitle;

	/** Run level the run was lost on (1-based; 1 without a run in progress). */
	UPROPERTY(BlueprintReadOnly, Category = "Run Lost")
	int32 LevelReached = 1;

	/** Levels of a run (URRRunSettings::LevelLayouts, at least 1). */
	UPROPERTY(BlueprintReadOnly, Category = "Run Lost")
	int32 LevelCount = 1;

	/** Robots destroyed over the whole run (run log kills of every level of the run, this one included). */
	UPROPERTY(BlueprintReadOnly, Category = "Run Lost")
	int32 RobotsDestroyed = 0;

	/** Duskite the miner held when the run was lost (carried + secured, before the loss emptied it). */
	UPROPERTY(BlueprintReadOnly, Category = "Run Lost")
	int32 DuskiteHeld = 0;

	/** "Level 2 of 4". */
	UPROPERTY(BlueprintReadOnly, Category = "Run Lost")
	FText LevelText;

	/** "47". */
	UPROPERTY(BlueprintReadOnly, Category = "Run Lost")
	FText RobotsText;

	/** "61". */
	UPROPERTY(BlueprintReadOnly, Category = "Run Lost")
	FText DuskiteText;
};

/** Fired once per world when the run-lost screen must show (after the death sequence, or at once for a missed shuttle). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRROnRunLostSignature, const FRRRunLostRecap&, Recap);

/**
 * Run lost of one level world (death-screen gdd; task-484). Game and PIE worlds.
 * Death: URRRunFlowSubsystem::HandleMinerDeath calls BeginDeathSequence with the Duskite held before the loss empties it. The
 * miner's level-up offer is cancelled, the global time dilation becomes URRRunSettings::DeathTimeDilation (controls are already cut
 * by ARRGameMode and the miner), and after DeathSequenceRealSeconds of real time (UWorld::GetRealTimeSeconds) the dilation is
 * restored, player 0's controller pauses the game (Game and UI input, cursor shown) and OnRunLost fires with the recap.
 * Missed shuttle: URRRunFlowSubsystem::EndRun(false) calls ShowShuttleMissed after its own pause: OnRunLost at once.
 * Buttons: Retry = main menu New Run (URRRunTransitionSubsystem::StartNewRun on URRRunSettings::NewRunMapName: run reset, loading
 * screen, level 1); Main Menu = URRRunFlowSubsystem::ReturnToMainMenu (run reset, L_MainMenu opened directly). Each refuses a
 * second press. ARRPlayerController creates the screen (RunLostWidgetClass) on OnRunLost.
 */
UCLASS()
class ROGUERIGS_API URRRunLostSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	/** The subsystem of WorldContext's world; null outside a game or PIE world. */
	static URRRunLostSubsystem* Get(const UObject* WorldContext);

	/** Game and PIE worlds only. */
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

	/** Restores the time dilation if the world ends during the death sequence. */
	virtual void Deinitialize() override;

	/** Ends the death sequence once its real time has passed. */
	virtual void Tick(float DeltaTime) override;

	/** Ticks only during the death sequence. */
	virtual bool IsTickable() const override { return bDeathSequence; }

	/** Stat id of the tick. */
	virtual TStatId GetStatId() const override;

	/**
	 * Miner death: starts the death sequence once (later calls and a run already lost are ignored). DuskiteHeld = carried + secured
	 * ore before the death emptied the carried ore. False when ignored.
	 */
	bool BeginDeathSequence(int32 DuskiteHeld);

	/** Missed shuttle: OnRunLost at once (the run flow already paused the game). False when the run was already lost. */
	bool ShowShuttleMissed(int32 DuskiteHeld);

	/** Retry button: new run at level 1 through the loading screen, as the main menu New Run. False when refused (logged). */
	UFUNCTION(BlueprintCallable, Category = "Run Lost")
	bool Retry();

	/** Main Menu button: run reset, L_MainMenu opened directly (URRRunFlowSubsystem::ReturnToMainMenu). False when refused. */
	UFUNCTION(BlueprintCallable, Category = "Run Lost")
	bool ReturnToMainMenu();

	/** True between the lethal hit and the run-lost screen. */
	UFUNCTION(BlueprintPure, Category = "Run Lost")
	bool IsDeathSequenceRunning() const { return bDeathSequence; }

	/** True once OnRunLost fired in this world. */
	UFUNCTION(BlueprintPure, Category = "Run Lost")
	bool IsRunLostShown() const { return bRunLostShown; }

	/** Recap sent with OnRunLost (default before). */
	UFUNCTION(BlueprintPure, Category = "Run Lost")
	const FRRRunLostRecap& GetRecap() const { return Recap; }

	/** Builds the recap of this world now: level, run kills, the given Duskite, texts of the death or shuttle item. */
	FRRRunLostRecap BuildRecap(bool bMinerDied, int32 DuskiteHeld) const;

	/** See FRROnRunLostSignature. */
	UPROPERTY(BlueprintAssignable, Category = "Run Lost")
	FRROnRunLostSignature OnRunLost;

protected:
	/** End of the death sequence: dilation restored, game paused with UI input, OnRunLost. */
	void FinishDeathSequence();

	/** Pauses player 0's controller (when there is one) with Game and UI input and the cursor shown. */
	void PauseForScreen();

	/** Stores Recap, marks the screen shown and broadcasts OnRunLost. */
	void ShowRunLost(const FRRRunLostRecap& InRecap);

	/** Sets the global time dilation back to the value before the death sequence. */
	void RestoreTimeDilation();

	/** True during the death sequence. */
	bool bDeathSequence = false;

	/** True once OnRunLost fired. */
	bool bRunLostShown = false;

	/** True once Retry or ReturnToMainMenu requested a travel. */
	bool bLeaving = false;

	/** True while the time dilation is changed by the death sequence. */
	bool bTimeDilated = false;

	/** Global time dilation before the death sequence. */
	float TimeDilationBefore = 1.f;

	/** Real time (UWorld::GetRealTimeSeconds) of the lethal hit. */
	double DeathRealSeconds = 0.0;

	/** Game time of the lethal hit (log only). */
	double DeathGameSeconds = 0.0;

	/** Duskite held at death, kept for the recap built at the end of the sequence. */
	int32 PendingDuskiteHeld = 0;

	/** Recap of the shown screen. */
	UPROPERTY(Transient)
	FRRRunLostRecap Recap;
};
