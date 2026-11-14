// Rogue Rigs - shared entrance effect of the end screens (task-547, death-screen gdd Appearance, intermission gdd): backdrop with the
// level desaturated, title drop with a short shake, frame slide up, buttons last; skip on a click or key. Used by the run-lost
// screen (URRRunLostWidget) and the stats screen (URRIntermissionWidget); any other screen opts in with its part names.

#pragma once

#include "CoreMinimal.h"
#include "Components/SlateWrapperTypes.h"
#include "UObject/Object.h"
#include "RRScreenEntrance.generated.h"

class UButton;
class UCameraComponent;
class UUserWidget;
class UWidget;

/**
 * Timings and amounts of the entrance (death-screen gdd Appearance Q2), overridable without code in Config/DefaultGame.ini,
 * section [/Script/RogueRigs.RRScreenEntranceSettings]. Phases in real seconds from the show: backdrop [0, BackdropEnd],
 * title [BackdropEnd, TitleEnd], frame [TitleEnd, FrameEnd], buttons [FrameEnd, ButtonsEnd], end state at TotalSeconds.
 */
UCLASS(Config = Game, DefaultConfig)
class ROGUERIGS_API URRScreenEntranceSettings : public UObject
{
	GENERATED_BODY()

public:
	/** End of the backdrop fade 0 -> 1 and of the level desaturation (0.6 s). */
	UPROPERTY(Config)
	float BackdropEnd = 0.6f;

	/** End of the title fade and drop (0.85 s: 0.25 s after the backdrop). */
	UPROPERTY(Config)
	float TitleEnd = 0.85f;

	/** End of the frame fade and slide (1.15 s: 0.3 s). */
	UPROPERTY(Config)
	float FrameEnd = 1.15f;

	/** End of the buttons fade (1.35 s: 0.2 s). */
	UPROPERTY(Config)
	float ButtonsEnd = 1.35f;

	/** Time of the end state, when input reaches the buttons (1.4 s). */
	UPROPERTY(Config)
	float TotalSeconds = 1.4f;

	/** Title render scale at the start of its drop (1.4 -> 1). */
	UPROPERTY(Config)
	float TitleStartScale = 1.4f;

	/** Title shake: horizontal amplitude in slate units (8), angular speed in radians per second (90), faded to 0 by ShakeEnd (1.0 s). */
	UPROPERTY(Config)
	float ShakeAmplitude = 8.f;

	UPROPERTY(Config)
	float ShakeSpeed = 90.f;

	UPROPERTY(Config)
	float ShakeEnd = 1.f;

	/** Frame slide: starts this many slate units below its place (40). */
	UPROPERTY(Config)
	float FrameSlide = 40.f;

	/** Level desaturation reached with the backdrop, 0 none to 1 grey (1). */
	UPROPERTY(Config)
	float LevelDesaturation = 1.f;
};

/** Names of the widgets of a screen that the entrance drives (found by name in the widget tree, variables or not). */
USTRUCT(BlueprintType)
struct FRRScreenEntranceParts
{
	GENERATED_BODY()

	/** Fades 0 -> 1 with the level desaturation. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Entrance")
	FName Backdrop = TEXT("Backdrop");

	/** Drops in: scale TitleStartScale -> 1 and the shake (also listed in TitleGroup for its fade). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Entrance")
	FName Title = TEXT("Title");

	/** Fade in with the title: title, subtitle, their side bars and lines. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Entrance")
	TArray<FName> TitleGroup;

	/** Fade in and slide up: the frame and everything drawn on it. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Entrance")
	TArray<FName> FrameGroup;

	/** Fade in last; not clickable before the end state. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Entrance")
	TArray<FName> Buttons;

	/** Invisible full-screen button that turns a click into a skip; created on the root canvas when the screen has none. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Entrance")
	FName SkipButton = TEXT("SkipButton");
};

DECLARE_DELEGATE(FRRScreenEntranceFinished);

/**
 * One entrance run on one screen widget. The owner widget creates it, calls Start after filling its texts, forwards NativeTick
 * (real-time delta, so it plays on paused screens) and Restore at NativeDestruct. A key or a mouse button of the owning player
 * (WasInputKeyJustPressed AnyKey) or a click on the skip button jumps to the end state; OnFinished fires once at the end state
 * (played or skipped). The buttons are hit-test invisible until then and covered by the skip button, so a skip never presses one.
 */
UCLASS()
class ROGUERIGS_API URRScreenEntrance : public UObject
{
	GENERATED_BODY()

public:
	/** Finds the parts of Screen by name and keeps their rest transform and opacity. Call once, after the widget tree exists. */
	void Setup(UUserWidget* Screen, const FRRScreenEntranceParts& Parts);

	/** Starts at T = 0: parts hidden, skip button shown, controller full tick while paused (the camera applies the desaturation). */
	void Start();

	/** Advances by DeltaSeconds (real time); ends at TotalSeconds or on a key of the owning player. */
	void Tick(float DeltaSeconds);

	/** Jumps to the end state now. False when not playing. */
	bool Skip();

	/** Puts the level saturation and the controller pause tick back as they were before Start. */
	void Restore();

	/** True between Start and the end state. */
	bool IsPlaying() const { return bPlaying; }

	/** Applies the state at T seconds (clamped to the end state). */
	void Apply(float T);

	/** Fired once when the end state is reached. */
	FRRScreenEntranceFinished OnFinished;

	/**
	 * Level behind the screen, 0 full colour to 1 grey: colour saturation of the owning player's view target camera post process,
	 * scaled from its value before the first change. False without a camera.
	 */
	bool SetLevelDesaturation(float Amount);

private:
	/** Skip button click. */
	UFUNCTION()
	void HandleSkipClicked();

	void Finish();
	UButton* MakeSkipButton();

	struct FPart
	{
		TWeakObjectPtr<UWidget> Widget;
		FVector2D RestTranslation = FVector2D::ZeroVector;
		FVector2D RestScale = FVector2D(1.0, 1.0);
		float RestOpacity = 1.f;
	};
	static FPart MakePart(UWidget* Widget);
	static void SetOpacity(const FPart& Part, float Alpha);

	TWeakObjectPtr<UUserWidget> Owner;
	FPart BackdropPart;
	FPart TitlePart;
	TArray<FPart> TitleParts;
	TArray<FPart> FrameParts;
	TArray<FPart> ButtonParts;
	TArray<ESlateVisibility> ButtonVisibility;

	UPROPERTY(Transient)
	TObjectPtr<UButton> SkipButton;

	float Elapsed = 0.f;
	bool bPlaying = false;

	/** Frames left before the controller pause tick is restored (lets the camera apply the last desaturation). */
	int32 RestoreTickFrames = -1;
	bool bSavedFullTickWhenPaused = false;
	bool bFullTickSaved = false;

	TWeakObjectPtr<UCameraComponent> SavedCamera;
	FVector4 SavedSaturation = FVector4(1.f, 1.f, 1.f, 1.f);
	bool bSavedOverride = false;
	bool bSaturationSaved = false;
};
