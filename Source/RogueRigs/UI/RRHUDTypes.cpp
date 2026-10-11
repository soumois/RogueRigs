// Rogue Rigs - HUD data types shared by the HUD widgets and the gameplay systems that fire HUD events (locator target, alert-bar icons, weapon slot state).

#include "UI/RRHUDTypes.h"

#include "AbilitySystemComponent.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRTurretLauncher.h"
#include "Weapons/RRWeapon.h"
#include "Weapons/RRWeaponAttributeSet.h"

bool FRRAlertBarEvent::ListsDiffer(const TArray<FRRAlertBarEvent>& A, const TArray<FRRAlertBarEvent>& B, float PositionTolerance)
{
	if (A.Num() != B.Num())
	{
		return true;
	}
	for (int32 Index = 0; Index < A.Num(); ++Index)
	{
		const FRRAlertBarEvent& New = A[Index];
		const FRRAlertBarEvent& Old = B[Index];
		if (New.Type != Old.Type || New.Index != Old.Index || New.bDone != Old.bDone || FMath::Abs(New.Position - Old.Position) > PositionTolerance)
		{
			return true;
		}
	}
	return false;
}

FRRWeaponSlotHUD FRRWeaponSlotHUD::Make(const ARRWeapon* InWeapon)
{
	FRRWeaponSlotHUD Slot;
	if (!IsValid(InWeapon))
	{
		return Slot;
	}
	Slot.bOwned = true;
	Slot.Weapon = const_cast<ARRWeapon*>(InWeapon);

	if (const ARRTurretLauncher* Launcher = Cast<ARRTurretLauncher>(InWeapon))
	{
		Slot.bIsTurretLauncher = true;
		Slot.Charges = Launcher->GetCharges();
		Slot.MaxCharges = Launcher->GetMaxCharges();
		Slot.Rounds = Slot.Charges;
		Slot.MagazineSize = Slot.MaxCharges;
		Slot.bRecharging = Launcher->IsRecharging();
		Slot.RechargeProgress = Launcher->GetRechargeProgress();
		return Slot;
	}

	if (const UAbilitySystemComponent* ASC = InWeapon->GetAbilitySystemComponent())
	{
		const float Ammo = ASC->GetNumericAttribute(URRWeaponAttributeSet::GetAmmoAttribute());
		const float Magazine = ASC->GetNumericAttribute(URRWeaponAttributeSet::GetMagazineSizeAttribute());
		Slot.Rounds = FMath::Max(FMath::FloorToInt32(Ammo + KINDA_SMALL_NUMBER), 0);
		Slot.MagazineSize = FMath::Max(FMath::FloorToInt32(Magazine + KINDA_SMALL_NUMBER), 0);
	}
	if (const URRAutoFireComponent* AutoFire = InWeapon->GetAutoFireComponent())
	{
		Slot.bReloading = AutoFire->IsReloading();
		const float Duration = AutoFire->GetReloadDuration();
		if (Slot.bReloading && Duration > 0.f)
		{
			Slot.ReloadProgress = FMath::Clamp(1.f - AutoFire->GetReloadTimeRemaining() / Duration, 0.f, 1.f);
		}
	}
	return Slot;
}

bool FRRWeaponSlotHUD::HasSameState(const FRRWeaponSlotHUD& Other) const
{
	return bOwned == Other.bOwned && Weapon == Other.Weapon && bIsTurretLauncher == Other.bIsTurretLauncher && Rounds == Other.Rounds
		&& MagazineSize == Other.MagazineSize && bReloading == Other.bReloading && Charges == Other.Charges && MaxCharges == Other.MaxCharges
		&& bRecharging == Other.bRecharging;
}
