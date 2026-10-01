// Rogue Rigs - auto-fire accumulator: decides when a weapon, turret or enemy shooter fires (burst timing, magazine, reload).

#include "Weapons/RRAutoFireComponent.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "RogueRigs.h"
#include "Weapons/RRGE_ConsumeAmmo.h"
#include "Weapons/RRGE_RefillAmmo.h"
#include "Weapons/RRWeaponAttributeSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRAutoFireComponent)

URRAutoFireComponent::URRAutoFireComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;

	ConsumeAmmoEffect = URRGE_ConsumeAmmo::StaticClass();
	RefillAmmoEffect = URRGE_RefillAmmo::StaticClass();
}

bool URRAutoFireComponent::Configure(UAbilitySystemComponent* InAbilitySystemComponent, int32 InBurstCount, float InBurstDuty, float InReloadTime)
{
	AbilitySystemComponent = InAbilitySystemComponent;
	BurstCount = FMath::Max(InBurstCount, 1);
	BurstDuty = FMath::Clamp(InBurstDuty, 0.f, 1.f);
	ReloadTime = FMath::Max(InReloadTime, 0.f);

	TimeToNextShot = 0.0;
	ReloadTimeRemaining = 0.0;
	ShotsFiredInBurst = 0;
	TotalShotsFired = 0;
	bReloading = false;

	if (!AbilitySystemComponent || !AbilitySystemComponent->HasAttributeSetForAttribute(URRWeaponAttributeSet::GetFireRateAttribute()))
	{
		UE_LOG(LogRogueRigs, Error, TEXT("%s: auto-fire needs an ASC with URRWeaponAttributeSet; it will not fire."), *GetNameSafe(GetOwner()));
		AbilitySystemComponent = nullptr;
		return false;
	}
	return true;
}

void URRAutoFireComponent::StartFiring()
{
	bFiring = true;
}

void URRAutoFireComponent::StopFiring()
{
	bFiring = false;
}

void URRAutoFireComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AdvanceTime(DeltaTime);
}

float URRAutoFireComponent::GetAttributeValue(const FGameplayAttribute& Attribute) const
{
	return AbilitySystemComponent ? AbilitySystemComponent->GetNumericAttribute(Attribute) : 0.f;
}

float URRAutoFireComponent::GetReloadDuration() const
{
	const float ReloadSpeed = GetAttributeValue(URRWeaponAttributeSet::GetReloadSpeedAttribute());
	return ReloadSpeed > 0.f ? ReloadTime / ReloadSpeed : 0.f;
}

void URRAutoFireComponent::AdvanceTime(float DeltaSeconds)
{
	if (!bFiring || !AbilitySystemComponent || DeltaSeconds <= 0.f)
	{
		return;
	}

	// Remaining time of this frame; may end a hair below 0 when an event fires inside the tolerance,
	// and that debt is carried into the next timer so no time is lost or gained.
	double Remaining = DeltaSeconds;

	for (int32 EventCount = 0; EventCount < MaxEventsPerAdvance; ++EventCount)
	{
		if (bReloading)
		{
			// ReloadSpeed <= 0 means the reload cannot progress.
			if (GetAttributeValue(URRWeaponAttributeSet::GetReloadSpeedAttribute()) <= 0.f
				|| ReloadTimeRemaining > Remaining + TimeTolerance)
			{
				ReloadTimeRemaining = FMath::Max(ReloadTimeRemaining - Remaining, 0.0);
				return;
			}
			Remaining -= ReloadTimeRemaining;
			FinishReload();
			continue;
		}

		const float FireRate = GetAttributeValue(URRWeaponAttributeSet::GetFireRateAttribute());
		if (FireRate <= 0.f)
		{
			// No fire rate: hold the current cooldown.
			return;
		}

		if (TimeToNextShot > Remaining + TimeTolerance)
		{
			TimeToNextShot -= Remaining;
			return;
		}

		Remaining -= TimeToNextShot;
		TimeToNextShot = 0.0;
		FireShot(FireRate);

		if (!bFiring || !AbilitySystemComponent)
		{
			// The shot handler stopped firing or the owner is going away.
			return;
		}
	}

	UE_LOG(LogRogueRigs, Warning, TEXT("%s: auto-fire hit MaxEventsPerAdvance (%d) in one frame; %.4f s dropped."),
		*GetNameSafe(GetOwner()), MaxEventsPerAdvance, Remaining);
}

void URRAutoFireComponent::FireShot(float FireRate)
{
	if (bUseMagazine && GetAttributeValue(URRWeaponAttributeSet::GetAmmoAttribute()) < 1.f)
	{
		StartReload();
		return;
	}

	const int32 ShotIndex = ShotsFiredInBurst;
	++ShotsFiredInBurst;
	++TotalShotsFired;

	ShotHandler.ExecuteIfBound();
	if (bUseMagazine)
	{
		ConsumeAmmo();
	}
	OnShotFired.Broadcast(ShotIndex);

	// Magazine empty (mid-burst or on the last bullet): the burst stops and the reload replaces the pause.
	if (bUseMagazine && GetAttributeValue(URRWeaponAttributeSet::GetAmmoAttribute()) < 1.f)
	{
		StartReload();
		return;
	}

	const double Period = 1.0 / FireRate;
	if (BurstCount <= 1)
	{
		ShotsFiredInBurst = 0;
		TimeToNextShot = Period;
	}
	else if (ShotsFiredInBurst >= BurstCount)
	{
		ShotsFiredInBurst = 0;
		TimeToNextShot = Period * (1.0 - BurstDuty);
	}
	else
	{
		TimeToNextShot = BurstDuty * Period / (BurstCount - 1);
	}
}

void URRAutoFireComponent::StartReload()
{
	ShotsFiredInBurst = 0;
	bReloading = true;
	const float Duration = GetReloadDuration();
	ReloadTimeRemaining = Duration;
	OnReloadStarted.Broadcast(Duration);
}

void URRAutoFireComponent::FinishReload()
{
	bReloading = false;
	ReloadTimeRemaining = 0.0;
	TimeToNextShot = 0.0;
	RefillAmmo();
	OnReloadFinished.Broadcast();
}

bool URRAutoFireComponent::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> EffectClass) const
{
	if (!EffectClass || !AbilitySystemComponent)
	{
		return false;
	}
	const UGameplayEffect* Effect = EffectClass->GetDefaultObject<UGameplayEffect>();
	AbilitySystemComponent->ApplyGameplayEffectToSelf(Effect, 1.f, AbilitySystemComponent->MakeEffectContext());
	return true;
}

void URRAutoFireComponent::ConsumeAmmo()
{
	if (!ApplyEffectToSelf(ConsumeAmmoEffect) && AbilitySystemComponent)
	{
		const float Ammo = AbilitySystemComponent->GetNumericAttributeBase(URRWeaponAttributeSet::GetAmmoAttribute());
		AbilitySystemComponent->SetNumericAttributeBase(URRWeaponAttributeSet::GetAmmoAttribute(), Ammo - 1.f);
	}
}

void URRAutoFireComponent::RefillAmmo()
{
	if (!AbilitySystemComponent)
	{
		return;
	}
	if (!ApplyEffectToSelf(RefillAmmoEffect))
	{
		AbilitySystemComponent->SetNumericAttributeBase(URRWeaponAttributeSet::GetAmmoAttribute(),
			GetAttributeValue(URRWeaponAttributeSet::GetMagazineSizeAttribute()));
	}
}
