// Rogue Rigs - game mode of a run: game over on the miner's death, then level restart.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RRGameMode.generated.h"

class ARRMiner;

/** Fired once when the run ends (the miner died). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRROnGameOverSignature);

/**
 * Game mode of a run. DefaultPawnClass (Miner Blueprint) and PlayerControllerClass (player controller
 * Blueprint) are set in the Blueprint subclass, because ARRMiner and ARRPlayerController are abstract
 * and need their assets.
 * Game over: each miner registers itself at BeginPlay (RegisterMiner). On the first miner death the game mode
 * disables the input of the miner's player controller, broadcasts OnGameOver once and arms a timer that reopens
 * the current level after RestartDelay seconds.
 */
UCLASS(Abstract)
class ROGUERIGS_API ARRGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	/** Listens to Miner's OnDeath for the game over. Called by ARRMiner::BeginPlay; safe to call twice. */
	void RegisterMiner(ARRMiner* Miner);

	/** True once the game over happened (the restart is pending). */
	UFUNCTION(BlueprintPure, Category = "Run")
	bool IsGameOver() const { return bGameOver; }

	/** Delay in seconds between the game over and the level restart. */
	UFUNCTION(BlueprintPure, Category = "Run")
	float GetRestartDelay() const { return RestartDelay; }

	/** Seconds left before the level restart; -1 when no restart is armed. */
	UFUNCTION(BlueprintPure, Category = "Run")
	float GetRestartTimeRemaining() const;

	/** Broadcast once at the game over, after the player input is disabled. The HUD widget binds to it. */
	UPROPERTY(BlueprintAssignable, Category = "Run")
	FRROnGameOverSignature OnGameOver;

protected:
	/** Clears the restart timer. */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Miner OnDeath callback: first call only, disables the player input, broadcasts OnGameOver, arms the restart timer. */
	UFUNCTION()
	void HandleMinerDeath(AActor* DeathInstigator);

	/** Restart timer callback: reopens the current level (UGameplayStatics::OpenLevel on the current level name). */
	virtual void RestartLevel();

	/** Seconds between the game over and the level restart (miner gdd, M1: 3 s). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Run", meta = (ClampMin = 0.1, Units = "s"))
	float RestartDelay = 3.f;

	/** True once the game over happened; guards against a second game over. */
	bool bGameOver = false;

	/** Timer that calls RestartLevel after RestartDelay. */
	FTimerHandle RestartTimerHandle;
};
