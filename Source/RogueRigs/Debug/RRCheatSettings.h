// Rogue Rigs - cheat properties on a class default object, polled every frame: lets Unreal MCP (which sets object properties but not console variables) drive PIE checks.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RRCheatSettings.generated.h"

/**
 * Cheat switches read from the class default object (/Script/RogueRigs.Default__RRCheatSettings) by URRCheatSubsystem every
 * frame, paused or not, in each game and PIE world. Set them with ObjectTools.set_properties on that path during PIE.
 * One-shot fields (GrantXP, GrantOre, ChooseUpgrade, bReroll, bSkipToConsole, bCompleteConsoleHack, bSkipToExtraction) are applied once
 * by the next world tick and reset to their neutral value; read them back to see they were consumed. bInvulnerable,
 * AlertSpeedMultiplier, ShuttleCountdownOverride and the LightHalo fields are persistent. The CDO lives as long as the editor process: reset persistent values after a check.
 * Debug tooling only; nothing reads it in shipping builds except AlertSpeedMultiplier (stays 1) and ShuttleCountdownOverride (stays 0).
 * Light halo (look test, task-026-04): bLightHalo adds a point light above the player miner and dims the directional and sky
 * lights plus the camera exposure; every LightHalo value is editable while it is on and applied on the next frame.
 */
UCLASS(NotBlueprintable, Transient)
class ROGUERIGS_API URRCheatSettings : public UObject
{
	GENERATED_BODY()

public:
	/** One-shot: N > 0 adds N raw XP (XPGain applies) to the player miner; reset to 0. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cheats")
	float GrantXP = 0.f;

	/** One-shot: N > 0 adds N carried ore (run flow ore counter); reset to 0. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cheats")
	int32 GrantOre = 0;

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

	/** Persistent: seconds > 0 replace the shuttle countdown of the next landing (short PIE checks of the run loss); 0 = the run flow value. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cheats")
	float ShuttleCountdownOverride = 0.f;

	/** Persistent: true adds a point light (halo) above the player miner and darkens the arena (sun and sky scaled, exposure bias); false restores. Default off. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cheats|LightHalo")
	bool bLightHalo = false;

	/** Halo point light intensity in candelas. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cheats|LightHalo", meta = (ClampMin = 0))
	float LightHaloIntensity = 60.f;

	/** Halo attenuation radius (uu): the lit disc around the miner. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cheats|LightHalo", meta = (ClampMin = 0))
	float LightHaloRadius = 900.f;

	/** Height of the halo light above the miner's origin (uu). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cheats|LightHalo")
	float LightHaloHeight = 250.f;

	/** Halo light colour (sRGB). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cheats|LightHalo")
	FLinearColor LightHaloColor = FLinearColor(1.f, 0.85f, 0.65f);

	/** True: the halo light casts shadows (rock walls cut the halo; costs more). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cheats|LightHalo")
	bool bLightHaloShadows = false;

	/** Multiplier of every directional and sky light intensity while the halo is on (1 = unchanged). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cheats|LightHalo", meta = (ClampMin = 0))
	float LightHaloAmbientScale = 0.3f;

	/** Exposure bias (EV) of the miner camera while the halo is on: negative darkens what auto exposure brightens back. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cheats|LightHalo")
	float LightHaloExposureBias = -1.f;
};
