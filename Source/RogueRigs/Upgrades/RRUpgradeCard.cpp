// Rogue Rigs - upgrade card: comparison of two cards. Kept out of the header: comparing TSubclassOf<ARRWeapon> needs the complete ARRWeapon type.

#include "Upgrades/RRUpgradeCard.h"

#include "Weapons/RRWeapon.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRUpgradeCard)

bool FRRUpgradeCard::IsSameChoice(const FRRUpgradeCard& Other) const
{
	if (CardType != Other.CardType)
	{
		return false;
	}
	if (IsNewWeapon())
	{
		return WeaponClass == Other.WeaponClass;
	}
	return Stat == Other.Stat && Target == Other.Target && TargetWeapon == Other.TargetWeapon;
}
