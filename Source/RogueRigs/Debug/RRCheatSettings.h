// Rogue Rigs - cheat properties on a class default object, polled every frame: lets Unreal MCP (which sets object properties but not console variables) drive PIE checks.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RRCheatSettings.generated.h"

/**
 * Cheat switches read from the class default object (/Script/RogueRigs.Default__RRCheatSettings) by URRCheatSubsystem every
 * frame, paused or not, in each game and PIE world. Set them with ObjectTools.set_properties on that path during PIE.
 * One-shot fields (GrantXP, ChooseUpgrade, bReroll, bSkipToConsole, bCompleteConsoleHack, bSkipToExtraction) are applied once
 * by the next world tick and reset to their neutral value; read them back to see they were consumed. bInvulnerable and
 * AlertSpeedMultiplier are persistent. The CDO lives as long as the editor process: reset persistent values after a check.
 * Debug tooling only; nothing reads it in shipping builds except AlertSpeedMultiplier (stays 1).
 */
UCLASS(NotBlueprintable, Transient)
class ROGUERIGS_API URRCheatSettings : public UObject
{
	GENERATED_BODY()

public:
	/** One-shot: N > 0 adds N raw XP (XPGain applies) to the player miner; reset to 0. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cheats")
	float GrantXP = 0.f;

	/** Persistent: true keeps every miner of the world invulnerable (loose State.Invulnerable, re-applied to new worlds); false removes it. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cheats")
	bool bInvulnerable = false;

	/** One-shot: 0..2 picks that card of the open level-up screen; reset to -1. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cheats")
	int32 ChooseUpgrade = -1;

	/** One-shot: true rerolls the open level-up screen; reset to false. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cheats")
	bool bReroll = false;

	/** Persistent: alert gain multiplier (time and kills) of the run flow; 1 = normal, values <= 0 count as 1. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cheats")
	float AlertSpeedMultiplier = 1.f;

	/** One-shot: true makes the alert jump to the console threshold and drops the console now; reset to false. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cheats")
	bool bSkipToConsole = false;

	/** One-shot: true completes the pending console hack (the alert resumes); reset to false. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cheats")
	bool bCompleteConsoleHack = false;

	/** One-shot: true completes or skips the console and fills the alert (extraction ready); reset to false. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cheats")
	bool bSkipToExtraction = false;
};
