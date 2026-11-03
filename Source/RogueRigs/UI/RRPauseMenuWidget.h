// Rogue Rigs - C++ base of the in-run pause menu (pause-menu gdd, task-424-03): miner stats snapshot (global and per weapon),
// Resume and Exit buttons, open event for the Blueprint.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Templates/SubclassOf.h"
// RRUpgradeCard.h converts TSubclassOf<ARRWeapon> inline (IsSameChoice): the complete type is needed by every includer.
#include "Upgrades/RRUpgradeCard.h"
#include "Weapons/RRWeapon.h"
#include "RRPauseMenuWidget.generated.h"

class ARRMiner;

/** One line of the pause menu stats: a stat, its English label and its value formatted for the screen. */
USTRUCT(BlueprintType)
struct ROGUERIGS_API FRRPauseMenuStatRow
{
	GENERATED_BODY()

	/** Stat shown on this line (icon lookup: URRIconLibrary::GetStatIcon). */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
	ERRUpgradeStat Stat = ERRUpgradeStat::Damage;

	/** English stat name (URRUpgradeComponent::GetStatDisplayName); the widget may write its own label instead. */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
	FText Label;

	/** Value ready for the screen ("120", "+12%", "1.5/s", "x1.5"; format per stat in URRPauseMenuWidget::BuildStatsSnapshot). */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
	FText ValueText;

	/** Raw number behind ValueText (attribute units: 0.12 = +12 %, points, multiplier or count). */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
	float Value = 0.f;

	/** True for the five global weapon bonus lines of the global block (false for the six miner stats and the weapon blocks). */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
	bool bGlobalWeaponBonus = false;
};

/** Stats block of one owned weapon on the pause menu. */
USTRUCT(BlueprintType)
struct ROGUERIGS_API FRRPauseMenuWeaponBlock
{
	GENERATED_BODY()

	/** Weapon name shown on the block (ARRWeapon::GetDisplayName, CSV column DisplayName). */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
	FText WeaponName;

	/** Exact class of the weapon (icon lookup: URRIconLibrary::GetWeaponIconByClass). */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
	TSubclassOf<ARRWeapon> WeaponClass;

	/**
	 * Current values of the weapon, in this order: Damage, Fire rate (bursts per second), Reload speed (multiplier), Crit chance (%),
	 * Crit damage (multiplier), Magazine size, Piercing.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
	TArray<FRRPauseMenuStatRow> Rows;
};

/** Everything the pause menu shows, read from the miner when the menu opens. */
USTRUCT(BlueprintType)
struct ROGUERIGS_API FRRPauseMenuStats
{
	GENERATED_BODY()

	/**
	 * Global block: the six miner stats (Max health in points; Move speed, Pickup radius, XP gain, Mining speed as a bonus % over the
	 * base value; Luck in points), then the five global weapon bonuses taken (sum of the "All weapons" cards picked: Damage, Fire rate,
	 * Reload speed in %, Crit chance and Crit damage in points shown as %), bGlobalWeaponBonus true on those five.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
	TArray<FRRPauseMenuStatRow> GlobalRows;

	/** One block per owned weapon, in acquisition order (slot order of the HUD); locked slots are not listed. */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
	TArray<FRRPauseMenuWeaponBlock> Weapons;
};

/**
 * In-run pause menu (pause-menu gdd). Created by ARRPlayerController::OpenPauseMenu (Escape, P, gamepad Start, or
 * rr.Debug.PauseMenu open) from PauseMenuWidgetClass (default /Game/RogueRigs/UI/WBP_PauseMenu), game paused, cursor shown, and
 * removed by ClosePauseMenu. Layout, labels and look live in the Blueprint subclass: implement OnPauseMenuOpened (stats
 * snapshot taken at opening; GetStatsSnapshot reads it again), bind the Resume button to Resume and the Exit button to
 * ExitToMainMenu. Keyboard focus: the controller focuses the child named by ResumeButtonName when it exists. No gameplay logic here.
 */
UCLASS(Abstract)
class ROGUERIGS_API URRPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Focusable by default, so the controller can give it keyboard focus. */
	URRPauseMenuWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Resume button: closes the menu and unpauses (ARRPlayerController::ClosePauseMenu). False when the menu was not open. */
	UFUNCTION(BlueprintCallable, Category = "Pause Menu")
	bool Resume();

	/**
	 * Exit button: no confirmation; the run-won "Main Menu" path (URRRunFlowSubsystem::ReturnToMainMenu: run reset, ore back to 0,
	 * L_MainMenu opened directly, no loading screen, no fade). False (logged) when refused.
	 */
	UFUNCTION(BlueprintCallable, Category = "Pause Menu")
	bool ExitToMainMenu();

	/** Stats snapshot taken when the menu opened (empty without a miner). */
	UFUNCTION(BlueprintPure, Category = "Pause Menu")
	FRRPauseMenuStats GetStatsSnapshot() const { return Stats; }

	/** Called by the controller after AddToViewport: stores the snapshot and fires OnPauseMenuOpened. */
	void ShowPauseMenu(const FRRPauseMenuStats& InStats);

	/** Name of the child button that gets keyboard focus at opening (none found: the widget itself is focused). */
	FName GetResumeButtonName() const { return ResumeButtonName; }

	/**
	 * Reads the pause menu stats of Miner (pause-menu gdd content): miner attributes on its ASC, the "All weapons" cards of its
	 * URRUpgradeComponent::GetPickedUpgrades, and each valid weapon of GetWeapons. Empty without a miner. Shared with rr.Debug.PauseMenu.
	 */
	static FRRPauseMenuStats BuildStatsSnapshot(const ARRMiner* Miner);

	/**
	 * Exit shared by the button and rr.Debug.PauseMenu exit: refused (false, logged) unless player 0's ARRPlayerController has the
	 * pause menu open; else the menu is closed and URRRunFlowSubsystem::ReturnToMainMenu runs.
	 */
	static bool ExitToMainMenuInWorld(UWorld* World);

	/** Writes every line of Stats to the log (rr.Debug.PauseMenu open, capture checks). */
	static void LogStats(const FRRPauseMenuStats& InStats, const TCHAR* Context);

protected:
	/** Show the screen from the snapshot (global block, then one block per weapon). Implement in the Blueprint subclass. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Pause Menu")
	void OnPauseMenuOpened(const FRRPauseMenuStats& InStats);

	/** Child widget (button) focused at opening, keyboard and gamepad (pause-menu gdd: focus on Resume). */
	UPROPERTY(EditDefaultsOnly, Category = "Pause Menu")
	FName ResumeButtonName = TEXT("ResumeButton");

	/** Snapshot taken at opening. */
	UPROPERTY(Transient)
	FRRPauseMenuStats Stats;
};
