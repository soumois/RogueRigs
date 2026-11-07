// Rogue Rigs - shared mouse hover effect of the screen buttons (ui-button-hover, task-485): the plate brightens and the button
// grows, eased in real time so it also plays on paused screens. Any widget opts a button in with one call; no Blueprint graph per button.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Ticker.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UObject/Object.h"
#include "RRButtonHover.generated.h"

class UButton;
class UMaterialInstanceDynamic;
class UMaterialInterface;
class UUserWidget;
class UWidget;

/**
 * Values of the button hover (ui-button-hover Look), overridable without code in Config/DefaultGame.ini,
 * section [/Script/RogueRigs.RRButtonHoverSettings]. Plain config object, same choice as URRIconSettings.
 */
UCLASS(Config = Game, DefaultConfig)
class ROGUERIGS_API URRButtonHoverSettings : public UObject
{
	GENERATED_BODY()

public:
	URRButtonHoverSettings();

	/** Render scale of a hovered button (1.05). Render transform only: the layout never moves. */
	UPROPERTY(Config)
	float HoverScale;

	/**
	 * Plate brightness of a hovered button, 1.25 = about +25 %, same hue: a factor on the displayed (sRGB) colour of textured plates;
	 * on untextured grey plates with white labels it multiplies the linear tint (smaller step, keeps the label readable).
	 */
	UPROPERTY(Config)
	float HoverBrightness;

	/** Ease time in and out, real seconds (0.1). */
	UPROPERTY(Config)
	float EaseSeconds;

	/** UI material that draws a plate texture (parameter PlateTexture) times the vertex colour times Brightness (linear). */
	UPROPERTY(Config)
	TSoftObjectPtr<UMaterialInterface> PlateMaterial;
};

/** Hover state of one button: its plate material (null for text buttons) and the eased hover amount 0..1. */
USTRUCT()
struct FRRButtonHoverEntry
{
	GENERATED_BODY()

	UPROPERTY()
	TWeakObjectPtr<UButton> Button;

	/** Dynamic plate material set on the Normal and Hovered brushes; null when the button only grows (main menu text buttons). */
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> PlateMaterial;

	/** Widget that grows: the button itself, or a wrapper when the button's own scale is animated by its widget (shop Buy juice). */
	UPROPERTY()
	TWeakObjectPtr<UWidget> ScaleWidget;

	/** Render scale the grown widget had when attached (the hover multiplies it). */
	FVector2D RestScale = FVector2D(1.0, 1.0);

	/** Untextured plate (grey rounded box): the brightening eases the Normal brush tint from RestTint instead of a material. */
	bool bTintPlate = false;

	/** Normal brush tint at attach time (untextured plates). */
	FLinearColor RestTint = FLinearColor::White;

	/** Eased hover amount: 0 at rest, 1 fully hovered. */
	float Amount = 0.0f;
};

/**
 * Drives the hover effect of every attached button. Polls each button every frame from the core ticker (real time, runs while
 * the game is paused): a button is hovered when the mouse is over it (Slate IsHovered) and it and its parents are enabled, so a
 * disabled button never reacts and keyboard or gamepad focus alone shows nothing. The plate brightens through a dynamic material on
 * the Normal brush (the Hovered brush becomes the same brush, so the Slate hover swap is invisible); the Pressed and Disabled
 * brushes are untouched, so the pressed look is unchanged.
 */
UCLASS()
class ROGUERIGS_API URRButtonHoverSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/**
	 * Gives Button the hover effect. bBrightenPlate: replace its Normal plate brush by the brightening material (plate buttons);
	 * false = growth only (text buttons). ScaleWidget: the widget that grows (null = the button); use a wrapper that holds only the
	 * button when the widget animates the button's own render scale. Safe to call twice on one button and with null.
	 */
	static void Attach(UButton* Button, bool bBrightenPlate = true, UWidget* ScaleWidget = nullptr);

	/** Attach for each named button of Owner's widget tree (a missing name is skipped with a log line). */
	static void AttachNamed(UUserWidget* Owner, const TArray<FName>& ButtonNames, bool bBrightenPlate = true);

	/** Eased hover amount of Button (0 at rest, 1 hovered), or -1 when it is not attached. For tests and probes. */
	float GetHoverAmount(const UButton* Button) const;

	/** Logs every attached button: owner widget, name, hovered, enabled, eased amount (rr.Debug.ButtonHover). */
	void LogEntries() const;

private:
	void AttachButton(UButton* Button, bool bBrightenPlate, UWidget* ScaleWidget);
	bool Tick(float DeltaTime);
	static bool IsButtonHoverable(const UButton* Button);
	void Apply(FRRButtonHoverEntry& Entry) const;

	UPROPERTY()
	TArray<FRRButtonHoverEntry> Entries;

	/** Loaded plate material (from the settings). */
	UPROPERTY()
	TObjectPtr<UMaterialInterface> PlateMaterialBase;

	FTSTicker::FDelegateHandle TickHandle;
};
