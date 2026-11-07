// Rogue Rigs - base class of the in-run HUD widget: health, XP, level-up, upgrade choice, alert bar, objective and game-over events fed by C++.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/SceneComponent.h"
#include "Objectives/RRConsole.h"
#include "UI/RRHUDTypes.h"
#include "UI/RRMinimap.h"
#include "Run/RRRunLostSubsystem.h"
#include "Upgrades/RRUpgradeCard.h"
#include "RRHUDWidget.generated.h"

class ARREnemySpawner;
class ARRGameMode;
class ARRMiner;
class ARRTurretLauncher;
class ARRWeapon;
class URRLevelUpComponent;
class URRHUDPanelWidget;
class URRProgressionComponent;
class URRRunFlowSubsystem;
class UAbilitySystemComponent;
class UWidget;
class FViewport;
class ARRBonusChest;
class ARRShuttle;
struct FMinimalViewInfo;
struct FOnAttributeChangeData;

/** URRHUDWidget::OnMinimapTextureReady: the minimap texture exists (once per HUD). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRROnMinimapTextureReadySignature, UTexture2D*, Texture);

/** URRHUDWidget::OnMinimapViewChanged: the minimap content or view changed; State = GetMinimapState now. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRROnMinimapViewChangedSignature, const FRRMinimapState&, State);

/** URRHUDWidget::OnEndScreenShownDelegate: an end screen shows; bRunWon = run won screen, bGameOver = miner death, both false = run lost (shuttle missed). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRROnEndScreenShownSignature, bool, bRunWon, bool, bGameOver);

/** URRHUDWidget::OnLocatorViewChanged: the locator arrow inputs changed; Locator = GetObjectiveLocator now (bHasLocator its result). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRROnLocatorViewChangedSignature, bool, bHasLocator, const FRRObjectiveLocator&, Locator);

/**
 * In-run HUD. Created by ARRPlayerController at BeginPlay (local player only) from its HUDWidgetClass. C++ binds it
 * to the miner's Health and MaxHealth attributes, to its URRProgressionComponent and URRLevelUpComponent (BindToMiner),
 * and to the game mode's OnGameOver (BindToGameMode); the Blueprint subclass only implements the display events
 * OnHealthChanged, OnXPChanged, OnLevelUp, OnUpgradeOfferShown, OnUpgradeOfferClosed and OnGameOver, and calls
 * ChooseUpgrade / RequestReroll from its card and reroll buttons (forwarded to the level-up component).
 * OnHealthChanged is never called with Max <= 0 and OnXPChanged never with Needed <= 0, so the Blueprint can divide by
 * them. Run flow (BindToRunFlow, URRRunFlowSubsystem): OnAlertChanged, OnObjectiveChanged, OnObjectiveProgressChanged and
 * OnExtractionReady, OnShuttleCountdown and OnRunEnded (end screen; its restart button calls RestartLevel). Turret charges
 * (BindToMiner: the miner's turret launcher, now or when acquired later through OnWeaponAdded): OnTurretChargesChanged, plus
 * GetTurretRechargeProgress for a per-frame bar (reload and recharge progress are the only per-frame HUD values). Minimap and
 * locator: OnMinimapTextureReady, OnMinimapViewChanged and OnLocatorViewChanged, driven by the miner's and its camera's transform
 * updates (no tick: an idle miner costs nothing). Weapon slots (ammo, reload, turret charges, locked slots): GetWeaponSlots / GetWeaponSlot. Pressure feedback: OnPlayerDamaged (red vignette, BindToMiner),
 * OnHordeIncoming (banner) and OnAlertEventsChanged (alert-bar icons: console, bonus chest, hordes, shuttle) from BindToSpawner and
 * BindToRunFlow. No gameplay logic.
 */
UCLASS(Abstract)
class ROGUERIGS_API URRHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * Listens to Miner's Health and MaxHealth changes and to its progression events (replaces a previous miner), then pushes
	 * the current values once (OnHealthChanged, OnXPChanged; each skipped while its maximum is still 0). Null unbinds.
	 */
	void BindToMiner(ARRMiner* Miner);

	/** Listens to GameMode's OnGameOver (replaces a previous game mode). Null unbinds. */
	void BindToGameMode(ARRGameMode* GameMode);

	/**
	 * Listens to the run flow's alert, objective, progress and extraction events (replaces a previous one), then pushes the
	 * current alert and objective once. Null unbinds.
	 */
	void BindToRunFlow(URRRunFlowSubsystem* RunFlow);

	/**
	 * Listens to Spawner's wave director horde events (replaces a previous spawner): OnHordeIncoming for the banner (pushed once
	 * when a horde is already announced) and the horde icons of the alert bar (refreshed now). Null unbinds.
	 */
	void BindToSpawner(ARREnemySpawner* Spawner);

	/**
	 * HUD panels (URRHUDPanelWidget) placed in this widget's tree, in tree order. Every Bind call above hands its object to each
	 * of them (URRHUDPanelWidget::SetMiner...), so the game creates and binds only this root widget.
	 */
	TArray<URRHUDPanelWidget*> GetPanels() const;

	/**
	 * Widget named Name anywhere in this HUD: its own tree first, then, depth first, the tree of every UUserWidget placed in it
	 * (the URRHUDPanelWidget children such as WBP_HealthStatus, and their own nested user widgets). Null when absent.
	 */
	UWidget* FindHUDWidget(FName Name) const { return FindWidgetInTree(this, Name); }

	/** FindHUDWidget for any user widget Root (tests, debug commands): Root's tree, then the trees of its nested user widgets. */
	static UWidget* FindWidgetInTree(const UUserWidget* Root, FName Name);

	/**
	 * User widget owning the (Blueprint) variable PropertyName: Root when its class has it, else the first nested user widget
	 * that has it, searched like FindWidgetInTree (e.g. HitFlash, now in WBP_HealthStatus). Null when absent. Tests and debug only.
	 */
	static UUserWidget* FindUserWidgetWithProperty(const UUserWidget* Root, FName PropertyName);

	/**
	 * Alert-bar icons of the bound run flow (URRRunFlowSubsystem::GetAlertBarEvents: console, bonus chest, hordes in index order,
	 * shuttle); empty without a bound run flow. The run flow computes them; this HUD only forwards them to OnAlertEventsChanged.
	 */
	UFUNCTION(BlueprintPure, Category = "HUD|Run")
	const TArray<FRRAlertBarEvent>& GetAlertBarEvents() const;

	/**
	 * Kessler-Voss line raised with alert event EventId (console_dropped, bonus_chest_dropped, horde_incoming, extraction_ready,
	 * shuttle_countdown; URRTextContentSubsystem::GetCurrentAlertLine): read it in the matching display event. Empty when no line was
	 * picked (missing file or id): keep the functional text.
	 */
	UFUNCTION(BlueprintPure, Category = "HUD|Run")
	FText GetAlertLine(FName EventId) const;

	/** Lowest render opacity the damage vignette is kept at (VignetteWarmOpacity): its "no vignette" value for checks and tests. */
	float GetVignetteWarmOpacity() const { return VignetteWarmOpacity; }

	/** Number of vignette warm-ups applied (tests: exactly 1 once the vignette exists, never per frame). */
	int32 GetVignetteWarmUpCount() const { return VignetteWarmUpCount; }

	/** Number of OnMinimapTextureReady broadcasts (tests: 1 per HUD). */
	int32 GetMinimapTextureReadyCount() const { return MinimapTextureReadyCount; }

	/** Number of OnMinimapViewChanged broadcasts (tests: none while the miner is idle). */
	int32 GetMinimapViewChangedCount() const { return MinimapViewChangedCount; }

	/** Number of OnLocatorViewChanged broadcasts (tests: none while nothing moves). */
	int32 GetLocatorViewChangedCount() const { return LocatorViewChangedCount; }

	/** Minimap data object (null until a built grid was found); tests read its seen-pass and update counters. */
	const URRMinimap* GetMinimap() const { return Minimap; }

	/**
	 * Drops the minimap and creates it again from the grid as it is now (task-494-01: the generator test map rebuilds its grid in
	 * place with a new layout). Fires OnMinimapTextureReady with the new texture, then OnMinimapViewChanged.
	 */
	void ResetMinimap();

	/**
	 * Fired once per world when an end screen shows (task-211-01): bRunWon = success screen of the last level (run won), else
	 * the run-lost screen (shuttle missed) or, bGameOver, the miner's death. Panels (URRHUDPanelWidget, BindToHUD) bind it to hide
	 * themselves; the panel that draws the end screen (WBP_AlertObjectives) keeps it. Same moment as the OnEndScreenShown event.
	 */
	UPROPERTY(BlueprintAssignable, Category = "HUD|Run")
	FRROnEndScreenShownSignature OnEndScreenShownDelegate;

	/** True once an end screen is shown in this world (run won, shuttle missed, game over); false again only in a new world (restart). */
	UFUNCTION(BlueprintPure, Category = "HUD|Run")
	bool IsEndScreenShown() const { return bEndScreenShown; }

	/** True when the shown end screen is the run won screen; false when none or a loss. */
	UFUNCTION(BlueprintPure, Category = "HUD|Run")
	bool IsEndScreenRunWon() const { return bEndScreenShown && bEndScreenRunWon; }

	/** True when the shown end screen is the miner's death (game over); false when none or a shuttle end. */
	UFUNCTION(BlueprintPure, Category = "HUD|Run")
	bool IsEndScreenGameOver() const { return bEndScreenShown && bEndScreenGameOver; }

	/** Fired once when the minimap texture exists (set it on the minimap material). Read GetMinimapTexture at bind time too. */
	UPROPERTY(BlueprintAssignable, Category = "HUD|Minimap")
	FRROnMinimapTextureReadySignature OnMinimapTextureReady;

	/**
	 * Fired only when the minimap content or view changed: cells revealed or a seen cell destroyed (pixels sent), the miner moved by
	 * more than MinimapConfig.ViewChangeThresholdCells, a marker appeared (console or bonus chest dropped, shuttle landed), a new
	 * miner bound, or the texture was created. Never while the miner is idle. Payload = GetMinimapState.
	 */
	UPROPERTY(BlueprintAssignable, Category = "HUD|Minimap")
	FRROnMinimapViewChangedSignature OnMinimapViewChanged;

	/**
	 * Fired only when the locator arrow can change: the objective target changed (run flow OnLocatorTargetChanged, also to None:
	 * hide), the miner camera moved by more than LocatorMoveThreshold or turned by more than LocatorTurnThresholdDegrees, the miner
	 * moved by more than LocatorMoveThreshold (distance text), the viewport was resized, or a new miner bound. Never while no target
	 * exists, never while nothing moves. Payload = GetObjectiveLocator(LocatorEdgeMargin).
	 */
	UPROPERTY(BlueprintAssignable, Category = "HUD|Locator")
	FRROnLocatorViewChangedSignature OnLocatorViewChanged;

	/**
	 * Estimated bar position of an event SecondsUntil seconds away: Alert + SecondsUntil / AlertFillSeconds (the base fill rate,
	 * without kills or pause), clamped to [Alert, 1]. Never behind the fill; recomputed as the fill moves.
	 */
	static float EstimateBarPosition(float Alert, float SecondsUntil, float AlertFillSeconds);

	/**
	 * Card button: picks card Index (0-based, order of OnUpgradeOfferShown's Cards) through the bound miner's
	 * URRLevelUpComponent::ChooseUpgrade. The next chained screen or OnUpgradeOfferClosed follows. False when refused.
	 */
	UFUNCTION(BlueprintCallable, Category = "HUD|Upgrades")
	bool ChooseUpgrade(int32 Index);

	/** Reroll button: URRLevelUpComponent::RequestReroll; OnUpgradeOfferShown fires again with the new cards. False when refused. */
	UFUNCTION(BlueprintCallable, Category = "HUD|Upgrades")
	bool RequestReroll();

	/** True while a choice screen is open and the miner can pay the reroll in ore (reroll button enabled, greyed otherwise). */
	UFUNCTION(BlueprintPure, Category = "HUD|Upgrades")
	bool CanRerollUpgrade() const;

	/** Ore cost of the next reroll (URRLevelUpComponent::GetRerollCost; 0 without a bound miner). Shown on the reroll button. */
	UFUNCTION(BlueprintPure, Category = "HUD|Upgrades")
	int32 GetUpgradeRerollCost() const;

	/** Progress 0..1 of the bound turret launcher's running recharge (0 when none or without a launcher), for a per-frame bar. */
	UFUNCTION(BlueprintPure, Category = "HUD|Turret")
	float GetTurretRechargeProgress() const;

	/** Number of weapon slots to draw: the bound miner's MaxWeapons (0 without a bound miner). */
	UFUNCTION(BlueprintPure, Category = "HUD|Weapons")
	int32 GetWeaponSlotCount() const;

	/**
	 * HUD state of weapon slot SlotIndex (0-based, the miner's weapons in acquisition order: slot 0 = starting burst rifle). A slot
	 * past the owned weapons, an index out of range or no bound miner gives a locked slot (bOwned false). Cheap; call each frame.
	 */
	UFUNCTION(BlueprintPure, Category = "HUD|Weapons")
	FRRWeaponSlotHUD GetWeaponSlot(int32 SlotIndex) const;

	/** Every slot, GetWeaponSlotCount entries in slot order (owned weapons first, then locked slots). Call each frame. */
	UFUNCTION(BlueprintPure, Category = "HUD|Weapons")
	TArray<FRRWeaponSlotHUD> GetWeaponSlots() const;

	/** HUD state of Weapon (locked slot when null): magazine and reload, or charges and recharge for a turret launcher. */
	static FRRWeaponSlotHUD MakeWeaponSlot(const ARRWeapon* Weapon);

	// --- Objective locator (objective-console: edge arrow to the console, then to the shuttle) ---

	/**
	 * Current objective target of the bound run flow: the console while it exists and is not hacked, else the landed shuttle;
	 * None otherwise (no console yet, between the hack and the landing, run ended). False for None.
	 */
	UFUNCTION(BlueprintPure, Category = "HUD|Locator")
	bool GetObjectiveTarget(ERRLocatorTarget& OutTarget, FVector& OutWorldLocation) const;

	/**
	 * Locator for an edge arrow (OnLocatorViewChanged sends it whenever it can change): target (GetObjectiveTarget), its projection with the locator view
	 * (GetLocatorView) in viewport widget units, on-screen flag, edge point (viewport inset by EdgeMargin widget units) and arrow angle. False
	 * when there is no target or no owning player camera (Out.Target and Out.WorldLocation are still filled).
	 */
	UFUNCTION(BlueprintPure, Category = "HUD|Locator")
	bool GetObjectiveLocator(FRRObjectiveLocator& OutLocator, float EdgeMargin = 48.f) const;

	/**
	 * Perspective projection of WorldLocation with View (horizontal FOV) on a viewport of ViewportSize (any unit, top-left
	 * origin). A point behind the camera is pushed far out along its camera-space direction, so the edge arrow still points the
	 * right way. False when behind the camera.
	 */
	static bool ProjectToViewport(const FMinimalViewInfo& View, const FVector2D& ViewportSize, const FVector& WorldLocation, FVector2D& OutScreenPosition);

	/**
	 * Edge arrow of ScreenPosition on a viewport of ViewportSize: true when it lies inside the viewport (OutEdgePosition = the
	 * position itself); else OutEdgePosition = where the line from the center meets the viewport inset by EdgeMargin.
	 * OutArrowAngle = direction from the center, degrees clockwise from screen right.
	 */
	static bool ComputeEdgeLocator(const FVector2D& ScreenPosition, const FVector2D& ViewportSize, float EdgeMargin, FVector2D& OutEdgePosition, float& OutArrowAngle);

	/** Reopens the current map (restart button of the success and run-lost screens). Forwarded to URRRunFlowSubsystem::RestartLevel. */
	UFUNCTION(BlueprintCallable, Category = "HUD|Run")
	void RestartLevel();

	// --- Minimap (readability-rules Minimap line: 200 px circle centred on the miner, seen terrain, objective markers) ---

	/**
	 * Minimap texture: one pixel per grid cell, UnseenColor until seen, nearest filter, clamp. Null until the HUD found a built
	 * grid (OnMinimapTextureReady fires then). Screen-aligned for camera yaw 0 (up = world +X, right = world +Y).
	 */
	UFUNCTION(BlueprintPure, Category = "HUD|Minimap")
	UTexture2D* GetMinimapTexture() const;

	/** Per-frame minimap state (miner and marker positions in texture pixels, UVs, view radius). False without a texture. */
	UFUNCTION(BlueprintPure, Category = "HUD|Minimap")
	bool GetMinimapState(FRRMinimapState& OutState) const;

	/**
	 * Offset of Position from Center in circle radii (Radius in the same unit, e.g. ViewRadiusCells): multiply by the widget
	 * circle radius (100 px) to place a marker. bOutOfRange true beyond the circle; with bClampToCircle the result is then
	 * clamped to length 1 (marker pinned on the rim).
	 */
	UFUNCTION(BlueprintPure, Category = "HUD|Minimap")
	static FVector2D GetMinimapOffset(FVector2D Position, FVector2D Center, float Radius, bool bClampToCircle, bool& bOutOfRange);

protected:
	/** Hands itself to the panels (BindToHUD), warms the vignette up and creates the minimap when a built grid exists. */
	virtual void NativeOnInitialized() override;

	/** Same as NativeOnInitialized (idempotent), and listens to viewport resizes for the locator. */
	virtual void NativeConstruct() override;

	/** Removes every binding. */
	virtual void NativeDestruct() override;

	// No NativeTick override (task-103-01): nothing in this root changes every frame. The reload and turret recharge progress
	// are read per frame by their panels (GetTurretRechargeProgress, FRRWeaponSlotHUD::ReloadProgress) only while they run.

	/**
	 * One-shot warm-up: raises the damage vignette's render opacity to VignetteWarmOpacity when lower. Slate culls a draw whose
	 * opacity is 0, so a vignette at 0 never renders its material until the first hit: its shaders (and pipeline state) are then
	 * not ready on that frame and the first flash is skipped (task-055). WBP_HealthStatus keeps the floor itself afterwards
	 * (ApplyVignette clamps to 0.001), so one call is enough. Looks the widget up by DamageVignetteWidgetName through FindHUDWidget
	 * until found (initialization, construct, each BindToMiner = when the panel holding it is bound), then never again.
	 */
	void WarmUpVignette();

	/** Creates the minimap on the first built grid found (fires OnMinimapTextureReady, then OnMinimapViewChanged). Retries on the
	 *  next ticks (MinimapRetryFrames at most) and on each miner move while no built grid exists. */
	void TryCreateMinimap();

	/** Next-tick retry of TryCreateMinimap (startup only: the grid builds in its BeginPlay). */
	void RetryCreateMinimap();

	/** Miner root TransformUpdated: seen pass and texture flush, then OnMinimapViewChanged when content or view changed. */
	void HandleMinerMoved(USceneComponent* UpdatedComponent, EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport);

	/** Miner camera TransformUpdated (after the camera's own update, so its view is fresh): OnLocatorViewChanged beyond the thresholds. */
	void HandleLocatorViewMoved(USceneComponent* UpdatedComponent, EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport);

	/** Minimap OnPixelsDirtied (a seen cell destroyed): one flush on the next tick for every cell destroyed this frame. */
	void HandleMinimapPixelsDirtied();

	/** Next-tick flush of the destroyed cells' pixels, then OnMinimapViewChanged. */
	void FlushMinimapPixels();

	/** FViewport::ViewportResizedEvent: the locator's edge positions depend on the viewport size. */
	void HandleViewportResized(FViewport* Viewport, uint32 Unused);

	/** Run flow OnLocatorTargetChanged: OnLocatorViewChanged. */
	UFUNCTION()
	void HandleLocatorTargetChanged(ERRLocatorTarget Target, FVector WorldLocation);

	/** Run flow OnConsoleDropped: a minimap marker appeared. */
	UFUNCTION()
	void HandleConsoleDropped(ARRConsole* Console);

	/** Run flow OnBonusChestDropped: a minimap marker appeared. */
	UFUNCTION()
	void HandleBonusChestDropped(ARRBonusChest* BonusChest);

	/** Run flow OnShuttleLanded: a minimap marker appeared. */
	UFUNCTION()
	void HandleShuttleLanded(ARRShuttle* Shuttle);

	/** Broadcasts OnMinimapViewChanged with GetMinimapState (no-op without minimap) and remembers the miner position sent. */
	void BroadcastMinimapView();

	/** Broadcasts OnLocatorViewChanged with GetObjectiveLocator and remembers the camera and miner positions sent. */
	void BroadcastLocatorView();

	/** Locator view: the bound miner's camera component view (fresh in its TransformUpdated) when it has one, else the owning
	 *  player's camera cache. False when neither exists. Camera shake (render-only) is ignored. */
	bool GetLocatorView(FMinimalViewInfo& OutView) const;

	/** Removes the miner transform bindings. */
	void UnbindMinerMovement();

	/** Minimap data (texture, seen mask); null until a built grid is found or when MinimapConfig.bEnabled is false. */
	UPROPERTY(Transient)
	TObjectPtr<URRMinimap> Minimap;

	/** Minimap tunables: enable flag, view radius in cells, terrain and unseen colors (edit on the HUD widget Blueprint). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HUD|Minimap")
	FRRMinimapConfig MinimapConfig;

	/**
	 * Display event: the miner lost Amount health (> 0); HealthFraction is the health left over MaxHealth (0..1, 0 = dead). For the
	 * red damage vignette, stronger at low HealthFraction. Not called for a clamp from a MaxHealth decrease. Implement in Blueprint.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "HUD|Pressure")
	void OnPlayerDamaged(float Amount, float HealthFraction);

	/**
	 * Display event: a horde front starts in SecondsUntil seconds (banner "HORDE INCOMING!"). Its Kessler-Voss line is shown as a
	 * subtitle just before (URRSubtitleSubsystem; the banner does not show it). Implement in Blueprint.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "HUD|Pressure")
	void OnHordeIncoming(float SecondsUntil);

	/**
	 * Display event: the alert-bar icons changed (also on binding): console, bonus chest (when its threshold is > 0), each horde in index order, shuttle; see
	 * FRRAlertBarEvent. Forwarded from the bound run flow's OnAlertBarEventsChanged (same list as GetAlertBarEvents). WBP_HUD does not
	 * implement it (WBP_AlertObjectives binds the run flow directly); kept for C++ subclasses and tests.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "HUD|Pressure")
	void OnAlertEventsChanged(const TArray<FRRAlertBarEvent>& Events);

	/** Native default of OnPlayerDamaged: nothing (the Blueprint draws the vignette). */
	virtual void OnPlayerDamaged_Implementation(float Amount, float HealthFraction) {}

	/** Native default of OnHordeIncoming: nothing (the Blueprint shows the banner). */
	virtual void OnHordeIncoming_Implementation(float SecondsUntil) {}

	/** Native default of OnAlertEventsChanged: nothing (the Blueprint places the icons). */
	virtual void OnAlertEventsChanged_Implementation(const TArray<FRRAlertBarEvent>& Events) {}

	/** Picks the horde_incoming Kessler-Voss line and shows it as a subtitle (URRSubtitleSubsystem::ShowAlertLine). */
	void ShowHordeAlertLine();

	/** Run flow OnMinerRegistered callback: binds that miner when none is bound yet (miner created after the HUD). */
	UFUNCTION()
	void HandleMinerRegistered(ARRMiner* Miner);

	/** Run flow OnSpawnerRegistered callback: binds that spawner when none is bound yet (spawner created after the HUD). */
	UFUNCTION()
	void HandleSpawnerRegistered(ARREnemySpawner* Spawner);

	/** Spawner OnHordeIncoming callback: shows the horde subtitle and calls OnHordeIncoming. */
	UFUNCTION()
	void HandleHordeIncoming(int32 HordeIndex, float SecondsUntil);

	/** Run flow OnAlertBarEventsChanged callback: forwards the list to OnAlertEventsChanged. */
	UFUNCTION()
	void HandleAlertBarEventsChanged(const TArray<FRRAlertBarEvent>& Events);

	/** Name of the damage vignette Image, searched through the panels (WBP_HealthStatus: DamageVignette); none found = no warm-up. */
	UPROPERTY(EditDefaultsOnly, Category = "HUD|Pressure")
	FName DamageVignetteWidgetName = TEXT("DamageVignette");

	/**
	 * Lowest render opacity of the damage vignette: above Slate's cull threshold (1e-4) so its material keeps rendering, small
	 * enough to stay invisible (under half an 8-bit step). 0 disables the warm-up.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "HUD|Pressure", meta = (ClampMin = 0, ClampMax = 0.01))
	float VignetteWarmOpacity = 0.001f;

	/** The damage vignette widget found by WarmUpVignette (null when the Blueprint has none). */
	TWeakObjectPtr<UWidget> DamageVignette;

	/** True once WarmUpVignette found and warmed the vignette (never again). */
	bool bVignetteWarmedUp = false;

	/** See GetVignetteWarmUpCount. */
	int32 VignetteWarmUpCount = 0;

	/** Camera move (uu) and miner move (uu, distance text) below which OnLocatorViewChanged is not sent (about 0.4 px at 1700 uu). */
	UPROPERTY(EditDefaultsOnly, Category = "HUD|Locator", meta = (ClampMin = 0))
	float LocatorMoveThreshold = 1.f;

	/** Camera turn in degrees below which OnLocatorViewChanged is not sent. */
	UPROPERTY(EditDefaultsOnly, Category = "HUD|Locator", meta = (ClampMin = 0))
	float LocatorTurnThresholdDegrees = 0.1f;

	/** Edge margin (widget units) of the locator sent by OnLocatorViewChanged (GetObjectiveLocator's default). */
	UPROPERTY(EditDefaultsOnly, Category = "HUD|Locator", meta = (ClampMin = 0))
	float LocatorEdgeMargin = 48.f;

	/** Next-tick retries of TryCreateMinimap at startup before relying on miner moves only. */
	UPROPERTY(EditDefaultsOnly, Category = "HUD|Minimap", meta = (ClampMin = 0))
	int32 MinimapRetryFrames = 120;

	/** Retries of TryCreateMinimap done. */
	int32 MinimapRetries = 0;

	/** True while a next-tick retry is scheduled. */
	bool bMinimapRetryPending = false;

	/** True while a next-tick pixel flush is scheduled. */
	bool bMinimapFlushPending = false;

	/** Miner position (texture pixels) of the last OnMinimapViewChanged. */
	FVector2D LastMinimapMinerPosition = FVector2D::ZeroVector;

	/** True once OnMinimapViewChanged was sent (the first move always sends). */
	bool bMinimapViewSent = false;

	/** Camera location of the last OnLocatorViewChanged. */
	FVector LastLocatorViewLocation = FVector::ZeroVector;

	/** Camera rotation of the last OnLocatorViewChanged. */
	FRotator LastLocatorViewRotation = FRotator::ZeroRotator;

	/** Miner location of the last OnLocatorViewChanged. */
	FVector LastLocatorMinerLocation = FVector::ZeroVector;

	/** True once OnLocatorViewChanged was sent. */
	bool bLocatorViewSent = false;

	/** See GetMinimapTextureReadyCount. */
	int32 MinimapTextureReadyCount = 0;

	/** See GetMinimapViewChangedCount. */
	int32 MinimapViewChangedCount = 0;

	/** See GetLocatorViewChangedCount. */
	int32 LocatorViewChangedCount = 0;

	/** Miner root whose TransformUpdated drives the minimap. */
	TWeakObjectPtr<USceneComponent> BoundMinerRoot;

	/** Miner camera (or root when it has none) whose TransformUpdated drives the locator. */
	TWeakObjectPtr<USceneComponent> BoundLocatorComponent;

	/** Handle of the miner root TransformUpdated binding. */
	FDelegateHandle MinerMovedHandle;

	/** Handle of the locator component TransformUpdated binding. */
	FDelegateHandle LocatorMovedHandle;

	/** Handle of the minimap OnPixelsDirtied binding. */
	FDelegateHandle MinimapDirtiedHandle;

	/** Handle of the FViewport::ViewportResizedEvent binding. */
	FDelegateHandle ViewportResizedHandle;

	/** Spawner whose horde events are bound. */
	TWeakObjectPtr<ARREnemySpawner> BoundSpawner;

	/** Display event: the miner's health changed (also called once on binding). Max is always > 0. Implement in Blueprint. */
	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void OnHealthChanged(float Current, float Max);

	/** Display event: the miner's XP changed (also called once on binding): XP in the level, XP to leave it (always > 0), level. Implement in Blueprint. */
	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void OnXPChanged(float Current, float Needed, int32 Level);

	/** Display event: the miner reached NewLevel (once per level gained, before OnXPChanged). Implement in Blueprint. */
	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void OnLevelUp(int32 NewLevel);

	/**
	 * Display event: show (or refresh) the choice screen with Cards (game paused, cursor visible). Called when a screen opens,
	 * after a reroll and for each chained screen; bCanReroll = the next reroll is affordable now (cost: GetUpgradeRerollCost).
	 * Implement in Blueprint.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "HUD|Upgrades")
	void OnUpgradeOfferShown(const TArray<FRRUpgradeCard>& Cards, bool bCanReroll);

	/** Display event: hide the choice screen (the last queued choice was made; the game resumes). Implement in Blueprint. */
	UFUNCTION(BlueprintImplementableEvent, Category = "HUD|Upgrades")
	void OnUpgradeOfferClosed();

	/**
	 * Display event: the turret launcher's charges changed (also on binding): charges ready, maximum, duration in seconds of the
	 * recharge that runs now (0 = none, all charges ready). Not called while the miner has no launcher. Implement in Blueprint.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "HUD|Turret")
	void OnTurretChargesChanged(int32 Charges, int32 MaxCharges, float RechargeDuration);

	/**
	 * Display event: the miner died (game mode OnGameOver), at the lethal hit. The death sequence follows; the end screen moment
	 * (HUD hidden) is the run-lost screen (URRRunLostSubsystem::OnRunLost, task-484). Implement in Blueprint.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void OnGameOver();

	/** Display event: the alert bar value (0..1) or its paused flag (a mandatory objective is pending) changed; also on binding. Implement in Blueprint. */
	UFUNCTION(BlueprintImplementableEvent, Category = "HUD|Run")
	void OnAlertChanged(float Alert, bool bPaused);

	/**
	 * Display event: the objective changed (also on binding). bActive is false while no console exists (hide the objective
	 * panel); State Locked = destroy the marked rocks (MarkedRocksLeft of MarkedRocksTotal), Idle = go to the console, Hacking =
	 * stay in the zone, Complete = hacked. Implement in Blueprint (the text lives there).
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "HUD|Run")
	void OnObjectiveChanged(bool bActive, ERRConsoleState State, int32 MarkedRocksLeft, int32 MarkedRocksTotal);

	/** Display event: the console hack progress (0..1) or hacking flag changed. Implement in Blueprint. */
	UFUNCTION(BlueprintImplementableEvent, Category = "HUD|Run")
	void OnObjectiveProgressChanged(float Progress, bool bHacking);

	/** Display event: the alert bar is full and no objective is pending (extraction starts). Implement in Blueprint. */
	UFUNCTION(BlueprintImplementableEvent, Category = "HUD|Run")
	void OnExtractionReady();

	/**
	 * Display event: the shuttle countdown (seconds left) and the shuttle location (off-screen marker); every countdown frame, at
	 * landing and on binding while it counts. Implement in Blueprint.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "HUD|Run")
	void OnShuttleCountdown(float Remaining, FVector ShuttleLocation);

	/**
	 * Display event: the run ended. bSuccess true = success screen (miner extracted), false = run-lost screen (shuttle missed).
	 * The game is paused, input is game-and-UI with the cursor shown, any upgrade screen was closed first. Both screens offer a
	 * button calling RestartLevel. Also on binding after the end. Implement in Blueprint.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "HUD|Run")
	void OnRunEnded(bool bSuccess);

	/**
	 * Display event (task-211-01): an end screen shows, once per world, right after OnRunEnded or OnGameOver (and on binding
	 * after the end). bRunWon = run won screen; bGameOver = miner death; both false = run lost (shuttle missed). Hide every HUD
	 * element except the end screen here; the restart opens a new world with a fresh HUD. Implement in Blueprint.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "HUD|Run")
	void OnEndScreenShown(bool bRunWon, bool bGameOver);

	/** Sets the end screen state once (later calls ignored), then calls OnEndScreenShown and OnEndScreenShownDelegate. */
	void ShowEndScreen(bool bRunWon, bool bGameOver);

	/** Run flow OnShuttleCountdown callback. */
	UFUNCTION()
	void HandleShuttleCountdown(float RemainingSeconds, FVector ShuttleLocation);

	/** Run flow OnRunEnded callback. */
	UFUNCTION()
	void HandleRunEnded(bool bSuccess);

	/** Run-lost OnRunLost callback (task-484): a death shows the end screen state (bGameOver) now, after the death sequence. */
	UFUNCTION()
	void HandleRunLost(const FRRRunLostRecap& Recap);

	/** Run flow OnAlertChanged callback. */
	UFUNCTION()
	void HandleAlertChanged(float Alert, bool bPaused);

	/** Run flow OnObjectiveChanged callback: calls OnObjectiveChanged with bActive = Console is valid. */
	UFUNCTION()
	void HandleObjectiveChanged(ARRConsole* Console, ERRConsoleState State, int32 MarkedRocksLeft, int32 MarkedRocksTotal);

	/** Run flow OnObjectiveProgressChanged callback. */
	UFUNCTION()
	void HandleObjectiveProgressChanged(float Progress, bool bHacking);

	/** Run flow OnExtractionReady callback. */
	UFUNCTION()
	void HandleExtractionReady();

	/** ASC callback for Health and MaxHealth: forwards the current values to OnHealthChanged. */
	void HandleHealthAttributeChanged(const FOnAttributeChangeData& ChangeData);

	/** Game mode OnGameOver callback: calls OnGameOver. */
	UFUNCTION()
	void HandleGameOver();

	/** Progression OnXPChanged callback: calls OnXPChanged unless Needed <= 0. */
	UFUNCTION()
	void HandleXPChanged(float Current, float Needed, int32 Level);

	/** Progression OnLevelUp callback: calls OnLevelUp. */
	UFUNCTION()
	void HandleLevelUp(int32 NewLevel);

	/** Level-up OnUpgradeOfferShown callback: calls OnUpgradeOfferShown. */
	UFUNCTION()
	void HandleUpgradeOfferShown(const TArray<FRRUpgradeCard>& Cards, bool bCanReroll);

	/** Level-up OnUpgradeOfferClosed callback: calls OnUpgradeOfferClosed. */
	UFUNCTION()
	void HandleUpgradeOfferClosed();

	/** Turret launcher OnTurretChargesChanged callback: calls OnTurretChargesChanged. */
	UFUNCTION()
	void HandleTurretChargesChanged(int32 Charges, int32 MaxCharges, float RechargeDuration);

	/** Miner OnWeaponAdded callback: binds a newly acquired turret launcher when none is bound. */
	UFUNCTION()
	void HandleWeaponAdded(ARRWeapon* Weapon);

	/** Listens to Launcher's charge changes (replaces a previous one) and pushes its current charges once. Null unbinds. */
	void BindToTurretLauncher(ARRTurretLauncher* Launcher);

	/** Reads Health and MaxHealth on the bound ASC and calls OnHealthChanged, unless MaxHealth <= 0 (stats not applied yet). */
	void BroadcastHealth();

	/** Removes the Health, MaxHealth and progression bindings. */
	void UnbindMiner();

	/** ASC of the bound miner. */
	TWeakObjectPtr<UAbilitySystemComponent> BoundASC;

	/** Progression component of the bound miner. */
	TWeakObjectPtr<URRProgressionComponent> BoundProgression;

	/** Level-up flow of the bound miner. */
	TWeakObjectPtr<URRLevelUpComponent> BoundLevelUp;

	/** Miner whose OnWeaponAdded is bound. */
	TWeakObjectPtr<ARRMiner> BoundMiner;

	/** Turret launcher whose OnTurretChargesChanged is bound. */
	TWeakObjectPtr<ARRTurretLauncher> BoundTurretLauncher;

	/** Game mode whose OnGameOver is bound. */
	TWeakObjectPtr<ARRGameMode> BoundGameMode;

	/** True once an end screen is shown (ShowEndScreen). */
	bool bEndScreenShown = false;

	/** The shown end screen is the run won screen. */
	bool bEndScreenRunWon = false;

	/** The shown end screen is the miner's death. */
	bool bEndScreenGameOver = false;

	/** Run flow whose events are bound. */
	TWeakObjectPtr<URRRunFlowSubsystem> BoundRunFlow;

	/** Run-lost subsystem of the bound run flow's world (OnRunLost bound). */
	TWeakObjectPtr<URRRunLostSubsystem> BoundRunLost;

	/** Handle of the Health change binding on BoundASC. */
	FDelegateHandle HealthChangedHandle;

	/** Handle of the MaxHealth change binding on BoundASC. */
	FDelegateHandle MaxHealthChangedHandle;
};
