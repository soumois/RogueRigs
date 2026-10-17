// Rogue Rigs - subtitle subsystem: one voice line at a time (speaker prefix, timed in C++), raised by the company alert events, shown by the HUD subtitle widget.

#pragma once

#include "CoreMinimal.h"
#include "Run/RRRunTransitionSubsystem.h"
#include "Subsystems/WorldSubsystem.h"
#include "RRSubtitleSubsystem.generated.h"

class ARRGameMode;
class URRRunFlowSubsystem;

/** Broadcast when a subtitle starts (also when it replaces the current one): speaker id, speaker display name, line, ready-to-show text ("Speaker: line"), seconds on screen. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FRROnSubtitleShownSignature, FName, SpeakerId, const FText&, Speaker, const FText&, Text, const FText&, Subtitle, float, Duration);

/** Broadcast when the current subtitle ends: its time ran out or Clear was called. Not broadcast when a new line replaces it. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRROnSubtitleClearedSignature);

/** One speaker of the subtitle table: id used by the callers and the bark data, English display name shown in front of the line. */
USTRUCT(BlueprintType)
struct FRRSubtitleSpeaker
{
	GENERATED_BODY()

	/** Speaker id (kessler_voss, rook, vesna). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Subtitles")
	FName Id;

	/** Name shown before the line ("Kessler-Voss"). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Subtitles")
	FText DisplayName;

	/** True: the line shows as "<DisplayName>: <line>"; false: the line alone (kessler_voss, owner decision task-215). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Subtitles")
	bool bShowPrefix = true;
};

/**
 * Subtitles of voice lines (sys-subtitles): one line at a time, a new line replaces the current one (queue of one). The line is
 * shown as "<Speaker>: <line>" for BaseSeconds + SecondsPerCharacter x line length (or an explicit duration); the remaining time
 * is ticked here (world time, frozen while the game is paused), so the widget never polls: it binds OnSubtitleShown and
 * OnSubtitleCleared. Callers: URRRunFlowSubsystem (console_dropped, bonus_chest_dropped, extraction_ready, shuttle_countdown once
 * per countdown start) and URRHUDWidget (horde_incoming) through ShowAlertLine, which picks the Kessler-Voss line with
 * URRTextContentSubsystem::GetAlertLine. Barks (rook, vesna) can use Show later; nothing shows them yet. Silent during a run
 * transition (task-207-01): the intermission or the loading clears the current line and Show refuses lines until OnLevelReady.
 * Silent after the run end (task-211-01): OnRunEnded (end screen) or OnGameOver (death) clears the line and silences until
 * the world ends (restart = new world, allowed again). Game and PIE worlds.
 */
UCLASS(Config = Game)
class ROGUERIGS_API URRSubtitleSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	/** Speaker id of the company voice (alert lines). */
	static const FName SpeakerKesslerVoss;
	/** Speaker id of Rook (barks). */
	static const FName SpeakerRook;
	/** Speaker id of Vesna (barks). */
	static const FName SpeakerVesna;

	/** Default speaker table and timing. */
	URRSubtitleSubsystem();

	/** Game and PIE worlds only. */
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

	/**
	 * Binds the run transition events (URRRunTransitionSubsystem, task-207-01): silent from OnIntermissionStarted and
	 * OnLoadingStarted to OnLevelReady. A world opened by a transition still loading starts silent.
	 */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/**
	 * Binds the run end (task-211-01): URRRunFlowSubsystem::OnRunEnded (end screen: run won at the last level, shuttle missed)
	 * and the auth ARRGameMode::OnGameOver (miner death) clear the current line and silence the subtitles for the rest of this
	 * world. OnLevelReady does not lift it: the restart opens a new world, whose subsystem starts allowed (logged here).
	 */
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	/** Unbinds the run transition, run end and game over events. */
	virtual void Deinitialize() override;

	/** The subsystem of WorldContext's world; null without a world or in an unsupported world type. */
	static URRSubtitleSubsystem* Get(const UObject* WorldContext);

	/** "Speaker: Text"; Text alone when Speaker is empty. */
	static FText ComposeSubtitle(const FText& Speaker, const FText& Text);

	/**
	 * Shows Text spoken by SpeakerId, replacing the current subtitle (no OnSubtitleCleared in between), then broadcasts
	 * OnSubtitleShown. DisplayName empty = the speaker table name (no prefix for an unknown speaker). DurationOverride <= 0 =
	 * GetDurationForText(Text). Returns false (nothing changes) when Text is empty or while silenced (IsSilenced: the line is
	 * dropped, not queued).
	 */
	UFUNCTION(BlueprintCallable, Category = "Subtitles")
	bool Show(FName SpeakerId, FText DisplayName, FText Text, float DurationOverride = -1.f);

	/**
	 * Picks a Kessler-Voss line of alert event EventId (URRTextContentSubsystem::GetAlertLine, which also keeps it as the
	 * event's current line) and shows it. False when the event has no line (missing file or id): nothing is shown.
	 */
	UFUNCTION(BlueprintCallable, Category = "Subtitles")
	bool ShowAlertLine(FName EventId);

	/** Ends the current subtitle now and broadcasts OnSubtitleCleared; nothing when none is shown. */
	UFUNCTION(BlueprintCallable, Category = "Subtitles")
	void Clear();

	/** True while a subtitle is shown. */
	UFUNCTION(BlueprintPure, Category = "Subtitles")
	bool HasSubtitle() const { return bActive; }

	/** True while a run transition (intermission or loading) or the run end (end screen, game over) silences the subtitles: Show refuses every line. */
	UFUNCTION(BlueprintPure, Category = "Subtitles")
	bool IsSilenced() const { return bSilenced; }

	/** Speaker id of the current subtitle; None when none. */
	UFUNCTION(BlueprintPure, Category = "Subtitles")
	FName GetCurrentSpeakerId() const { return bActive ? CurrentSpeakerId : NAME_None; }

	/** Speaker display name of the current subtitle; empty when none. */
	UFUNCTION(BlueprintPure, Category = "Subtitles")
	FText GetCurrentSpeaker() const { return bActive ? CurrentSpeaker : FText::GetEmpty(); }

	/** Line of the current subtitle (without the speaker); empty when none. */
	UFUNCTION(BlueprintPure, Category = "Subtitles")
	FText GetCurrentText() const { return bActive ? CurrentText : FText::GetEmpty(); }

	/** Current subtitle as shown ("Speaker: line"); empty when none. */
	UFUNCTION(BlueprintPure, Category = "Subtitles")
	FText GetCurrentSubtitle() const { return bActive ? CurrentSubtitle : FText::GetEmpty(); }

	/** Seconds the current subtitle stays on screen in total; 0 when none. */
	UFUNCTION(BlueprintPure, Category = "Subtitles")
	float GetCurrentDuration() const { return bActive ? CurrentDuration : 0.f; }

	/** Seconds left before the current subtitle clears; 0 when none. */
	UFUNCTION(BlueprintPure, Category = "Subtitles")
	float GetRemainingSeconds() const { return bActive ? RemainingSeconds : 0.f; }

	/** Display name of SpeakerId in the speaker table; empty when unknown. */
	UFUNCTION(BlueprintPure, Category = "Subtitles")
	FText GetSpeakerDisplayName(FName SpeakerId) const;

	/** True when SpeakerId lines show the "<Speaker>: " prefix (table bShowPrefix); true for a speaker missing from the table. */
	UFUNCTION(BlueprintPure, Category = "Subtitles")
	bool ShowsSpeakerPrefix(FName SpeakerId) const;

	/** On-screen seconds of a line: BaseSeconds + SecondsPerCharacter x its character count. */
	UFUNCTION(BlueprintPure, Category = "Subtitles")
	float GetDurationForText(const FText& Text) const;

	/** A subtitle started or replaced the current one. Bind in the subtitle widget. */
	UPROPERTY(BlueprintAssignable, Category = "Subtitles")
	FRROnSubtitleShownSignature OnSubtitleShown;

	/** The current subtitle ended (time out or Clear). Bind in the subtitle widget. */
	UPROPERTY(BlueprintAssignable, Category = "Subtitles")
	FRROnSubtitleClearedSignature OnSubtitleCleared;

	/** Counts the remaining time down and clears the subtitle when it reaches 0. */
	virtual void Tick(float DeltaTime) override;

	/** Stat id of the tick. */
	virtual TStatId GetStatId() const override;

protected:
	/** Transition OnIntermissionStarted callback: clears the current line and silences. */
	UFUNCTION()
	void HandleIntermissionStarted(const FRRIntermissionRecap& Recap);

	/** Transition OnLoadingStarted callback: clears the current line and silences. */
	UFUNCTION()
	void HandleLoadingStarted(int32 LevelIndex, const FString& LayoutName);

	/** Transition OnLevelReady callback: subtitles allowed again. */
	UFUNCTION()
	void HandleLevelReady(int32 LevelIndex, const FString& LayoutName);

	/** Run flow OnRunEnded callback (end screen shown): clears the current line and silences until the world ends. */
	UFUNCTION()
	void HandleRunEnded(bool bSuccess);

	/** Game mode OnGameOver callback (miner death): clears the current line and silences until the world ends. */
	UFUNCTION()
	void HandleGameOver();

	/** true: clears the current subtitle (OnSubtitleCleared) and refuses new lines; false: lines allowed again. Reason is logged. */
	void SetSilenced(bool bInSilenced, const TCHAR* Reason);

	/** Speakers and their English display names (kessler_voss, rook, vesna). */
	UPROPERTY(Config, EditAnywhere, Category = "Subtitles")
	TArray<FRRSubtitleSpeaker> Speakers;

	/** Fixed part of a subtitle duration, seconds (owner decision 2026-10-10: 4). */
	UPROPERTY(Config, EditAnywhere, Category = "Subtitles", meta = (ClampMin = "0"))
	float BaseSeconds = 4.f;

	/** Duration added per character of the line, seconds (owner decision 2026-10-10: 0.06). */
	UPROPERTY(Config, EditAnywhere, Category = "Subtitles", meta = (ClampMin = "0"))
	float SecondsPerCharacter = 0.06f;

	/** True while a subtitle is shown. */
	bool bActive = false;

	/** True between a transition's OnIntermissionStarted / OnLoadingStarted and its OnLevelReady: Show refuses every line. */
	bool bSilenced = false;

	/** Transition subsystem bound at Initialize (null in bare test worlds without a game instance). */
	TWeakObjectPtr<URRRunTransitionSubsystem> BoundTransition;

	/** True once the run ended in this world (end screen or game over): the silence is kept, OnLevelReady does not lift it. */
	bool bRunEndSilenced = false;

	/** Run flow whose OnRunEnded is bound (OnWorldBeginPlay). */
	TWeakObjectPtr<URRRunFlowSubsystem> BoundRunFlow;

	/** Game mode whose OnGameOver is bound (OnWorldBeginPlay; null in worlds without an ARRGameMode). */
	TWeakObjectPtr<ARRGameMode> BoundGameMode;

	/** Speaker id of the current subtitle. */
	FName CurrentSpeakerId;

	/** Speaker display name of the current subtitle. */
	FText CurrentSpeaker;

	/** Line of the current subtitle. */
	FText CurrentText;

	/** Composed current subtitle ("Speaker: line"). */
	FText CurrentSubtitle;

	/** Total on-screen seconds of the current subtitle. */
	float CurrentDuration = 0.f;

	/** Seconds left of the current subtitle. */
	float RemainingSeconds = 0.f;
};
