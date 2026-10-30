// Rogue Rigs - secured ore of the session: a game-instance subsystem, so the total survives a level restart (OpenLevel).

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "RROreBankSubsystem.generated.h"

/**
 * Secured ore of the play session (ore-economy): the run flow adds the carried ore here when the miner boards the shuttle
 * (URRRunFlowSubsystem::EndRun success). Lives on the game instance, so it survives URRRunFlowSubsystem::RestartLevel; it is
 * not saved to disk (meta-progression is deferred). Read it through URRRunFlowSubsystem::GetSecuredOre and the run flow
 * OnOreChanged event, which also cover worlds without an initialized game instance (headless tests). Paid rerolls and shop
 * purchases spend it through URRRunFlowSubsystem::SpendOre (RemoveSecuredOre). URRRunSubsystem::ResetRun clears it at every run end (lost, won, restart, New Run), so ore lasts one run.
 * This balance is the one ore figure the player sees (task-391-04): HUD, stats screen Total Ore, shop balance, run won end screen.
 */
UCLASS()
class ROGUERIGS_API URROreBankSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/** The bank of WorldContext's game instance; null without an initialized game instance (bare test worlds). */
	static URROreBankSubsystem* Get(const UObject* WorldContext);

	/** Ore secured by successful extractions this session. */
	UFUNCTION(BlueprintPure, Category = "Ore")
	int32 GetSecuredOre() const { return SecuredOre; }

	/** Adds Amount (> 0) to the secured total; returns the new total. Called by the run flow on extraction success. */
	int32 AddSecuredOre(int32 Amount);

	/** Removes Amount (> 0, at most the total) from the secured total; returns the new total. Called by URRRunFlowSubsystem::SpendOre. */
	int32 RemoveSecuredOre(int32 Amount);

	/** Clears the secured total (ore lasts one run, ore-economy); returns the amount cleared. Called by URRRunSubsystem::ResetRun. */
	int32 ResetSecuredOre();

protected:
	/** Ore secured this session (starts at 0 with the game instance). */
	int32 SecuredOre = 0;
};
