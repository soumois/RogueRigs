// Rogue Rigs - cheat actions shared by the rr.Cheat.* console variables and the polled URRCheatSettings properties.

#pragma once

#include "CoreMinimal.h"

class ARRMiner;
class UWorld;

/**
 * Cheat actions (sys-cheats). Each takes the world it acts on and a Source label for the log ("rr.Cheat" for the console
 * variables, "RRCheatSettings" for the property route); every log line starts with "rr.Cheat.<Name>" so one log pattern finds
 * both routes. Debug tooling only.
 */
namespace RRCheats
{
	/** The world the console variables act on: the first PIE world, else the first game world (headless tests, -game). */
	ROGUERIGS_API UWorld* FindCheatWorld();

	/** The miner of player 0 in World, else the first live miner found. */
	ROGUERIGS_API ARRMiner* FindPlayerMiner(UWorld* World);

	/** Adds Amount raw XP (XPGain applies) to the player miner of World. False without a miner with a progression. */
	ROGUERIGS_API bool GrantXP(UWorld* World, float Amount, const TCHAR* Source);

	/**
	 * true: adds one loose State.Invulnerable count to every miner of World not tagged by the cheat yet; returns the miners of
	 * World tagged afterwards. false: removes every count the cheat added (all worlds); returns how many were removed.
	 */
	ROGUERIGS_API int32 SetInvulnerable(UWorld* World, bool bInvulnerable, const TCHAR* Source);

	/** Picks card Index of the open level-up screen of the player miner. False when refused. */
	ROGUERIGS_API bool ChooseUpgrade(UWorld* World, int32 Index, const TCHAR* Source);

	/** Rerolls the open level-up screen of the player miner. False when refused. */
	ROGUERIGS_API bool Reroll(UWorld* World, const TCHAR* Source);

	/** URRRunFlowSubsystem::SkipToConsole of World. False when no console exists afterwards. */
	ROGUERIGS_API bool SkipToConsole(UWorld* World, const TCHAR* Source);

	/** URRRunFlowSubsystem::CompleteConsoleHack of World. False when no console was pending. */
	ROGUERIGS_API bool CompleteConsoleHack(UWorld* World, const TCHAR* Source);

	/** URRRunFlowSubsystem::SkipToExtraction of World. False when extraction is not ready afterwards. */
	ROGUERIGS_API bool SkipToExtraction(UWorld* World, const TCHAR* Source);
}
