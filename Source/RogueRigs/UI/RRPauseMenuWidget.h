// Rogue Rigs - C++ base of the in-run pause menu (WBP_PauseMenu, task-424-03): stats snapshot of the miner (own stats, summed
// global weapon bonuses, one block per owned weapon) and the Resume and Exit buttons.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RRPauseMenuWidget.generated.h"

class ARRMiner;
class ARRPlayerController;
class ARRWeapon;
class UButton;

/** Current stats of one owned weapon for the pause menu (pause-menu gdd, Content): numbers and the texts drawn by the widget. */
USTRUCT(BlueprintType)
struct ROGUERIGS_API FRRPauseMenuWeaponStats
{
	GENERATED_BODY()

	/** The weapon these stats were read from. */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
	TObjectPtr<ARRWeapon> Weapon;

	/** Block title: the weapon's CSV DisplayName (ARRWeapon::GetDisplayName, e.g. "Burst rifle"). */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
	FText Name;

	/** True for the turret launcher (MagazineSize = turret charges, reload = one charge's recharge). */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
	bool bIsTurretLauncher = false;

	/** Current Damage attribute (per bullet or pellet, before critical hits). */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
	float Damage = 0.f;

	/** Current FireRate attribute (bursts per second). */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
	float FireRate = 0.f;

	/** Current ReloadSpeed attribute (multiplier, 1 = base). */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
	float ReloadSpeed = 0.f;

	/** Reload duration in seconds: CSV ReloadTime / ReloadSpeed (0 when ReloadSpeed is 0). */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
	float ReloadSeconds = 0.f;

	/** Current CritChance attribute (0..1). */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
	float CritChance = 0.f;

	/** Current CritDamage attribute (multiplier, 1.5 = 150 %). */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
	float CritDamage = 0.f;

	/** Current MagazineSize attribute, rounded down. */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
	int32 MagazineSize = 0;

	/** Current Pierce attribute, rounded down (robots one projectile damages). */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
	int32 Pierce = 0;

	/** Damage as drawn: "12" (one decimal only when not whole). */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
	FText DamageText;

	/** Fire rate as drawn: "3.0". */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
	FText FireRateText;

	/** Reload duration as drawn: "1.8s". */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
	FText ReloadText;

	/** Critical chance as drawn: "5%". */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
	FText CritChanceText;

	/** Critical damage as drawn: "150%". */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
	FText CritDamageText;

	/** Magazine as drawn: "30". */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
	FText MagazineText;

	/** Pierce as drawn: "4". */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
	FText PierceText;
};

/**
 * Stats snapshot shown by the pause menu (pause-menu gdd, Content; player-stats gdd): the miner's six own stats, the sum of the
 * global ("all weapons") weapon bonuses picked this run, and one entry per owned weapon (locked slots absent). Bonuses are
 * fractions (0.12 = +12 %); the *Text fields are ready to draw (invariant culture, the validated mockup's format).
 */
USTRUCT(BlueprintType)
struct ROGUERIGS_API FRRPauseMenuStats
{
	GENERATED_BODY()

	/** False when no miner was found (every value at its default). */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
	bool bHasMiner = false;

	/** Current MaxHealth (points). */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
	float MaxHealth = 0.f;

	/** MoveSpeed bonus over its base value (current / base - 1). */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
	float MoveSpeedBonus = 0.f;

	/** PickupRadius bonus over its base value. */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
	float PickupRadiusBonus = 0.f;

	/** XPGain bonus over its base value. */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
	float XPGainBonus = 0.f;

	/** MiningSpeed bonus over its base value. */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
	float MiningSpeedBonus = 0.f;

	/** Current Luck (points). */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
	float Luck = 0.f;

	/** Sum of the all-weapons Damage upgrades picked (fraction). */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
	float GlobalDamageBonus = 0.f;

	/** Sum of the all-weapons FireRate upgrades picked (fraction). */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
	float GlobalFireRateBonus = 0.f;

	/** Sum of the all-weapons ReloadSpeed upgrades picked (fraction). */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
	float GlobalReloadSpeedBonus = 0.f;

	/** Sum of the all-weapons CritChance upgrades picked (attribute units, 0.02 = 2 points). */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
	float GlobalCritChanceBonus = 0.f;

	/** Sum of the all-weapons CritDamage upgrades picked (attribute units, 0.1 = 10 points). */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
	float GlobalCritDamageBonus = 0.f;

	/** Max health as drawn: "100". */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
	FText MaxHealthText;

	/** Move speed bonus as drawn: "+0%". */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
	FText MoveSpeedText;

	/** Pickup radius bonus as drawn: "+0%". */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
	FText PickupRadiusText;

	/** XP gain bonus as drawn: "+0%". */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
	FText XPGainText;

	/** Mining speed bonus as drawn: "+0%". */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
	FText MiningSpeedText;

	/** Luck as drawn: "0". */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
	FText LuckText;

	/** Global damage bonus as drawn: "+10%". */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
	FText GlobalDamageText;

	/** Global fire rate bonus as drawn: "+5%". */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
	FText GlobalFireRateText;

	/** Global reload speed bonus as drawn: "+0%". */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
	FText GlobalReloadSpeedText;

	/** Global critical chance bonus in points as drawn: "+2". */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
	FText GlobalCritChanceText;

	/** Global critical damage bonus in points as drawn: "+10". */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
	FText GlobalCritDamageText;

	/** One entry per owned weapon, acquisition order (slot 0 = burst rifle), at most the miner's MaxWeapons. */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
	TArray<FRRPauseMenuWeaponStats> Weapons;
};

/**
 * In-run pause menu (pause-menu gdd; task-424). Created by ARRPlayerController (PauseMenuWidgetClass) when the pause opens, game
 * paused, and removed on Resume. Layout (title PAUSED, English labels, frames) lives in the Blueprint subclass WBP_PauseMenu:
 * optional bound buttons ResumeButton and ExitButton are wired here (focus on ResumeButton at open); implement OnStatsRefreshed
 * to draw GetStats (called once when the menu opens). The pause keys (ARRPlayerController PauseMenuKeys) still reach the
 * game input while the widget has focus (Game and UI mode, the action triggers when paused) and close the menu. Resume and Exit call the controller, the same functions as rr.Debug.PauseMenu.
 */
UCLASS(Abstract)
class ROGUERIGS_API URRPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Resume button: ARRPlayerController::ClosePauseMenu (menu removed, game unpaused). False when the menu was not open. */
	UFUNCTION(BlueprintCallable, Category = "Pause Menu")
	bool Resume();

	/** Exit button: ARRPlayerController::ExitPauseMenuToMainMenu (run reset, L_MainMenu opened, no confirmation). False when refused. */
	UFUNCTION(BlueprintCallable, Category = "Pause Menu")
	bool Exit();

	/** Snapshot taken by the last RefreshStats (when the menu opened). */
	UFUNCTION(BlueprintPure, Category = "Pause Menu")
	const FRRPauseMenuStats& GetStats() const { return Stats; }

	/** Reads the owning player's miner again (BuildStats), stores it and calls OnStatsRefreshed. */
	UFUNCTION(BlueprintCallable, Category = "Pause Menu")
	void RefreshStats();

	/** Gives keyboard and gamepad focus to ResumeButton (or to the widget itself without one). */
	UFUNCTION(BlueprintCallable, Category = "Pause Menu")
	void FocusResume();

	/** Widget to focus in the Game and UI input mode: ResumeButton when bound, else this widget. */
	UWidget* GetResumeFocusWidget();

	/** Builds the snapshot of Miner (see FRRPauseMenuStats); bHasMiner false for a null miner. Shared with rr.Debug.PauseMenu. */
	static FRRPauseMenuStats BuildStats(const ARRMiner* Miner);

	/** One line summary of Stats for the log (miner stats, global bonuses, each weapon). */
	static FString DescribeStats(const FRRPauseMenuStats& Stats);

protected:
	/** Makes the widget focusable and binds ResumeButton / ExitButton clicks. */
	virtual void NativeOnInitialized() override;


	/** Draw GetStats: called after each RefreshStats (once when the menu opens). Implement in Blueprint. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Pause Menu")
	void OnStatsRefreshed(const FRRPauseMenuStats& NewStats);

	/** ResumeButton click: Resume. */
	UFUNCTION()
	void HandleResumeClicked();

	/** ExitButton click: Exit. */
	UFUNCTION()
	void HandleExitClicked();

	/** The owning player controller as a run controller, or null. */
	ARRPlayerController* GetRunController() const;

	/** Optional Resume button of the Blueprint layout (name ResumeButton): click calls Resume, focused at open. */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu", meta = (BindWidgetOptional))
	TObjectPtr<UButton> ResumeButton;

	/** Optional Exit button of the Blueprint layout (name ExitButton): click calls Exit. */
	UPROPERTY(BlueprintReadOnly, Category = "Pause Menu", meta = (BindWidgetOptional))
	TObjectPtr<UButton> ExitButton;

	/** Snapshot of the last RefreshStats. */
	UPROPERTY(Transient)
	FRRPauseMenuStats Stats;
};
