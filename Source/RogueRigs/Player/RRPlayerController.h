// Rogue Rigs - player controller: Enhanced Input setup, mouse cursor, routes input to the miner.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Components/SlateWrapperTypes.h"
#include "InputCoreTypes.h"
#include "Run/RRRunTransitionSubsystem.h"
#include "RRPlayerController.generated.h"

class ARRMiner;
class UEnhancedInputLocalPlayerSubsystem;
class UInputAction;
class UInputMappingContext;
class URRHUDWidget;
class URRIntermissionWidget;
class URRLoadingScreenWidget;
class URRShopScreenWidget;
class URRScreenFadeWidget;
struct FInputActionValue;

/**
 * Player controller of a run. Adds DefaultMappingContext to the local player's Enhanced Input subsystem,
 * binds the move, aim and place-turret actions and forwards them to the possessed ARRMiner. The place-turret action works
 * without assets: when PlaceTurretAction is unset, a transient action and mapping context are built from PlaceTurretKeys. Shows the mouse cursor;
 * hides it while a stick aims, shows it again on mouse movement. Assets are assigned in the Blueprint
 * subclass. Creates the HUD widget (HUDWidgetClass) for a local player. No gameplay logic here.
 */
UCLASS(Abstract)
class ROGUERIGS_API ARRPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	/** Shows the mouse cursor by default. */
	ARRPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** The HUD widget created at BeginPlay; null without a local player or without HUDWidgetClass. */
	URRHUDWidget* GetHUDWidget() const { return HUDWidget; }

	/** The intermission widget on screen; null outside the intermission or without IntermissionWidgetClass. */
	URRIntermissionWidget* GetIntermissionWidget() const { return IntermissionWidget; }

	/** The cards shop screen widget on screen; null outside the shop phase or without ShopScreenWidgetClass. */
	URRShopScreenWidget* GetShopScreenWidget() const { return ShopScreenWidget; }

	/** The loading screen widget on screen; null outside the loading or without LoadingScreenWidgetClass. */
	URRLoadingScreenWidget* GetLoadingScreenWidget() const { return LoadingScreenWidget; }

	/** True while a level transition is loading (URRRunTransitionSubsystem::IsLevelLoading): move, aim and turret input ignored. */
	bool IsGameplayInputHeld() const;

	/** Game-only input mode: the main menu leaves the viewport in UI-only mode, which would swallow the move keys in a packaged build. */
	void RestoreGameInputMode();

	/** True while the HUD widget is hidden by a run transition (intermission or loading, task-207-01). */
	bool IsHUDHiddenForTransition() const { return bHUDHiddenForTransition; }

protected:
	/**
	 * Local player only: creates HUDWidgetClass, adds it to the viewport and binds it to the game mode, the possessed miner and
	 * the run flow. Binds the run transition events (intermission and loading widgets) and shows the loading screen at once
	 * when this world was opened by a transition still loading, with the HUD hidden until OnLevelReady.
	 */
	virtual void BeginPlay() override;

	/** Unbinds the run transition events. */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Transition OnIntermissionStarted callback: hides the HUD, creates IntermissionWidgetClass and shows the recap (warning when unset: rr.Run.Continue). */
	UFUNCTION()
	void HandleIntermissionStarted(const FRRIntermissionRecap& Recap);

	/** Transition OnShopScreenStarted callback: removes the stats widget, creates ShopScreenWidgetClass (warning when unset: rr.Run.Continue). */
	UFUNCTION()
	void HandleShopScreenStarted();

	/** Transition OnRunWonScreenStarted callback: removes the stats widget and restores the HUD (its run won end screen follows). */
	UFUNCTION()
	void HandleRunWonScreenStarted();

	/** Removes the cards shop screen widget from the viewport. */
	void RemoveShopScreenWidget();

	/** Transition OnLoadingStarted callback: keeps the HUD hidden, removes the stats and shop widgets, shows the loading screen. */
	UFUNCTION()
	void HandleLoadingStarted(int32 LevelIndex, const FString& LayoutName);

	/** Transition OnLevelReady callback: removes the loading screen (and the stats or cards screen left by the fade path) and shows the HUD again. */
	UFUNCTION()
	void HandleLevelReady(int32 LevelIndex, const FString& LayoutName);

	/**
	 * Transition OnNextLevelReady callback (task-318-01: next level prepared in this world): replaces the HUD widget by a fresh one,
	 * as a new world would create it (minimap of the rebuilt grid, objective, shuttle and alert state of a level start); it stays
	 * collapsed until OnLevelReady.
	 */
	UFUNCTION()
	void HandleNextLevelReady(int32 LevelIndex, const FString& LayoutName);

	/** Transition OnScreenFadeStarted callback (task-318-02): adds the black fade overlay to the viewport (once). */
	UFUNCTION()
	void HandleScreenFadeStarted(ERRTransitionFade Fade, float Duration);

	/** Transition OnScreenFadeFinished callback: removes the overlay after the fade in (kept black after the fade out). */
	UFUNCTION()
	void HandleScreenFadeFinished(ERRTransitionFade Fade);

	/** Creates the URRScreenFadeWidget overlay (once) at ScreenFadeZOrder and starts the ticker that sets its alpha. No local player: none. */
	void AddScreenFadeOverlay();

	/** Removes the fade overlay from the game viewport. */
	void RemoveScreenFadeOverlay();

	/** Creates HUDWidgetClass, adds it to the viewport and binds it to the game mode, the miner, the run flow and the spawner. */
	void CreateHUDWidget();

	/** Creates LoadingScreenWidgetClass (once) and shows it for LevelIndex / LayoutName. */
	void ShowLoadingScreen(int32 LevelIndex, const FString& LayoutName);

	/** Removes the intermission widget from the viewport. */
	void RemoveIntermissionWidget();

	/** Removes the loading screen widget from the viewport. */
	void RemoveLoadingScreenWidget();

	/**
	 * Hides (bHideHUD true) or restores the HUD widget (minimap, bars, weapons, objectives, subtitle line) around a run
	 * transition: collapses it and keeps its previous visibility for the restore. Reason is logged. No-op without a HUD or when
	 * already in the requested state.
	 */
	void SetHUDHiddenForTransition(bool bHideHUD, const TCHAR* Reason);

	/** Rebinds the HUD widget to the newly possessed miner. */
	virtual void OnPossess(APawn* InPawn) override;

	/** Adds the mapping context (local controller only) and binds the input actions. */
	virtual void SetupInputComponent() override;

	/** Move action handler: forwards the Axis2D value to ARRMiner::ApplyMoveInput. */
	void HandleMove(const FInputActionValue& Value);

	/** Stick aim action handler: forwards the Axis2D value to ARRMiner::ApplyStickAimInput and hides the cursor. */
	void HandleStickAim(const FInputActionValue& Value);

	/** Mouse aim action handler: switches the miner to mouse aim and shows the cursor. */
	void HandleMouseAim(const FInputActionValue& Value);

	/** Place-turret action handler (Started: once per press): ARRMiner::PlaceTurret. */
	void HandlePlaceTurret(const FInputActionValue& Value);

	/**
	 * Returns the place-turret action to bind: PlaceTurretAction when set (its keys come from DefaultMappingContext), else a
	 * transient Boolean action mapped to PlaceTurretKeys in a transient mapping context added to Subsystem (when given).
	 */
	UInputAction* ResolvePlaceTurretAction(UEnhancedInputLocalPlayerSubsystem* Subsystem);

	/** Returns the possessed pawn as a miner, or nullptr. */
	ARRMiner* GetMiner() const;

	/** Mapping context added at input setup; maps keys to the actions below. */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	/** Priority of DefaultMappingContext in the Enhanced Input subsystem. */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	int32 MappingContextPriority = 0;

	/** Axis2D move action, in screen axes (X = right, Y = up). Bound on Triggered. */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	/** Axis2D stick aim action (right stick), in screen axes (X = right, Y = up). Bound on Triggered. */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> StickAimAction;

	/** Mouse aim action (mouse XY movement); any trigger switches back to mouse aim. Bound on Triggered. */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> MouseAimAction;

	/** Boolean place-turret action, bound on Started. Unset = a transient action mapped to PlaceTurretKeys is built at input setup. */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> PlaceTurretAction;

	/** Keys of the transient place-turret mapping, used only when PlaceTurretAction is unset (task-024-03: E, right mouse button, gamepad face button bottom). */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TArray<FKey> PlaceTurretKeys;

	/** Transient place-turret action built when PlaceTurretAction is unset. */
	UPROPERTY(Transient)
	TObjectPtr<UInputAction> RuntimePlaceTurretAction;

	/** Transient mapping context holding PlaceTurretKeys, added at MappingContextPriority + 1. */
	UPROPERTY(Transient)
	TObjectPtr<UInputMappingContext> RuntimePlaceTurretContext;

	/** If true, the cursor is hidden while a stick aims and shown again when the mouse moves. */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	bool bHideCursorWhileStickAiming = true;

	/** HUD widget class created at BeginPlay (a Blueprint subclass of URRHUDWidget). None = no HUD. */
	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	TSubclassOf<URRHUDWidget> HUDWidgetClass;

	/** The HUD widget created from HUDWidgetClass. */
	UPROPERTY(Transient)
	TObjectPtr<URRHUDWidget> HUDWidget;

	/** Stats screen class (a Blueprint subclass of URRIntermissionWidget, task-206-04, task-317-01). None = no screen; rr.Run.Stats.Next goes on. */
	UPROPERTY(EditDefaultsOnly, Category = "HUD|Run Transition")
	TSubclassOf<URRIntermissionWidget> IntermissionWidgetClass;

	/** Cards shop screen class (a Blueprint subclass of URRShopScreenWidget, task-317-01). None = no screen; rr.Run.Continue goes on. */
	UPROPERTY(EditDefaultsOnly, Category = "HUD|Run Transition")
	TSubclassOf<URRShopScreenWidget> ShopScreenWidgetClass;

	/** Loading screen class (a Blueprint subclass of URRLoadingScreenWidget, task-206-05). None = no screen (the hold still runs). */
	UPROPERTY(EditDefaultsOnly, Category = "HUD|Run Transition")
	TSubclassOf<URRLoadingScreenWidget> LoadingScreenWidgetClass;

	/** Viewport Z-order of the intermission and loading widgets (above the HUD, which is added at 0). */
	UPROPERTY(EditDefaultsOnly, Category = "HUD|Run Transition")
	int32 TransitionWidgetZOrder = 50;

	/** Viewport Z-order of the transition fade overlay (task-318-02): above the transition widgets (TransitionWidgetZOrder, + 1). */
	UPROPERTY(EditDefaultsOnly, Category = "HUD|Run Transition")
	int32 ScreenFadeZOrder = 60;

	/** The fade overlay in the viewport (null when no fade is drawn). */
	UPROPERTY(Transient)
	TObjectPtr<URRScreenFadeWidget> ScreenFadeWidget;

	/** Core ticker pushing GetScreenFadeAlpha to the overlay every frame, also while paused (valid while the overlay is up). */
	FTSTicker::FDelegateHandle ScreenFadeTickHandle;

	/** The intermission widget on screen. */
	UPROPERTY(Transient)
	TObjectPtr<URRIntermissionWidget> IntermissionWidget;

	/** The cards shop screen widget on screen. */
	UPROPERTY(Transient)
	TObjectPtr<URRShopScreenWidget> ShopScreenWidget;

	/** The loading screen widget on screen. */
	UPROPERTY(Transient)
	TObjectPtr<URRLoadingScreenWidget> LoadingScreenWidget;

	/** Transition subsystem bound at BeginPlay. */
	TWeakObjectPtr<URRRunTransitionSubsystem> BoundTransition;

	/** True while SetHUDHiddenForTransition(true) keeps the HUD widget collapsed. */
	bool bHUDHiddenForTransition = false;

	/** Visibility of the HUD widget before the transition hid it, restored on OnLevelReady. */
	ESlateVisibility HUDVisibilityBeforeTransition = ESlateVisibility::SelfHitTestInvisible;
};
