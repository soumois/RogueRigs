// Rogue Rigs - C++ base of the in-run pause menu (WBP_PauseMenu, task-424-03): stats snapshot of the miner (own stats, summed
// global weapon bonuses, one block per owned weapon) and the Resume and Exit buttons.

#include "UI/RRPauseMenuWidget.h"

#include "AbilitySystemComponent.h"
#include "Components/Button.h"
#include "GAS/RRHealthAttributeSet.h"
#include "GAS/RRMovementAttributeSet.h"
#include "GAS/RRPlayerAttributeSet.h"
#include "Internationalization/Internationalization.h"
#include "Player/RRMiner.h"
#include "UI/RRButtonHover.h"
#include "Player/RRPlayerController.h"
#include "Upgrades/RRUpgradeComponent.h"
#include "Weapons/RRTurretLauncher.h"
#include "Weapons/RRWeapon.h"
#include "Weapons/RRWeaponAttributeSet.h"

namespace RRPauseMenuFormat
{
	/** Value with up to MaxFractionalDigits decimals (at least MinFractionalDigits), invariant culture, no grouping. */
	static FString Number(float Value, int32 MinFractionalDigits, int32 MaxFractionalDigits)
	{
		FNumberFormattingOptions Options;
		Options.SetUseGrouping(false);
		Options.SetMinimumFractionalDigits(MinFractionalDigits);
		Options.SetMaximumFractionalDigits(MaxFractionalDigits);
		Options.SetRoundingMode(ERoundingMode::HalfFromZero);
		return FText::AsNumber(Value, &Options, FInternationalization::Get().GetInvariantCulture()).ToString();
	}

	/** "+12" / "-3" (sign always shown). */
	static FString Signed(float Value, int32 MaxFractionalDigits)
	{
		const FString Body = Number(FMath::Abs(Value), 0, MaxFractionalDigits);
		return (Value < 0.f && Body != TEXT("0") ? TEXT("-") : TEXT("+")) + Body;
	}

	/** Fraction as a signed percentage: 0.12 -> "+12%". */
	static FText SignedPercent(float Fraction)
	{
		return FText::FromString(Signed(Fraction * 100.f, 1) + TEXT("%"));
	}

	/** Fraction as signed points: 0.02 -> "+2". */
	static FText SignedPoints(float Fraction)
	{
		return FText::FromString(Signed(Fraction * 100.f, 1));
	}

	/** Fraction as a percentage: 0.05 -> "5%". */
	static FText Percent(float Fraction)
	{
		return FText::FromString(Number(Fraction * 100.f, 0, 1) + TEXT("%"));
	}

	/** Plain number with up to one decimal: "12", "10.5". */
	static FText Plain(float Value)
	{
		return FText::FromString(Number(Value, 0, 1));
	}

	/** Current / base - 1 of Attribute on ASC; 0 when the base is not above 0. */
	static float BonusOverBase(const UAbilitySystemComponent& ASC, const FGameplayAttribute& Attribute)
	{
		const float Base = ASC.GetNumericAttributeBase(Attribute);
		return Base > UE_KINDA_SMALL_NUMBER ? ASC.GetNumericAttribute(Attribute) / Base - 1.f : 0.f;
	}
}

FRRPauseMenuStats URRPauseMenuWidget::BuildStats(const ARRMiner* Miner)
{
	using namespace RRPauseMenuFormat;

	FRRPauseMenuStats Result;
	const UAbilitySystemComponent* ASC = Miner ? Miner->GetAbilitySystemComponent() : nullptr;
	Result.bHasMiner = ASC != nullptr;
	if (ASC)
	{
		Result.MaxHealth = ASC->GetNumericAttribute(URRHealthAttributeSet::GetMaxHealthAttribute());
		Result.MoveSpeedBonus = BonusOverBase(*ASC, URRMovementAttributeSet::GetMoveSpeedAttribute());
		Result.PickupRadiusBonus = BonusOverBase(*ASC, URRPlayerAttributeSet::GetPickupRadiusAttribute());
		Result.XPGainBonus = BonusOverBase(*ASC, URRPlayerAttributeSet::GetXPGainAttribute());
		Result.MiningSpeedBonus = BonusOverBase(*ASC, URRPlayerAttributeSet::GetMiningSpeedAttribute());
		Result.Luck = ASC->GetNumericAttribute(URRPlayerAttributeSet::GetLuckAttribute());
	}

	// Global weapon bonuses (player-stats gdd): the sum of the all-weapons cards picked this run, level-ups and shop alike.
	if (const URRUpgradeComponent* Upgrades = Miner ? Miner->GetUpgrades() : nullptr)
	{
		for (const FRRUpgradeCard& Card : Upgrades->GetPickedUpgrades())
		{
			if (Card.CardType != ERRUpgradeCardType::StatUpgrade || Card.Target != ERRUpgradeTarget::AllWeapons)
			{
				continue;
			}
			switch (Card.Stat)
			{
			case ERRUpgradeStat::Damage:      Result.GlobalDamageBonus += Card.Value; break;
			case ERRUpgradeStat::FireRate:    Result.GlobalFireRateBonus += Card.Value; break;
			case ERRUpgradeStat::ReloadSpeed: Result.GlobalReloadSpeedBonus += Card.Value; break;
			case ERRUpgradeStat::CritChance:  Result.GlobalCritChanceBonus += Card.Value; break;
			case ERRUpgradeStat::CritDamage:  Result.GlobalCritDamageBonus += Card.Value; break;
			default: break;
			}
		}
	}

	Result.MaxHealthText = Plain(Result.MaxHealth);
	Result.MoveSpeedText = SignedPercent(Result.MoveSpeedBonus);
	Result.PickupRadiusText = SignedPercent(Result.PickupRadiusBonus);
	Result.XPGainText = SignedPercent(Result.XPGainBonus);
	Result.MiningSpeedText = SignedPercent(Result.MiningSpeedBonus);
	Result.LuckText = Plain(Result.Luck);
	Result.GlobalDamageText = SignedPercent(Result.GlobalDamageBonus);
	Result.GlobalFireRateText = SignedPercent(Result.GlobalFireRateBonus);
	Result.GlobalReloadSpeedText = SignedPercent(Result.GlobalReloadSpeedBonus);
	Result.GlobalCritChanceText = SignedPoints(Result.GlobalCritChanceBonus);
	Result.GlobalCritDamageText = SignedPoints(Result.GlobalCritDamageBonus);

	if (!Miner)
	{
		return Result;
	}
	// Owned weapons in acquisition order; locked slots are not listed (pause-menu gdd).
	for (const TObjectPtr<ARRWeapon>& WeaponPtr : Miner->GetWeapons())
	{
		ARRWeapon* Weapon = WeaponPtr.Get();
		const UAbilitySystemComponent* WeaponASC = IsValid(Weapon) ? Weapon->GetAbilitySystemComponent() : nullptr;
		if (!WeaponASC || Result.Weapons.Num() >= Miner->GetMaxWeapons())
		{
			continue;
		}
		FRRPauseMenuWeaponStats& Entry = Result.Weapons.AddDefaulted_GetRef();
		Entry.Weapon = Weapon;
		Entry.Name = Weapon->GetDisplayName();
		Entry.bIsTurretLauncher = Weapon->IsA<ARRTurretLauncher>();
		Entry.Damage = WeaponASC->GetNumericAttribute(URRWeaponAttributeSet::GetDamageAttribute());
		Entry.FireRate = WeaponASC->GetNumericAttribute(URRWeaponAttributeSet::GetFireRateAttribute());
		Entry.ReloadSpeed = WeaponASC->GetNumericAttribute(URRWeaponAttributeSet::GetReloadSpeedAttribute());
		Entry.ReloadSeconds = Entry.ReloadSpeed > UE_KINDA_SMALL_NUMBER ? Weapon->GetReloadTime() / Entry.ReloadSpeed : 0.f;
		Entry.CritChance = WeaponASC->GetNumericAttribute(URRWeaponAttributeSet::GetCritChanceAttribute());
		Entry.CritDamage = WeaponASC->GetNumericAttribute(URRWeaponAttributeSet::GetCritDamageAttribute());
		Entry.MagazineSize = FMath::FloorToInt32(WeaponASC->GetNumericAttribute(URRWeaponAttributeSet::GetMagazineSizeAttribute()));
		Entry.Pierce = FMath::FloorToInt32(WeaponASC->GetNumericAttribute(URRWeaponAttributeSet::GetPierceAttribute()));

		Entry.DamageText = Plain(Entry.Damage);
		Entry.FireRateText = FText::FromString(Number(Entry.FireRate, 1, 1));
		Entry.ReloadText = FText::FromString(Number(Entry.ReloadSeconds, 1, 1) + TEXT("s"));
		Entry.CritChanceText = Percent(Entry.CritChance);
		Entry.CritDamageText = Percent(Entry.CritDamage);
		Entry.MagazineText = FText::AsNumber(Entry.MagazineSize, nullptr, FInternationalization::Get().GetInvariantCulture());
		Entry.PierceText = FText::AsNumber(Entry.Pierce, nullptr, FInternationalization::Get().GetInvariantCulture());
	}
	return Result;
}

FString URRPauseMenuWidget::DescribeStats(const FRRPauseMenuStats& InStats)
{
	if (!InStats.bHasMiner)
	{
		return TEXT("no miner");
	}
	FString Line = FString::Printf(TEXT("max health %s, move speed %s, pickup radius %s, XP gain %s, mining speed %s, luck %s; "
		"weapon bonuses damage %s, fire rate %s, reload speed %s, crit chance %s, crit damage %s; %d weapons"),
		*InStats.MaxHealthText.ToString(), *InStats.MoveSpeedText.ToString(), *InStats.PickupRadiusText.ToString(),
		*InStats.XPGainText.ToString(), *InStats.MiningSpeedText.ToString(), *InStats.LuckText.ToString(),
		*InStats.GlobalDamageText.ToString(), *InStats.GlobalFireRateText.ToString(), *InStats.GlobalReloadSpeedText.ToString(),
		*InStats.GlobalCritChanceText.ToString(), *InStats.GlobalCritDamageText.ToString(), InStats.Weapons.Num());
	for (const FRRPauseMenuWeaponStats& Weapon : InStats.Weapons)
	{
		Line += FString::Printf(TEXT(" | %s: damage %s, fire rate %s, reload %s, crit chance %s, crit damage %s, magazine %s, pierce %s"),
			*Weapon.Name.ToString(), *Weapon.DamageText.ToString(), *Weapon.FireRateText.ToString(), *Weapon.ReloadText.ToString(),
			*Weapon.CritChanceText.ToString(), *Weapon.CritDamageText.ToString(), *Weapon.MagazineText.ToString(),
			*Weapon.PierceText.ToString());
	}
	return Line;
}

void URRPauseMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	SetIsFocusable(true);
	if (ResumeButton)
	{
		ResumeButton->OnClicked.AddUniqueDynamic(this, &URRPauseMenuWidget::HandleResumeClicked);
	}
	if (ExitButton)
	{
		ExitButton->OnClicked.AddUniqueDynamic(this, &URRPauseMenuWidget::HandleExitClicked);
	}
	// Mouse hover of both plates (ui-button-hover, task-485-01).
	URRButtonHoverSubsystem::Attach(ResumeButton);
	URRButtonHoverSubsystem::Attach(ExitButton);
}

void URRPauseMenuWidget::RefreshStats()
{
	const APlayerController* Controller = GetOwningPlayer();
	Stats = BuildStats(Controller ? Controller->GetPawn<ARRMiner>() : nullptr);
	OnStatsRefreshed(Stats);
}

UWidget* URRPauseMenuWidget::GetResumeFocusWidget()
{
	return ResumeButton ? static_cast<UWidget*>(ResumeButton) : static_cast<UWidget*>(this);
}

void URRPauseMenuWidget::FocusResume()
{
	GetResumeFocusWidget()->SetFocus();
}

bool URRPauseMenuWidget::Resume()
{
	ARRPlayerController* Controller = GetRunController();
	return Controller && Controller->ClosePauseMenu(TEXT("Resume button"));
}

bool URRPauseMenuWidget::Exit()
{
	ARRPlayerController* Controller = GetRunController();
	return Controller && Controller->ExitPauseMenuToMainMenu(TEXT("Exit button"));
}

void URRPauseMenuWidget::HandleResumeClicked()
{
	Resume();
}

void URRPauseMenuWidget::HandleExitClicked()
{
	Exit();
}

ARRPlayerController* URRPauseMenuWidget::GetRunController() const
{
	return Cast<ARRPlayerController>(GetOwningPlayer());
}
