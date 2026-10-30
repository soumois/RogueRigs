// Rogue Rigs - secured ore of the session: a game-instance subsystem, so the total survives a level restart (OpenLevel).

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "RROreBankSubsystem.generated.h"

/**
 * Secured ore of the play session (ore-economy): the run flow adds the carried ore here when the miner boards the shuttle
 * (URRRunFlowSubsystem::EndRun success). Lives on the game instance, so it survives URRRunFlowSubsystem::RestartLevel; it is
 * not saved to disk (meta-progression is deferred). Read it through URRRunFlowSubsystem::GetSecuredOre and the run flow
 * OnOreChanged event, which also cover worlds without an initialized game instance (headless tests). Paid rerolls spend it
 * through URRRunFlowSubsystem::SpendOre (RemoveSecuredOre). URRRunSubsystem::ResetRun clears it at every run end (lost, won, restart, New Run), so ore lasts one run.
 * Next to the spendable balance it keeps the ore secured since the last reset, which spending never lowers (task-391-01): the
 * run recap "Total Ore" and the run won end screen read it through URRRunSubsystem::GetRunSecuredOre.
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

	/** Ore secured since the last ResetSecuredOre (the run start): every AddSecuredOre, spending not subtracted (task-391-01). */
	UFUNCTION(BlueprintPure, Category = "Ore")
	int32 GetSecuredSinceReset() const { return SecuredSinceReset; }

	/** Adds Amount (> 0) to the secured balance and to the secured-since-reset total; returns the new balance. Called by the run flow on extraction success. */
	int32 AddSecuredOre(int32 Amount);

	/** Removes Amount (> 0, at most the total) from the secured total; returns the new total. Called by URRRunFlowSubsystem::SpendOre. */
	int32 RemoveSecuredOre(int32 Amount);

	/** Clears the secured balance and the secured-since-reset total (ore lasts one run, ore-economy); returns the balance cleared. Called by URRRunSubsystem::ResetRun. */
	int32 ResetSecuredOre();

protected:
	/** Spendable secured ore (starts at 0 with the game instance). */
	int32 SecuredOre = 0;

	/** Ore secured since the last ResetSecuredOre; only AddSecuredOre raises it and only ResetSecuredOre clears it. */
	int32 SecuredSinceReset = 0;
};
