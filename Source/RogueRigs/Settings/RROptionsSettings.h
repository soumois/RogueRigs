// Rogue Rigs - tunables of the options screen (options-screen gdd, task-500-03): sound classes, steps, lists, countdown.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UObject/SoftObjectPtr.h"
#include "RROptionsSettings.generated.h"

class USoundClass;
class UTexture2D;
class URROptionsWidget;

/**
 * Data of the options screen, read with GetDefault<URROptionsSettings>(). C++ defaults, overridable without code in
 * Config/DefaultGame.ini, section [/Script/RogueRigs.RROptionsSettings] (not a UDeveloperSettings, same choice as URRRunSettings).
 */
UCLASS(Config = Game, DefaultConfig)
class ROGUERIGS_API URROptionsSettings : public UObject
{
	GENERATED_BODY()

public:
	URROptionsSettings();

	/** Sound class of the Master slider (parent of Music and Effects). */
	UPROPERTY(Config, EditAnywhere, Category = "Sound")
	TSoftObjectPtr<USoundClass> MasterSoundClass;

	/** Sound class of the Music slider. */
	UPROPERTY(Config, EditAnywhere, Category = "Sound")
	TSoftObjectPtr<USoundClass> MusicSoundClass;

	/** Sound class of the Effects slider. */
	UPROPERTY(Config, EditAnywhere, Category = "Sound")
	TSoftObjectPtr<USoundClass> EffectsSoundClass;

	/** Volume slider step (0.05 = 5 %). */
	UPROPERTY(Config, EditAnywhere, Category = "Sound")
	float VolumeStep = 0.05f;

	/**
	 * Sound event (URRAudioSettings table) played when the Effects slider reaches a new VolumeStep step, so the player hears the
	 * level it sets (task-520, Q1: the player shot, centred, plays paused). None = no preview.
	 */
	UPROPERTY(Config, EditAnywhere, Category = "Sound")
	FName EffectsPreviewEvent = TEXT("EffectsPreview");

	/** Minimum real time between two Effects previews, seconds (0.15); the last step reached plays once the interval ends. */
	UPROPERTY(Config, EditAnywhere, Category = "Sound")
	float EffectsPreviewInterval = 0.15f;

	/** Frame rate limit choices in order, 0 = Unlimited (30, 60, 120, 144, Unlimited). */
	UPROPERTY(Config, EditAnywhere, Category = "Graphics")
	TArray<int32> FrameRateLimits;

	/** Resolution scale range and step, percent (50 to 100). */
	UPROPERTY(Config, EditAnywhere, Category = "Graphics")
	float MinResolutionScalePercent = 50.f;

	UPROPERTY(Config, EditAnywhere, Category = "Graphics")
	float MaxResolutionScalePercent = 100.f;

	UPROPERTY(Config, EditAnywhere, Category = "Graphics")
	float ResolutionScaleStep = 5.f;

	/** Seconds of the keep-or-revert dialog after a window mode or resolution change (15). */
	UPROPERTY(Config, EditAnywhere, Category = "Graphics")
	float DisplayConfirmSeconds = 15.f;

	/** Options screen class opened by the main and pause menus (WBP_Options). */
	UPROPERTY(Config, EditAnywhere, Category = "Screen")
	TSoftClassPtr<URROptionsWidget> OptionsWidgetClass;

	/** Z order of the options screen above the menu it opens from. */
	UPROPERTY(Config, EditAnywhere, Category = "Screen")
	int32 OptionsZOrderAbove = 5;

	/** Plate of the active tab (the cream button plate) and its horizontal 3-slice margin (fraction). */
	UPROPERTY(Config, EditAnywhere, Category = "Screen")
	TSoftObjectPtr<UTexture2D> ActiveTabTexture;

	UPROPERTY(Config, EditAnywhere, Category = "Screen")
	float ActiveTabMargin = 0.155f;

	/** Plate of the inactive tabs (dark plate) and its horizontal 3-slice margin (fraction). */
	UPROPERTY(Config, EditAnywhere, Category = "Screen")
	TSoftObjectPtr<UTexture2D> InactiveTabTexture;

	UPROPERTY(Config, EditAnywhere, Category = "Screen")
	float InactiveTabMargin = 0.10f;

	/** Tab label colours: dark on the cream active plate, light on the dark inactive plates. */
	UPROPERTY(Config, EditAnywhere, Category = "Screen")
	FLinearColor ActiveTabTextColor = FLinearColor(0.05f, 0.04f, 0.03f, 1.f);

	UPROPERTY(Config, EditAnywhere, Category = "Screen")
	FLinearColor InactiveTabTextColor = FLinearColor(0.85f, 0.85f, 0.82f, 1.f);
};
