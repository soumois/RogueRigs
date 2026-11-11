// Rogue Rigs - C++ base of the options screen (WBP_Options, task-505-02): Sound and Graphics tabs, one row per option of
// options-menu built here, Reset to defaults (current tab), Back, and the keep-or-revert panel of a display change.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Fonts/SlateFontInfo.h"
#include "RROptionsWidget.generated.h"

class UButton;
class UCanvasPanel;
class UImage;
class UOverlay;
class USizeBox;
class USlider;
class UTextBlock;
class UTexture2D;
class UVerticalBox;
class URROptionsWidget;

/** Tab of the options screen (Gameplay comes with task-506). */
UENUM(BlueprintType)
enum class ERROptionsTab : uint8
{
	Sound,
	Graphics
};

/** One option row of the screen, in display order. */
UENUM(BlueprintType)
enum class ERROptionRow : uint8
{
	MasterVolume,
	MusicVolume,
	EffectsVolume,
	WindowMode,
	Resolution,
	VSync,
	FrameRateLimit,
	ResolutionScale,
	QualityPreset,
	Shadows,
	Effects,
	AntiAliasing,
	PostProcessing,
	Textures
};

/** Fired when the screen closes (Back or Escape); the screen that opened it puts its focus back. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRROptionsClosedEvent);

/** Click and value relay of one row control (a dynamic delegate carries no payload). */
UCLASS(Transient)
class ROGUERIGS_API URROptionsRowHandler : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TObjectPtr<URROptionsWidget> Owner;

	ERROptionRow Row = ERROptionRow::MasterVolume;
	int32 Direction = 1;

	UFUNCTION()
	void HandleClicked();

	UFUNCTION()
	void HandleValueChanged(float Value);
};

/** Widgets of one built row. */
USTRUCT()
struct FRROptionsRowWidgets
{
	GENERATED_BODY()

	ERROptionRow Row = ERROptionRow::MasterVolume;

	UPROPERTY()
	TObjectPtr<UCanvasPanel> Canvas;

	UPROPERTY()
	TObjectPtr<UTextBlock> Value;

	UPROPERTY()
	TObjectPtr<UButton> LeftButton;

	UPROPERTY()
	TObjectPtr<UButton> RightButton;

	UPROPERTY()
	TObjectPtr<USizeBox> LeftBox;

	UPROPERTY()
	TObjectPtr<USizeBox> RightBox;

	UPROPERTY()
	TObjectPtr<USlider> Slider;

	UPROPERTY()
	TObjectPtr<UButton> Toggle;

	UPROPERTY()
	TObjectPtr<UTextBlock> ToggleText;

	UPROPERTY()
	TObjectPtr<USizeBox> ThumbBox;
};

/**
 * Options screen (options-menu gdd). The Blueprint subclass holds the static layout (backdrop, title, tabs, frame, the two
 * row lists, Reset to defaults, Back, the confirmation panel); this base builds the option rows into SoundList and
 * GraphicsList (label, selector arrows and value, slider, or toggle), keeps them in sync with URRGameUserSettings and sizes
 * the arrows, knob and thumb from the row height, so the rows follow the frame anchors at every resolution. Every change
 * goes through URRGameUserSettings and applies at once; a window mode or resolution change shows ConfirmPanel until Keep,
 * Revert, Escape or the end of the countdown. Escape: Revert while the panel shows, else Back. Debug: rr.Options.
 */
UCLASS(Abstract)
class ROGUERIGS_API URROptionsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Shows a tab's rows and marks its tab selected. */
	UFUNCTION(BlueprintCallable, Category = "Options")
	void ShowTab(ERROptionsTab Tab);

	UFUNCTION(BlueprintPure, Category = "Options")
	ERROptionsTab GetCurrentTab() const { return CurrentTab; }

	/** Reset to defaults of the current tab only (Sound: volumes 100 %; Graphics: first-launch values). */
	UFUNCTION(BlueprintCallable, Category = "Options")
	void ResetCurrentTab();

	/** Back: reverts a pending display change, removes the screen and fires OnOptionsClosed. */
	UFUNCTION(BlueprintCallable, Category = "Options")
	void Back();

	/** Reads every value from URRGameUserSettings into the rows. */
	UFUNCTION(BlueprintCallable, Category = "Options")
	void RefreshValues();

	/** Keyboard focus on the first tab (options-menu: focus starts there). */
	UFUNCTION(BlueprintCallable, Category = "Options")
	void FocusFirstTab();

	/** Selector arrow or toggle of a row: Direction > 0 next, < 0 previous; applies through the settings and refreshes. */
	UFUNCTION(BlueprintCallable, Category = "Options")
	void StepRow(ERROptionRow Row, int32 Direction);

	/** Slider of a row moved (volume 0..1, resolution scale in percent). */
	UFUNCTION(BlueprintCallable, Category = "Options")
	void SetRowSliderValue(ERROptionRow Row, float Value);

	/** Text shown in a row's value (rr.Options dump). */
	FString GetRowValueString(ERROptionRow Row) const;

	/** Fired by Back after the screen is removed. */
	UPROPERTY(BlueprintAssignable, Category = "Options")
	FRROptionsClosedEvent OnOptionsClosed;

	/** Label of a row as drawn ("Window mode"). */
	static FText GetRowLabel(ERROptionRow Row);

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	// ---- Bound from the Blueprint (all optional) ---------------------------------------------------------------------

	UPROPERTY(BlueprintReadOnly, Category = "Options", meta = (BindWidgetOptional))
	TObjectPtr<UButton> SoundTabButton;

	UPROPERTY(BlueprintReadOnly, Category = "Options", meta = (BindWidgetOptional))
	TObjectPtr<UButton> GraphicsTabButton;

	/** Cream plate drawn over the tab button when the tab is selected. */
	UPROPERTY(BlueprintReadOnly, Category = "Options", meta = (BindWidgetOptional))
	TObjectPtr<UWidget> SoundTabSelected;

	UPROPERTY(BlueprintReadOnly, Category = "Options", meta = (BindWidgetOptional))
	TObjectPtr<UWidget> GraphicsTabSelected;

	UPROPERTY(BlueprintReadOnly, Category = "Options", meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> SoundTabText;

	UPROPERTY(BlueprintReadOnly, Category = "Options", meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> GraphicsTabText;

	/** Container of a tab (button, selected plate, label): the widget that grows on hover. */
	UPROPERTY(BlueprintReadOnly, Category = "Options", meta = (BindWidgetOptional))
	TObjectPtr<UWidget> SoundTab;

	UPROPERTY(BlueprintReadOnly, Category = "Options", meta = (BindWidgetOptional))
	TObjectPtr<UWidget> GraphicsTab;

	/** Row lists filled by this class (keep them empty in the designer). */
	UPROPERTY(BlueprintReadOnly, Category = "Options", meta = (BindWidgetOptional))
	TObjectPtr<UVerticalBox> SoundList;

	UPROPERTY(BlueprintReadOnly, Category = "Options", meta = (BindWidgetOptional))
	TObjectPtr<UVerticalBox> GraphicsList;

	UPROPERTY(BlueprintReadOnly, Category = "Options", meta = (BindWidgetOptional))
	TObjectPtr<UButton> ResetButton;

	UPROPERTY(BlueprintReadOnly, Category = "Options", meta = (BindWidgetOptional))
	TObjectPtr<UButton> BackButton;

	/** Keep-or-revert panel, collapsed until a display change waits for Keep. */
	UPROPERTY(BlueprintReadOnly, Category = "Options", meta = (BindWidgetOptional))
	TObjectPtr<UWidget> ConfirmPanel;

	/** "Reverting in 15 s", updated each frame while the panel shows. */
	UPROPERTY(BlueprintReadOnly, Category = "Options", meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ConfirmCountdownText;

	UPROPERTY(BlueprintReadOnly, Category = "Options", meta = (BindWidgetOptional))
	TObjectPtr<UButton> KeepButton;

	UPROPERTY(BlueprintReadOnly, Category = "Options", meta = (BindWidgetOptional))
	TObjectPtr<UButton> RevertButton;

	// ---- Row look (class defaults) -----------------------------------------------------------------------------------

	UPROPERTY(EditDefaultsOnly, Category = "Options|Rows")
	TSoftObjectPtr<UTexture2D> ArrowTexture;

	UPROPERTY(EditDefaultsOnly, Category = "Options|Rows")
	TSoftObjectPtr<UTexture2D> SliderTrackTexture;

	UPROPERTY(EditDefaultsOnly, Category = "Options|Rows")
	TSoftObjectPtr<UTexture2D> SliderKnobTexture;

	UPROPERTY(EditDefaultsOnly, Category = "Options|Rows")
	TSoftObjectPtr<UTexture2D> ToggleTrackTexture;

	UPROPERTY(EditDefaultsOnly, Category = "Options|Rows")
	TSoftObjectPtr<UTexture2D> ToggleThumbTexture;

	/** Box margins (UMG fractions) of the 9-slice row sprites (art-prompts-options-screen-mockup). */
	UPROPERTY(EditDefaultsOnly, Category = "Options|Rows")
	FMargin SliderTrackMargin = FMargin(0.09f, 0.f, 0.09f, 0.f);

	UPROPERTY(EditDefaultsOnly, Category = "Options|Rows")
	FMargin ToggleTrackMargin = FMargin(0.10f, 0.23f, 0.10f, 0.23f);

	/** Font of labels and values (Roboto Bold). */
	UPROPERTY(EditDefaultsOnly, Category = "Options|Rows")
	FSlateFontInfo RowFont;

	UPROPERTY(EditDefaultsOnly, Category = "Options|Rows")
	FLinearColor LabelColour = FLinearColor(0.85f, 0.82f, 0.76f);

	UPROPERTY(EditDefaultsOnly, Category = "Options|Rows")
	FLinearColor ValueColour = FLinearColor::White;

	UPROPERTY(EditDefaultsOnly, Category = "Options|Rows")
	FLinearColor DividerColour = FLinearColor(0.85f, 0.82f, 0.76f, 0.8f);

	/** Tab label colour on the cream selected plate and on the dark plate. */
	UPROPERTY(EditDefaultsOnly, Category = "Options|Tabs")
	FLinearColor SelectedTabTextColour = FLinearColor(0.05f, 0.04f, 0.03f);

	UPROPERTY(EditDefaultsOnly, Category = "Options|Tabs")
	FLinearColor TabTextColour = FLinearColor(0.92f, 0.90f, 0.85f);

	/** Horizontal anchors of the row columns, fractions of the list width (from the validated mockup). */
	UPROPERTY(EditDefaultsOnly, Category = "Options|Rows")
	float LeftArrowX = 0.486f;

	UPROPERTY(EditDefaultsOnly, Category = "Options|Rows")
	float ValueX = 0.724f;

	UPROPERTY(EditDefaultsOnly, Category = "Options|Rows")
	float RightArrowX = 0.961f;

	UPROPERTY(EditDefaultsOnly, Category = "Options|Rows")
	float SliderMinX = 0.455f;

	UPROPERTY(EditDefaultsOnly, Category = "Options|Rows")
	float SliderMaxX = 0.895f;

	UPROPERTY(EditDefaultsOnly, Category = "Options|Rows")
	float ToggleMinX = 0.829f;

	/** Control heights as fractions of the row height (rows share the list height). */
	UPROPERTY(EditDefaultsOnly, Category = "Options|Rows")
	float ArrowHeightFraction = 0.78f;

	UPROPERTY(EditDefaultsOnly, Category = "Options|Rows")
	float KnobHeightFraction = 0.8f;

	UPROPERTY(EditDefaultsOnly, Category = "Options|Rows")
	float SliderBarFraction = 0.32f;

	UPROPERTY(EditDefaultsOnly, Category = "Options|Rows")
	float ThumbHeightFraction = 0.62f;

	/** Height of the divider after Resolution scale, in rows. */
	UPROPERTY(EditDefaultsOnly, Category = "Options|Rows")
	float DividerRowWeight = 0.5f;

public:
	URROptionsWidget(const FObjectInitializer& ObjectInitializer);

private:
	void BuildRows();
	void AddRow(UVerticalBox* List, ERROptionRow Row);
	void AddDivider(UVerticalBox* List);
	UButton* MakeArrow(UCanvasPanel* Canvas, ERROptionRow Row, int32 Direction, float AnchorX, USizeBox*& OutBox);
	URROptionsRowHandler* MakeHandler(ERROptionRow Row, int32 Direction);
	void ApplyRowHeight(float RowHeight);
	void RefreshRow(FRROptionsRowWidgets& Widgets);

	UFUNCTION()
	void HandleSoundTabClicked();

	UFUNCTION()
	void HandleGraphicsTabClicked();

	UFUNCTION()
	void HandleResetClicked();

	UFUNCTION()
	void HandleBackClicked();

	UFUNCTION()
	void HandleKeepClicked();

	UFUNCTION()
	void HandleRevertClicked();

	UFUNCTION()
	void HandleDisplayConfirmChanged(bool bPending);

	UPROPERTY(Transient)
	TArray<FRROptionsRowWidgets> Rows;

	UPROPERTY(Transient)
	TArray<TObjectPtr<URROptionsRowHandler>> Handlers;

	UPROPERTY(Transient)
	TObjectPtr<UTexture2D> LoadedArrow;

	UPROPERTY(Transient)
	TObjectPtr<UTexture2D> LoadedTrack;

	UPROPERTY(Transient)
	TObjectPtr<UTexture2D> LoadedKnob;

	UPROPERTY(Transient)
	TObjectPtr<UTexture2D> LoadedToggleTrack;

	UPROPERTY(Transient)
	TObjectPtr<UTexture2D> LoadedThumb;

	ERROptionsTab CurrentTab = ERROptionsTab::Graphics;
	float AppliedRowHeight = 0.f;
	bool bClosing = false;
};
