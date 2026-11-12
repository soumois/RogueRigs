// Rogue Rigs - player controller: Enhanced Input setup, mouse cursor, routes input to the miner.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Components/SlateWrapperTypes.h"
#include "InputCoreTypes.h"
#include "Run/RRRunLostSubsystem.h"
#include "Run/RRRunTransitionSubsystem.h"
#include "RRPlayerController.generated.h"

class ARRMiner;
class UEnhancedInputLocalPlayerSubsystem;
class UInputAction;
class UInputMappingContext;
class URRHUDWidget;
class URRIntermissionWidget;
class URRLoadingScreenWidget;
class URRPauseMenuWidget;
class URRRunLostWidget;
class URRShopScreenWidget;
class URRScreenFadeWidget;
struct FInputActionValue;

/**
 * Player controller of a run. Adds DefaultMappingContext to the local player's Enhanced Input subsystem,
 * binds the move, aim and place-turret actions and forwards them to the possessed ARRMiner. The place-turret action works
 * without assets: when PlaceTurretAction is unset, a transient action and mapping context are built from PlaceTurretKeys. Shows the mouse cursor;
 * hides it while a stick aims, shows it again on mouse movement. Assets are assigned in the Blueprint
 * subclass. Creates the HUD widget (HUDWidgetClass) for a local player. The pause menu (task-424-03) works the same way: a
 * transient action that triggers while paused, mapped to PauseMenuKeys, unless PauseMenuAction is set. No gameplay logic here.
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

	/**
	 * Pause menu (pause-menu gdd, task-424-03): pauses the game, creates PauseMenuWidgetClass when set (stats snapshot, focus on
	 * Resume), Game and UI input, cursor shown. Refused (false, reason logged) when CanOpenPauseMenu is false. Source names the
	 * caller in the log (pause key, rr.Debug.PauseMenu).
	 */
	bool OpenPauseMenu(const TCHAR* Source);

	/** Resume: removes the pause menu, unpauses the game and restores the game input mode. False (logged) when it was not open. */
	bool ClosePauseMenu(const TCHAR* Source);

	/**
	 * Exit: ends the run like the run won end screen "Main Menu" button (URRRunFlowSubsystem::ReturnToMainMenu: run reset, ore
	 * back to 0, L_MainMenu opened directly, no loading screen, no fade, no confirmation). False (logged) when the menu is not
	 * open, an exit is already requested or the run flow refused.
	 */
	bool ExitPauseMenuToMainMenu(const TCHAR* Source);

	/** Pause key: ClosePauseMenu when open, else OpenPauseMenu. */
	bool TogglePauseMenu(const TCHAR* Source);

	/** True between OpenPauseMenu and ClosePauseMenu. */
	bool IsPauseMenuOpen() const { return bPauseMenuOpen; }

	/**
	 * True when the pause menu may open: local controller in a game world, not already open, game not paused by another screen
	 * (level-up cards, end screens), no run transition (stats, shop, loading), run not ended, no game over, miner alive. The
	 * main menu has its own controller, so it never opens there. OutReason (optional) gets the refusal reason.
	 */
	bool CanOpenPauseMenu(FString* OutReason = nullptr) const;

	/** The pause menu widget on screen; null when closed or without PauseMenuWidgetClass. */
	URRPauseMenuWidget* GetPauseMenuWidget() const { return PauseMenuWidget; }

	/** The run-lost screen on screen (task-484); null before the run is lost or without RunLostWidgetClass. */
	URRRunLostWidget* GetRunLostWidget() const { return RunLostWidget; }

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

	/**
	 * Run-lost OnRunLost callback (death-screen gdd, task-484): HUD hidden, RunLostWidgetClass created above it with the recap, Game
	 * and UI input with the focus on the screen, cursor shown (the run-lost subsystem paused the game).
	 */
	UFUNCTION()
	void HandleRunLost(const FRRRunLostRecap& Recap);

	/** Removes the run-lost screen from the viewport (Retry starts the loading screen). */
	void RemoveRunLostWidget();

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

	/** Pause action handler (Started: once per press, also while paused): TogglePauseMenu. */
	void HandlePauseMenu(const FInputActionValue& Value);

	/**
	 * Returns the pause action to bind: PauseMenuAction when set (its keys come from DefaultMappingContext; it must have
	 * bTriggerWhenPaused, warned otherwise), else a transient Boolean action that triggers while paused, mapped to PauseMenuKeys
	 * in a transient mapping context added to Subsystem (when given).
	 */
	UInputAction* ResolvePauseMenuAction(UEnhancedInputLocalPlayerSubsystem* Subsystem);

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

	/** Boolean pause menu action (asset, optional), bound on Started; must trigger when paused. Unset = a transient action mapped to PauseMenuKeys. */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> PauseMenuAction;

	/** Keys of the transient pause mapping, used only when PauseMenuAction is unset (pause-menu gdd: Escape, P, gamepad Start). */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TArray<FKey> PauseMenuKeys;

	/** Transient pause action built when PauseMenuAction is unset (bTriggerWhenPaused true). */
	UPROPERTY(Transient)
	TObjectPtr<UInputAction> RuntimePauseMenuAction;

	/** Transient mapping context holding PauseMenuKeys, added at MappingContextPriority + 2. */
	UPROPERTY(Transient)
	TObjectPtr<UInputMappingContext> RuntimePauseMenuContext;

	/** Pause menu class (a Blueprint subclass of URRPauseMenuWidget: WBP_PauseMenu). None = the game still pauses, no screen; the pause key or rr.Debug.PauseMenu resume closes it. */
	UPROPERTY(EditDefaultsOnly, Category = "HUD|Pause Menu")
	TSubclassOf<URRPauseMenuWidget> PauseMenuWidgetClass;

	/** Viewport Z-order of the pause menu: above the HUD (0), below the run transition widgets (TransitionWidgetZOrder). */
	UPROPERTY(EditDefaultsOnly, Category = "HUD|Pause Menu")
	int32 PauseMenuWidgetZOrder = 40;

	/** The pause menu widget on screen. */
	UPROPERTY(Transient)
	TObjectPtr<URRPauseMenuWidget> PauseMenuWidget;

	/** True between OpenPauseMenu and ClosePauseMenu (the game is paused by the menu). */
	bool bPauseMenuOpen = false;

	/** True once ExitPauseMenuToMainMenu requested the main menu map (a second Exit is refused). */
	bool bPauseMenuExitRequested = false;

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

	/** Run-lost subsystem of this world, bound at BeginPlay (local player). */
	TWeakObjectPtr<URRRunLostSubsystem> BoundRunLost;

	/** Run-lost screen class (a Blueprint subclass of URRRunLostWidget: WBP_RunLost, task-484). None = no screen (the game stays paused; rr.Run.Lost.Retry and rr.Run.Lost.MainMenu go on). */
	UPROPERTY(EditDefaultsOnly, Category = "HUD|Run Lost")
	TSubclassOf<URRRunLostWidget> RunLostWidgetClass;

	/** Viewport Z-order of the run-lost screen: above the HUD (0) and the pause menu, below the run transition widgets (TransitionWidgetZOrder). */
	UPROPERTY(EditDefaultsOnly, Category = "HUD|Run Lost")
	int32 RunLostWidgetZOrder = 45;

	/** The run-lost screen on screen. */
	UPROPERTY(Transient)
	TObjectPtr<URRRunLostWidget> RunLostWidget;

	/** True while SetHUDHiddenForTransition(true) keeps the HUD widget collapsed. */
	bool bHUDHiddenForTransition = false;

	/** Visibility of the HUD widget before the transition hid it, restored on OnLevelReady. */
	ESlateVisibility HUDVisibilityBeforeTransition = ESlateVisibility::SelfHitTestInvisible;
};
