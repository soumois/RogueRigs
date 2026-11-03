// Rogue Rigs - C++ base of the in-run pause menu (pause-menu gdd, task-424-03): miner stats snapshot (global and per weapon),
// Resume and Exit buttons, open event for the Blueprint.

#include "UI/RRPauseMenuWidget.h"

#include "AbilitySystemComponent.h"
#include "Engine/World.h"
#include "GAS/RRHealthAttributeSet.h"
#include "GAS/RRMovementAttributeSet.h"
#include "GAS/RRPlayerAttributeSet.h"
#include "Kismet/GameplayStatics.h"
#include "Objectives/RRRunFlowSubsystem.h"
#include "Player/RRMiner.h"
#include "Player/RRPlayerController.h"
#include "RogueRigs.h"
#include "Upgrades/RRUpgradeComponent.h"
#include "Weapons/RRWeapon.h"
#include "Weapons/RRWeaponAttributeSet.h"

namespace RRPauseMenu
{
	/** Number with one decimal at most and no trailing ".0" (readability-rules, same rounding as the card values). */
	static FString FormatNumber(double Number)
	{
		const double Rounded = FMath::RoundToDouble(Number * 10.0) / 10.0;
		return FMath::IsNearlyEqual(Rounded, FMath::RoundToDouble(Rounded))
			? FString::Printf(TEXT("%lld"), static_cast<long long>(FMath::RoundToDouble(Rounded)))
			: FString::Printf(TEXT("%.1f"), Rounded);
	}

	/** Builds a row with the stat's English label. */
	static FRRPauseMenuStatRow MakeRow(ERRUpgradeStat Stat, float Value, const FString& ValueText, bool bGlobalWeaponBonus = false)
	{
		FRRPauseMenuStatRow Row;
		Row.Stat = Stat;
		Row.Label = URRUpgradeComponent::GetStatDisplayName(Stat);
		Row.Value = Value;
		Row.ValueText = FText::FromString(ValueText);
		Row.bGlobalWeaponBonus = bGlobalWeaponBonus;
		return Row;
	}

	/** Current value of Attribute on ASC; 0 when the ASC lacks its set. */
	static float GetCurrent(const UAbilitySystemComponent* ASC, const FGameplayAttribute& Attribute)
	{
		return ASC && ASC->HasAttributeSetForAttribute(Attribute) ? ASC->GetNumericAttribute(Attribute) : 0.f;
	}

	/** Bonus of a percentage stat over its base value (current / base - 1; 0 when the base is 0 or the set is missing). */
	static float GetBonusOverBase(const UAbilitySystemComponent* ASC, const FGameplayAttribute& Attribute)
	{
		if (!ASC || !ASC->HasAttributeSetForAttribute(Attribute))
		{
			return 0.f;
		}
		const float Base = ASC->GetNumericAttributeBase(Attribute);
		return Base > KINDA_SMALL_NUMBER ? ASC->GetNumericAttribute(Attribute) / Base - 1.f : 0.f;
	}
}

URRPauseMenuWidget::URRPauseMenuWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsFocusable(true);
}

void URRPauseMenuWidget::ShowPauseMenu(const FRRPauseMenuStats& InStats)
{
	Stats = InStats;
	OnPauseMenuOpened(Stats);
}

bool URRPauseMenuWidget::Resume()
{
	ARRPlayerController* Controller = GetOwningPlayer<ARRPlayerController>();
	return Controller && Controller->ClosePauseMenu();
}

bool URRPauseMenuWidget::ExitToMainMenu()
{
	return ExitToMainMenuInWorld(GetWorld());
}

bool URRPauseMenuWidget::ExitToMainMenuInWorld(UWorld* World)
{
	ARRPlayerController* Controller = World ? Cast<ARRPlayerController>(UGameplayStatics::GetPlayerController(World, 0)) : nullptr;
	if (!Controller || !Controller->IsPauseMenuOpen())
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("Pause menu Exit refused (%s)."), Controller ? TEXT("the pause menu is not open") : TEXT("no run player controller"));
		return false;
	}
	URRRunFlowSubsystem* RunFlow = World->GetSubsystem<URRRunFlowSubsystem>();
	if (!RunFlow)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("Pause menu Exit refused (no run flow subsystem)."));
		return false;
	}
	Controller->ClosePauseMenu();
	UE_LOG(LogRogueRigs, Log, TEXT("Pause menu Exit: run reset, back to the main menu (run-won Main Menu path)."));
	return RunFlow->ReturnToMainMenu();
}

FRRPauseMenuStats URRPauseMenuWidget::BuildStatsSnapshot(const ARRMiner* Miner)
{
	using namespace RRPauseMenu;

	FRRPauseMenuStats Result;
	if (!Miner)
	{
		return Result;
	}

	// Miner stats (player-stats): max health and luck in points, the four percentage stats as a bonus over their base.
	const UAbilitySystemComponent* ASC = Miner->GetAbilitySystemComponent();
	const float MaxHealth = GetCurrent(ASC, URRHealthAttributeSet::GetMaxHealthAttribute());
	Result.GlobalRows.Add(MakeRow(ERRUpgradeStat::MaxHealth, MaxHealth, FormatNumber(MaxHealth)));
	const TPair<ERRUpgradeStat, FGameplayAttribute> PercentStats[] = {
		{ ERRUpgradeStat::MoveSpeed, URRMovementAttributeSet::GetMoveSpeedAttribute() },
		{ ERRUpgradeStat::PickupRadius, URRPlayerAttributeSet::GetPickupRadiusAttribute() },
		{ ERRUpgradeStat::XPGain, URRPlayerAttributeSet::GetXPGainAttribute() },
		{ ERRUpgradeStat::MiningSpeed, URRPlayerAttributeSet::GetMiningSpeedAttribute() },
	};
	for (const TPair<ERRUpgradeStat, FGameplayAttribute>& Stat : PercentStats)
	{
		const float Bonus = GetBonusOverBase(ASC, Stat.Value);
		Result.GlobalRows.Add(MakeRow(Stat.Key, Bonus, URRUpgradeComponent::FormatValueText(Stat.Key, Bonus).ToString()));
	}
	const float Luck = GetCurrent(ASC, URRPlayerAttributeSet::GetLuckAttribute());
	Result.GlobalRows.Add(MakeRow(ERRUpgradeStat::Luck, Luck, FormatNumber(Luck)));

	// Global weapon bonuses (player-stats: the sum of the global bonuses taken): every "All weapons" card picked, in attribute units.
	const ERRUpgradeStat GlobalWeaponStats[] = {
		ERRUpgradeStat::Damage, ERRUpgradeStat::FireRate, ERRUpgradeStat::ReloadSpeed, ERRUpgradeStat::CritChance, ERRUpgradeStat::CritDamage
	};
	const URRUpgradeComponent* Upgrades = Miner->GetUpgrades();
	for (const ERRUpgradeStat Stat : GlobalWeaponStats)
	{
		float Sum = 0.f;
		if (Upgrades)
		{
			for (const FRRUpgradeCard& Card : Upgrades->GetPickedUpgrades())
			{
				if (!Card.IsNewWeapon() && Card.Target == ERRUpgradeTarget::AllWeapons && Card.Stat == Stat)
				{
					Sum += Card.Value;
				}
			}
		}
		Result.GlobalRows.Add(MakeRow(Stat, Sum, URRUpgradeComponent::FormatValueText(Stat, Sum).ToString(), true));
	}

	// One block per owned weapon: current values as the weapon uses them.
	for (const TObjectPtr<ARRWeapon>& Weapon : Miner->GetWeapons())
	{
		if (!IsValid(Weapon))
		{
			continue;
		}
		const UAbilitySystemComponent* WeaponASC = Weapon->GetAbilitySystemComponent();
		FRRPauseMenuWeaponBlock& Block = Result.Weapons.AddDefaulted_GetRef();
		Block.WeaponName = Weapon->GetDisplayName();
		Block.WeaponClass = Weapon->GetClass();

		const float Damage = GetCurrent(WeaponASC, URRWeaponAttributeSet::GetDamageAttribute());
		const float FireRate = GetCurrent(WeaponASC, URRWeaponAttributeSet::GetFireRateAttribute());
		const float ReloadSpeed = GetCurrent(WeaponASC, URRWeaponAttributeSet::GetReloadSpeedAttribute());
		const float CritChance = GetCurrent(WeaponASC, URRWeaponAttributeSet::GetCritChanceAttribute());
		const float CritDamage = GetCurrent(WeaponASC, URRWeaponAttributeSet::GetCritDamageAttribute());
		const int32 Magazine = Weapon->GetMagazineRounds();
		const int32 Pierce = Weapon->GetPierceCount();
		Block.Rows.Add(MakeRow(ERRUpgradeStat::Damage, Damage, FormatNumber(Damage)));
		Block.Rows.Add(MakeRow(ERRUpgradeStat::FireRate, FireRate, FormatNumber(FireRate) + TEXT("/s")));
		Block.Rows.Add(MakeRow(ERRUpgradeStat::ReloadSpeed, ReloadSpeed, TEXT("x") + FormatNumber(ReloadSpeed)));
		Block.Rows.Add(MakeRow(ERRUpgradeStat::CritChance, CritChance, FormatNumber(CritChance * 100.0) + TEXT("%")));
		Block.Rows.Add(MakeRow(ERRUpgradeStat::CritDamage, CritDamage, TEXT("x") + FormatNumber(CritDamage)));
		Block.Rows.Add(MakeRow(ERRUpgradeStat::MagazineSize, static_cast<float>(Magazine), FString::FromInt(Magazine)));
		Block.Rows.Add(MakeRow(ERRUpgradeStat::Pierce, static_cast<float>(Pierce), FString::FromInt(Pierce)));
	}
	return Result;
}

void URRPauseMenuWidget::LogStats(const FRRPauseMenuStats& InStats, const TCHAR* Context)
{
	FString Global;
	for (const FRRPauseMenuStatRow& Row : InStats.GlobalRows)
	{
		Global += FString::Printf(TEXT("%s%s %s"), Global.IsEmpty() ? TEXT("") : TEXT(", "), *Row.Label.ToString(), *Row.ValueText.ToString());
	}
	UE_LOG(LogRogueRigs, Log, TEXT("%s: pause menu stats, global: %s; %d weapon block(s)."), Context, Global.IsEmpty() ? TEXT("none") : *Global,
		InStats.Weapons.Num());
	for (const FRRPauseMenuWeaponBlock& Block : InStats.Weapons)
	{
		FString Rows;
		for (const FRRPauseMenuStatRow& Row : Block.Rows)
		{
			Rows += FString::Printf(TEXT("%s%s %s"), Rows.IsEmpty() ? TEXT("") : TEXT(", "), *Row.Label.ToString(), *Row.ValueText.ToString());
		}
		UE_LOG(LogRogueRigs, Log, TEXT("%s: pause menu weapon %s: %s."), Context, *Block.WeaponName.ToString(), *Rows);
	}
}
