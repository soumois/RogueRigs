// Rogue Rigs - audio system (audio-direction, task-511): plays the sound of a named event from the event table of URRAudioSettings,
// through the Effects sound class, with the caps, pitch jitter, placement and pause rules of each row; button hover and click sounds.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Ticker.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "RRAudioSubsystem.generated.h"

class FRRAudioClickProcessor;
class SWidget;
class UAudioComponent;
class USoundBase;
class USoundClass;
struct FRRSoundEventRow;

/** Counters of one table row since start (debug and log checks). */
struct FRRSoundEventStats
{
	int32 Played = 0;
	int32 CapStops = 0;
	int32 PeakActive = 0;
};

/**
 * Game instance subsystem of the sound events.
 * - PlayEvent / PlayEventAt: finds the row of the event (dotted names fall back to their parent), plays its sound through the Effects
 *   class of URROptionsSettings, pitch jittered, placed (world rows) or centred, pause-proof (UI rows); a row capped at N stops its
 *   oldest play before the N+1th. A row with FadeOuter > 0 fades with the distance from the player pawn (silent from FadeOuter). No row or no sound: silent, no error.
 * - Buttons: a real-time ticker plays UIHover when the cursor enters an enabled Slate button of any screen (paused ones included), and
 *   a Slate input pre-processor plays UIClick on a left press on an enabled button. A button counts as disabled when it or a parent is.
 */
UCLASS()
class ROGUERIGS_API URRAudioSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/** The subsystem of WorldContextObject's game instance, or null. */
	static URRAudioSubsystem* Get(const UObject* WorldContextObject);

	/** Plays Event centred (or at nothing for a world row: centred too). Variant refines the event: Event.Variant row first. */
	static void PlayEvent(const UObject* WorldContextObject, FName Event, FName Variant = NAME_None);

	/** Plays Event at Location (used by world rows; other rows play centred). */
	static void PlayEventAt(const UObject* WorldContextObject, FName Event, const FVector& Location, FName Variant = NAME_None);

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/** Plays from this subsystem; Location null = centred. Returns the component played, or null (no row, no sound, no audio). */
	UAudioComponent* Play(const UObject* WorldContextObject, FName Event, const FVector* Location);

	/**
	 * Volume factor of Row at Distance uu from the player (task-513): 1 up to FadeInner, linear to 0 at FadeOuter, 0 beyond;
	 * 1 when the row has no fade (FadeOuter 0).
	 */
	static float GetDistanceFade(const FRRSoundEventRow& Row, float Distance);

	/** Logs, per row, sound, plays, cap stops, peak of simultaneous plays and plays active now. */
	void LogStats() const;

	/** Clears the counters of LogStats. */
	void ResetStats() { Stats.Reset(); }

	/** Plays of the row of Event active now (stopped and finished ones pruned). */
	int32 GetActiveCount(FName Event);

	/** Called by the input pre-processor on a left press at a screen position. */
	void HandleMousePress(const FVector2D& ScreenPosition);

private:
	bool TickHover(float DeltaTime);

	/** Enabled Slate button under ScreenPosition, or null; bOutDisabledButton tells a disabled button was found there instead. */
	TSharedPtr<SWidget> FindButtonAt(const FVector2D& ScreenPosition, bool& bOutDisabledButton) const;

	USoundBase* ResolveSound(const FRRSoundEventRow& Row);
	void PruneActive(TArray<TWeakObjectPtr<UAudioComponent>>& Active) const;

	/** Loaded sounds per row event name (null = row has no sound or failed to load, logged once). */
	UPROPERTY(Transient)
	TMap<FName, TObjectPtr<USoundBase>> LoadedSounds;

	UPROPERTY(Transient)
	TObjectPtr<USoundClass> EffectsClass;

	/** Active plays per row event name, oldest first. */
	TMap<FName, TArray<TWeakObjectPtr<UAudioComponent>>> ActivePlays;

	TMap<FName, FRRSoundEventStats> Stats;
	TSet<FName> ReportedSilent;

	TWeakPtr<SWidget> HoveredButton;
	TSharedPtr<FRRAudioClickProcessor> ClickProcessor;
	FTSTicker::FDelegateHandle TickerHandle;
};
