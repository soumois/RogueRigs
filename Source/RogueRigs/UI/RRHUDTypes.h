// Rogue Rigs - HUD data types shared by the HUD widgets and the gameplay systems that fire HUD events (locator target, alert-bar icons, weapon slot state).

#pragma once

#include "CoreMinimal.h"
#include "RRHUDTypes.generated.h"

class ARRWeapon;

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

	/**
	 * True when the lists differ for the HUD: count, type, index, done flag, or a position moved by more than PositionTolerance.
	 * Used by every alert-bar event source so smaller moves never re-fire the event.
	 */
	static bool ListsDiffer(const TArray<FRRAlertBarEvent>& A, const TArray<FRRAlertBarEvent>& B, float PositionTolerance);
};

/** HUD state of one weapon slot (index 0..MaxWeapons-1): read by GetWeaponSlot, sent by ARRMiner::OnWeaponSlotChanged. */
USTRUCT(BlueprintType)
struct ROGUERIGS_API FRRWeaponSlotHUD
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

	/** HUD state of Weapon (locked slot when null): magazine and reload, or charges and recharge for a turret launcher. */
	static FRRWeaponSlotHUD Make(const ARRWeapon* InWeapon);

	/** True when every field but the per-frame progress values (ReloadProgress, RechargeProgress) matches Other. */
	bool HasSameState(const FRRWeaponSlotHUD& Other) const;
};
