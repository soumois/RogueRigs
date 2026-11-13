// Rogue Rigs - music per screen (task-519): main menu, loading and in game tracks of URRAudioSettings::Music, looped, through the
// Music sound class, changed with a fade out of the old track and a fade in of the new one.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Ticker.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "RRMusicSubsystem.generated.h"

class UAudioComponent;
class USoundBase;
class USoundClass;
class USoundWave;

/**
 * Game instance subsystem of the music. A core ticker (runs while paused) picks the wanted track every frame:
 * - Loading while URRRunTransitionSubsystem::IsLevelLoading (New Run, death Retry, Continue with the loading screen), on both sides
 *   of the map travel; none during the short screen fade out of a Continue whose next level is already prepared (no loading screen).
 * - MainMenu in a world of ARRMainMenuGameMode.
 * - InGame in any other game world. It restarts from its start at each new level (OnLevelReady) or new world (Q13).
 * A change fades the old track out over MusicFadeOut and starts the new one from its start, faded in over MusicFadeIn. Pause,
 * level-up, shop, level end and death screens keep the in-game track as it is (Q12): the plays are UI sounds (never paused) and
 * persist across map travels. Each track loops (the wave is imported looping). No row or no sound: that screen is silent.
 */
UCLASS()
class ROGUERIGS_API URRMusicSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static const FName MainMenuTrack;
	static const FName LoadingTrack;
	static const FName InGameTrack;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/** The subsystem of WorldContextObject's game instance, or null. */
	static URRMusicSubsystem* Get(const UObject* WorldContextObject);

	/** Track playing (or fading in) now; None when silent. */
	FName GetCurrentTrack() const { return CurrentTrack; }

	/** Logs the current track, its sound, level, position, loops and the plays still fading out. */
	void LogState() const;

	/** Debug: restarts the current track at StartSeconds, without fade (loop checks near the end of a track). */
	void SeekCurrent(float StartSeconds);

private:
	bool Tick(float DeltaTime);

	/** Wanted track now; false when it cannot be told (no world, world tearing down, game mode not spawned yet). */
	bool GetWantedTrack(FName& OutTrack, UWorld*& OutWorld) const;

	/** Fades the current play out and starts Track (None = silence) in World, from StartSeconds. */
	void SwitchTo(FName Track, UWorld* World, float StartSeconds = 0.f, bool bFade = true);

	/** Playback percent of the current play: a drop (or a new whole pass) is the track restarting from its start. */
	void HandlePlaybackPercent(const UAudioComponent* Component, const USoundWave* Wave, const float Percent);

	UFUNCTION()
	void HandleLevelReady(int32 LevelIndex, const FString& LayoutName);

	UPROPERTY(Transient)
	TObjectPtr<USoundClass> MusicClass;

	/** Play of the current track (persists across map travels). */
	UPROPERTY(Transient)
	TObjectPtr<UAudioComponent> CurrentComponent;

	/** Old plays fading out, until they stop. */
	UPROPERTY(Transient)
	TArray<TObjectPtr<UAudioComponent>> FadingComponents;

	FName CurrentTrack;

	/** World and level serial the in-game track was started for (a new level or world restarts it). */
	TWeakObjectPtr<UWorld> StartedWorld;
	int32 StartedLevelSerial = 0;
	int32 LevelSerial = 0;

	/** Last playback percent and passes of the current play (loop log). */
	float LastPercent = 0.f;
	bool bPercentValid = false;
	int32 Passes = 0;
	double PlayStartTime = 0.0;
	float PlayStartOffset = 0.f;

	/** Track sounds loaded at start (null = no row sound or not found). */
	UPROPERTY(Transient)
	TMap<FName, TObjectPtr<USoundBase>> LoadedSounds;

	/** Tracks already reported without a sound. */
	TSet<FName> ReportedSilent;

	FTSTicker::FDelegateHandle TickerHandle;
};
