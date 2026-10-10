// Rogue Rigs - base class of the in-run HUD widget: health, XP, level-up, upgrade choice, alert bar, objective and game-over events fed by C++.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Objectives/RRConsole.h"
#include "UI/RRMinimap.h"
#include "Upgrades/RRUpgradeCard.h"
#include "RRHUDWidget.generated.h"

class ARREnemySpawner;
class ARRGameMode;
class ARRMiner;
class ARRTurretLauncher;
class ARRWeapon;
class URRLevelUpComponent;
class URRProgressionComponent;
class URRRunFlowSubsystem;
class UAbilitySystemComponent;
class UWidget;
struct FMinimalViewInfo;
struct FOnAttributeChangeData;

/** What the objective locator points at. */
UENUM(BlueprintType)
enum class ERRLocatorTarget : uint8
{
	/** Nothing to point at (no console yet, console hacked and no shuttle yet, run ended). */
	None,
	/** The command console, until it is hacked. */
	Console,
	/** The extraction shuttle, once landed. */
	Shuttle
};

/** Objective locator state for an edge arrow (objective-console: console until hacked, then the shuttle). */
USTRUCT(BlueprintType)
struct FRRObjectiveLocator
{
	GENERATED_BODY()

	/** What the locator points at; None = hide the arrow. */
	UPROPERTY(BlueprintReadOnly, Category = "HUD|Locator")
	ERRLocatorTarget Target = ERRLocatorTarget::None;

	/** World location of the target (actor location). */
	UPROPERTY(BlueprintReadOnly, Category = "HUD|Locator")
	FVector WorldLocation = FVector::ZeroVector;

	/** True when the target projects inside the viewport (hide the arrow, or show an on-target marker). */
	UPROPERTY(BlueprintReadOnly, Category = "HUD|Locator")
	bool bOnScreen = false;

	/** Projected target position in viewport widget units (DPI-scaled, top-left origin); far outside when off screen. */
	UPROPERTY(BlueprintReadOnly, Category = "HUD|Locator")
	FVector2D ScreenPosition = FVector2D::ZeroVector;

	/** Arrow position: the target position when on screen, else the point where the line from the viewport center to it meets the viewport inset by the edge margin (widget units). */
	UPROPERTY(BlueprintReadOnly, Category = "HUD|Locator")
	FVector2D EdgePosition = FVector2D::ZeroVector;

	/** Arrow angle in degrees, clockwise from screen right (0 = right, 90 = down, -90 = up), for a right-pointing arrow image's render angle. */
	UPROPERTY(BlueprintReadOnly, Category = "HUD|Locator")
	float ArrowAngle = 0.f;

	/** Viewport size in widget units used for the computation. */
	UPROPERTY(BlueprintReadOnly, Category = "HUD|Locator")
	FVector2D ViewportSize = FVector2D::ZeroVector;
};

/** Kind of an icon on the alert bar. */
UENUM(BlueprintType)
enum class ERRAlertBarEventType : uint8
{
	/** The command console drop (the bar pauses there until it is hacked). */
	Console,
	/** A horde front of the spawner's wave director. */
	Horde,
	/** The extraction shuttle (end of the bar). */
	Extraction,
	/** The optional bonus chest drop (gold icon; done once hacked; the bar never pauses there). */
	BonusChest
};

/** One icon of the alert bar: what, where on the bar, and whether it already happened. */
USTRUCT(BlueprintType)
struct FRRAlertBarEvent
{
	GENERATED_BODY()

	/** Kind of icon (console, bonus chest, horde, shuttle). */
	UPROPERTY(BlueprintReadOnly, Category = "HUD|Run")
	ERRAlertBarEventType Type = ERRAlertBarEventType::Console;

	/** Position on the bar, 0..1 (same scale as OnAlertChanged's Alert). A pending horde's position is an estimate (sys-hud). */
	UPROPERTY(BlueprintReadOnly, Category = "HUD|Run")
	float Position = 0.f;

	/** True once the event happened (console hacked, bonus chest hacked, horde front started, extraction ready). */
	UPROPERTY(BlueprintReadOnly, Category = "HUD|Run")
	bool bDone = false;

	/** Horde index (spawner DirectorConfig.Hordes) for a Horde icon; -1 otherwise. */
	UPROPERTY(BlueprintReadOnly, Category = "HUD|Run")
	int32 Index = INDEX_NONE;
};

/** HUD state of one weapon slot (index 0..MaxWeapons-1), read each frame by the Blueprint (GetWeaponSlot / GetWeaponSlots). */
USTRUCT(BlueprintType)
struct FRRWeaponSlotHUD
{
	GENERATED_BODY()

	/** True when the slot holds a weapon; false = locked slot (padlock), every other field is at its default. */
	UPROPERTY(BlueprintReadOnly, Category = "HUD|Weapons")
	bool bOwned = false;

	/** Weapon in the slot (null when locked). */
	UPROPERTY(BlueprintReadOnly, Category = "HUD|Weapons")
	TObjectPtr<ARRWeapon> Weapon = nullptr;

	/** True when the weapon is a turret launcher: draw Charges, MaxCharges and RechargeProgress instead of the magazine. */
	UPROPERTY(BlueprintReadOnly, Category = "HUD|Weapons")
	bool bIsTurretLauncher = false;

	/** Rounds left in the magazine (Ammo rounded down; the charges for a turret launcher). */
	UPROPERTY(BlueprintReadOnly, Category = "HUD|Weapons")
	int32 Rounds = 0;

	/** Magazine size (MagazineSize rounded down; the maximum charges for a turret launcher). */
	UPROPERTY(BlueprintReadOnly, Category = "HUD|Weapons")
	int32 MagazineSize = 0;

	/** True while the magazine reloads (always false for a turret launcher, see bRecharging). */
	UPROPERTY(BlueprintReadOnly, Category = "HUD|Weapons")
	bool bReloading = false;

	/** Progress 0..1 of the running reload (0 when not reloading or when the reload never ends). */
	UPROPERTY(BlueprintReadOnly, Category = "HUD|Weapons")
	float ReloadProgress = 0.f;

	/** Turret launcher: charges ready (0 for other weapons). */
	UPROPERTY(BlueprintReadOnly, Category = "HUD|Weapons")
	int32 Charges = 0;

	/** Turret launcher: maximum charges (0 for other weapons). */
	UPROPERTY(BlueprintReadOnly, Category = "HUD|Weapons")
	int32 MaxCharges = 0;

	/** Turret launcher: true while one charge recharges (one at a time; drawn in the leftmost empty block). */
	UPROPERTY(BlueprintReadOnly, Category = "HUD|Weapons")
	bool bRecharging = false;

	/** Turret launcher: progress 0..1 of the charge recharging now (0 when none). */
	UPROPERTY(BlueprintReadOnly, Category = "HUD|Weapons")
	float RechargeProgress = 0.f;
};

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
 * GetTurretRechargeProgress for a per-frame bar. Weapon slots (ammo, reload, turret charges, locked slots): GetWeaponSlots / GetWeaponSlot. Pressure feedback: OnPlayerDamaged (red vignette, BindToMiner),
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
	 * Recomputes the alert-bar icons from the bound run flow and spawner and calls OnAlertEventsChanged when they changed (count,
	 * type, done flag, or a position moved by more than AlertEventPositionTolerance). Called on every bound event and each tick.
	 */
	void RefreshAlertBarEvents();

	/** Alert-bar icons last sent to OnAlertEventsChanged (console, bonus chest, hordes in index order, shuttle). */
	UFUNCTION(BlueprintPure, Category = "HUD|Run")
	const TArray<FRRAlertBarEvent>& GetAlertBarEvents() const { return AlertBarEvents; }

	/**
	 * Kessler-Voss line raised with alert event EventId (console_dropped, bonus_chest_dropped, horde_incoming, extraction_ready,
	 * shuttle_countdown; URRTextContentSubsystem::GetCurrentAlertLine): read it in the matching display event. Empty when no line was
	 * picked (missing file or id): keep the functional text.
	 */
	UFUNCTION(BlueprintPure, Category = "HUD|Run")
	FText GetAlertLine(FName EventId) const;

	/** Lowest render opacity the damage vignette is kept at (VignetteWarmOpacity): its "no vignette" value for checks and tests. */
	float GetVignetteWarmOpacity() const { return VignetteWarmOpacity; }

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

	/** True while the open choice screen can still be rerolled (reroll button enabled). */
	UFUNCTION(BlueprintPure, Category = "HUD|Upgrades")
	bool CanRerollUpgrade() const;

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
	 * Locator for an edge arrow, to call each frame: target (GetObjectiveTarget), its projection with the owning player's camera
	 * in viewport widget units, on-screen flag, edge point (viewport inset by EdgeMargin widget units) and arrow angle. False
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
	/** Starts drawing the damage vignette at VignetteWarmOpacity (KeepVignetteWarm). */
	virtual void NativeConstruct() override;

	/** Removes every binding. */
	virtual void NativeDestruct() override;

	/**
	 * Refreshes the alert-bar icons (a pending horde's estimate moves while the bar is paused), then, after the Blueprint tick
	 * (UpdateVignette), keeps the damage vignette at VignetteWarmOpacity at least.
	 */
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	/**
	 * Raises the damage vignette's render opacity to VignetteWarmOpacity when lower. Slate culls a draw whose opacity is 0, so a
	 * vignette at 0 never renders its material until the first hit: its shaders (and pipeline state) are then not ready on that
	 * frame and the first flash is skipped. A floor keeps the material drawn, hence warm, from the HUD construct on. The first
	 * call looks the widget up by DamageVignetteWidgetName.
	 */
	void KeepVignetteWarm();

	/** Creates the minimap on the first built grid found (fires OnMinimapTextureReady), then ticks it (seen cells, texture). */
	void UpdateMinimap();

	/** Display event: the minimap texture exists (once per HUD; set it on the minimap material). Implement in Blueprint. */
	UFUNCTION(BlueprintImplementableEvent, Category = "HUD|Minimap")
	void OnMinimapTextureReady(UTexture2D* Texture);

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
	 * Display event: a horde front starts in SecondsUntil seconds (banner "HORDE INCOMING!"). Its Kessler-Voss line is picked just
	 * before (GetAlertLine(horde_incoming)). Implement in Blueprint.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "HUD|Pressure")
	void OnHordeIncoming(float SecondsUntil);

	/**
	 * Display event: the alert-bar icons changed (also on binding): console, bonus chest (when its threshold is > 0), each horde in index order, shuttle; see
	 * FRRAlertBarEvent. The same list is kept for GetAlertBarEvents. Implement in Blueprint.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "HUD|Pressure")
	void OnAlertEventsChanged(const TArray<FRRAlertBarEvent>& Events);

	/** Native default of OnPlayerDamaged: nothing (the Blueprint draws the vignette). */
	virtual void OnPlayerDamaged_Implementation(float Amount, float HealthFraction) {}

	/** Native default of OnHordeIncoming: nothing (the Blueprint shows the banner). */
	virtual void OnHordeIncoming_Implementation(float SecondsUntil) {}

	/** Native default of OnAlertEventsChanged: nothing (the Blueprint places the icons). */
	virtual void OnAlertEventsChanged_Implementation(const TArray<FRRAlertBarEvent>& Events) {}

	/** Spawner OnHordeIncoming callback: calls OnHordeIncoming and refreshes the icons. */
	UFUNCTION()
	void HandleHordeIncoming(int32 HordeIndex, float SecondsUntil);

	/** Spawner OnHordeLull callback: refreshes the icons. */
	UFUNCTION()
	void HandleHordeLull(int32 HordeIndex);

	/** Spawner OnHordeStarted callback: freezes the horde icon at the current alert (done) and refreshes. */
	UFUNCTION()
	void HandleHordeStarted(int32 HordeIndex, ERRSpawnSide Side);

	/** Spawner OnHordeFinished callback: refreshes the icons. */
	UFUNCTION()
	void HandleHordeFinished(int32 HordeIndex, int32 Spawned);

	/** Computes the current alert-bar icons into OutEvents (see sys-hud for the position rule). */
	void ComputeAlertBarEvents(TArray<FRRAlertBarEvent>& OutEvents);

	/** Alert-bar icons last sent to OnAlertEventsChanged. */
	UPROPERTY(Transient)
	TArray<FRRAlertBarEvent> AlertBarEvents;

	/** True once OnAlertEventsChanged was called (the first refresh always calls it). */
	bool bAlertBarEventsSent = false;

	/** Bar position of each horde whose front started, frozen at the alert of that moment (key: horde index). */
	TMap<int32, float> DoneHordePositions;

	/** A pending horde icon moving by more than this (bar fraction) calls OnAlertEventsChanged again; smaller moves wait. */
	UPROPERTY(EditDefaultsOnly, Category = "HUD|Pressure", meta = (ClampMin = 0))
	float AlertEventPositionTolerance = 0.002f;

	/** Name of the damage vignette Image in the HUD Blueprint (WBP_HUD: DamageVignette); none found = no warm-up. */
	UPROPERTY(EditDefaultsOnly, Category = "HUD|Pressure")
	FName DamageVignetteWidgetName = TEXT("DamageVignette");

	/**
	 * Lowest render opacity of the damage vignette: above Slate's cull threshold (1e-4) so its material keeps rendering, small
	 * enough to stay invisible (under half an 8-bit step). 0 disables the warm-up.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "HUD|Pressure", meta = (ClampMin = 0, ClampMax = 0.01))
	float VignetteWarmOpacity = 0.001f;

	/** The damage vignette widget found by the first KeepVignetteWarm (null when the Blueprint has none). */
	TWeakObjectPtr<UWidget> DamageVignette;

	/** True once KeepVignetteWarm looked DamageVignetteWidgetName up (done once). */
	bool bDamageVignetteLookedUp = false;

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
	 * after a reroll (bCanReroll false) and for each chained screen. Implement in Blueprint.
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

	/** Display event: the run is over (the level restarts after the game mode's RestartDelay). Implement in Blueprint. */
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

	/** Run flow OnShuttleCountdown callback. */
	UFUNCTION()
	void HandleShuttleCountdown(float RemainingSeconds, FVector ShuttleLocation);

	/** Run flow OnRunEnded callback. */
	UFUNCTION()
	void HandleRunEnded(bool bSuccess);

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

	/** Run flow whose events are bound. */
	TWeakObjectPtr<URRRunFlowSubsystem> BoundRunFlow;

	/** Handle of the Health change binding on BoundASC. */
	FDelegateHandle HealthChangedHandle;

	/** Handle of the MaxHealth change binding on BoundASC. */
	FDelegateHandle MaxHealthChangedHandle;
};
