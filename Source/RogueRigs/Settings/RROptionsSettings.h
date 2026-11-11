// Rogue Rigs - project data of the options screen (options-menu gdd): volume group assets, step sizes, display confirmation.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UObject/SoftObjectPath.h"
#include "RROptionsSettings.generated.h"

/**
 * Data of the Sound and Graphics options, read with GetDefault<URROptionsSettings>() by URRGameUserSettings. Not a
 * UDeveloperSettings (the module does not link DeveloperSettings, same choice as URRRunSettings). Override without code in
 * Config/DefaultGame.ini, section [/Script/RogueRigs.RROptionsSettings]. Values provisional (owner 2026-11-11, options-menu).
 */
UCLASS(Config = Game, DefaultConfig)
class ROGUERIGS_API URROptionsSettings : public UObject
{
	GENERATED_BODY()

public:
	URROptionsSettings();

	/** Sound class of the Master slider: parent of the Music and Effects classes, its volume scales both. */
	UPROPERTY(Config, EditAnywhere, Category = "Sound", meta = (AllowedClasses = "/Script/Engine.SoundClass"))
	FSoftObjectPath MasterSoundClass;

	/** Sound class of the Music slider; future music sounds use it. */
	UPROPERTY(Config, EditAnywhere, Category = "Sound", meta = (AllowedClasses = "/Script/Engine.SoundClass"))
	FSoftObjectPath MusicSoundClass;

	/** Sound class of the Effects slider; future effect sounds use it. */
	UPROPERTY(Config, EditAnywhere, Category = "Sound", meta = (AllowedClasses = "/Script/Engine.SoundClass"))
	FSoftObjectPath EffectsSoundClass;

	/** Sound mix pushed once per map that carries the three slider volumes as class overrides. */
	UPROPERTY(Config, EditAnywhere, Category = "Sound", meta = (AllowedClasses = "/Script/Engine.SoundMix"))
	FSoftObjectPath VolumeSoundMix;

	/** Slider step of the volumes (fraction, 0.05 = 5 %). */
	UPROPERTY(Config, EditAnywhere, Category = "Sound")
	float VolumeStep = 0.05f;

	/** Frame rate limit choices in selector order; 0 = Unlimited. */
	UPROPERTY(Config, EditAnywhere, Category = "Graphics")
	TArray<int32> FrameRateLimitOptions;

	/** Lowest resolution scale offered (percent). */
	UPROPERTY(Config, EditAnywhere, Category = "Graphics")
	float MinResolutionScalePercent = 50.f;

	/** Highest resolution scale offered (percent). */
	UPROPERTY(Config, EditAnywhere, Category = "Graphics")
	float MaxResolutionScalePercent = 100.f;

	/** Slider step of the resolution scale (percent). */
	UPROPERTY(Config, EditAnywhere, Category = "Graphics")
	float ResolutionScaleStepPercent = 5.f;

	/** Seconds the "Keep these display settings?" countdown lasts before it reverts (provisional, Planner 2026-11-11). */
	UPROPERTY(Config, EditAnywhere, Category = "Graphics")
	float DisplayConfirmSeconds = 15.f;
};
