// Rogue Rigs - game mode of a run: game over on the miner's death (the run-lost screen follows, task-484).

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Objectives/RRRunFlowConfig.h"
#include "RRGameMode.generated.h"

class ARRMiner;

/** Fired once when the run ends (the miner died). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRROnGameOverSignature);

/**
 * Game mode of a run. DefaultPawnClass (Miner Blueprint) and PlayerControllerClass (player controller
 * Blueprint) are set in the Blueprint subclass, because ARRMiner and ARRPlayerController are abstract
 * and need their assets.
 * Game over: each miner registers itself at BeginPlay (RegisterMiner). On the first miner death the game mode
 * disables the input of every player controller and broadcasts OnGameOver once. Nothing restarts on its own any more
 * (death-screen gdd, task-484): URRRunLostSubsystem plays the death sequence and shows the run-lost screen, whose Retry and
 * Main Menu buttons end the run.
 * Run flow: RunFlowConfig holds the alert and console tunables read by URRRunFlowSubsystem (set ConsoleClass to the console
 * Blueprint here).
 */
UCLASS(Abstract)
class ROGUERIGS_API ARRGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	/** Listens to Miner's OnDeath for the game over. Called by ARRMiner::BeginPlay; safe to call twice. */
	void RegisterMiner(ARRMiner* Miner);

	/** True once the game over happened (the miner died). */
	UFUNCTION(BlueprintPure, Category = "Run")
	bool IsGameOver() const { return bGameOver; }

	/** Tunables of the level run flow (alert bar, console drop); read once by URRRunFlowSubsystem. */
	const FRRRunFlowConfig& GetRunFlowConfig() const { return RunFlowConfig; }

	/** Broadcast once at the game over, after the player input is disabled. The HUD widget binds to it. */
	UPROPERTY(BlueprintAssignable, Category = "Run")
	FRROnGameOverSignature OnGameOver;

protected:
	/** Miner OnDeath callback: first call only, disables the player input, broadcasts OnGameOver. */
	UFUNCTION()
	void HandleMinerDeath(AActor* DeathInstigator);

	/** Alert and console tunables of the run flow (level-flow M1 defaults); ConsoleClass = the console Blueprint. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Run|Flow")
	FRRRunFlowConfig RunFlowConfig;

	/** True once the game over happened; guards against a second game over. */
	bool bGameOver = false;
};
