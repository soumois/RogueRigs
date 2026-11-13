// Rogue Rigs - sound event table (audio-direction, task-511): one row per sound event, read by URRAudioSubsystem.
// Adding or swapping a sound is one row in Config/DefaultGame.ini; a new event also needs its call site.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UObject/SoftObjectPtr.h"
#include "RRAudioSettings.generated.h"

class USoundBase;

/** One row of the event table: the sound an event plays and how it is mixed. */
USTRUCT()
struct FRRSoundEventRow
{
	GENERATED_BODY()

	/**
	 * Event name the code plays (Shot, Impact, Impact.Wall, EnemyDeath, PlayerHit, XPPickup, LevelUp, UIHover, UIClick). A dotted name refines an
	 * event: a call for Shot.Shotgun uses row Shot.Shotgun when it exists, else row Shot.
	 */
	UPROPERTY(Config)
	FName Event;

	/** Sound played (wave or cue). Empty: the event stays silent, without error. */
	UPROPERTY(Config)
	TSoftObjectPtr<USoundBase> Sound;

	/** Volume multiplier (1). */
	UPROPERTY(Config)
	float Volume = 1.f;

	/** Random pitch range, fraction: 0.05 = pitch drawn in [0.95, 1.05] on each play (0 = none). */
	UPROPERTY(Config)
	float PitchJitter = 0.f;

	/** At most this many plays of the row at once; a new play stops the oldest one (0 = no cap). */
	UPROPERTY(Config)
	int32 MaxConcurrent = 0;

	/** World sound: placed at the event location, panned by its place on screen (no engine attenuation). False: centred. */
	UPROPERTY(Config)
	bool bSpatial = false;

	/**
	 * Distance fade from the player (task-513), for a play with a location (PlayEventAt): Volume up to FadeInner uu (horizontal
	 * distance player to event at the moment of the play), then linear down to silence at FadeOuter uu; at or beyond FadeOuter the
	 * event does not play. FadeOuter 0 = no fade (default). Any row can use it.
	 */
	UPROPERTY(Config)
	float FadeInner = 0.f;

	/** Distance in uu at which the fade reaches silence; 0 = no distance fade. See FadeInner. */
	UPROPERTY(Config)
	float FadeOuter = 0.f;

	/** Interface sound: also plays while the game is paused. */
	UPROPERTY(Config)
	bool bUI = false;
};

/**
 * Data of the audio system, read with GetDefault<URRAudioSettings>(). C++ defaults, overridable without code in
 * Config/DefaultGame.ini, section [/Script/RogueRigs.RRAudioSettings] (plain config object, same choice as URROptionsSettings).
 * Every event plays through the Effects sound class of URROptionsSettings (child of Master), so the Effects and Master sliders set it.
 */
UCLASS(Config = Game, DefaultConfig)
class ROGUERIGS_API URRAudioSettings : public UObject
{
	GENERATED_BODY()

public:
	/** The event table. */
	UPROPERTY(Config)
	TArray<FRRSoundEventRow> Events;

	/** Row of Event, or of its parent name (Shot.Shotgun -> Shot); null when none. */
	const FRRSoundEventRow* FindRow(FName Event) const;
};
