// Rogue Rigs - polls the URRCheatSettings default object every frame (paused too) and applies the cheats it requests to its world.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "RRCheatSubsystem.generated.h"

/**
 * Property route of the cheats (sys-cheats): each tick, paused or not, reads the URRCheatSettings class default object,
 * applies the requested cheats to this world through the shared RRCheats actions and resets the one-shot fields.
 * bInvulnerable is applied on change (and on the first tick when true): a true value with no miner yet is retried until one
 * is tagged. Game and PIE worlds; never in shipping builds.
 */
UCLASS()
class ROGUERIGS_API URRCheatSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	/** Game and PIE worlds of non-shipping builds. */
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

	/** Applies and resets the requested cheats. */
	virtual void Tick(float DeltaTime) override;

	/** Ticks while the game is paused (the level-up screen pauses it). */
	virtual bool IsTickableWhenPaused() const override { return true; }

	/** Stat id of the tick. */
	virtual TStatId GetStatId() const override;

protected:
	/** bInvulnerable value last applied by this subsystem (false at start). */
	bool bAppliedInvulnerable = false;
};
