// Rogue Rigs - icon lookups for the UI.

#include "UI/RRIconLibrary.h"

#include "Engine/Texture2D.h"
#include "Misc/PackageName.h"
#include "UI/RRIconSettings.h"
#include "Weapons/RRWeapon.h"

UTexture2D* URRIconLibrary::GetStatIcon(ERRUpgradeStat Stat)
{
	const TSoftObjectPtr<UTexture2D>* Icon = GetDefault<URRIconSettings>()->StatIcons.Find(Stat);
	return Icon ? LoadIcon(*Icon) : nullptr;
}

UTexture2D* URRIconLibrary::GetWeaponIconByName(const FText& WeaponName)
{
	const TSoftObjectPtr<UTexture2D>* Icon = GetDefault<URRIconSettings>()->WeaponIcons.Find(WeaponName.ToString().TrimStartAndEnd());
	return Icon ? LoadIcon(*Icon) : nullptr;
}

UTexture2D* URRIconLibrary::GetWeaponIconByClass(TSubclassOf<ARRWeapon> WeaponClass)
{
	return WeaponClass ? GetWeaponIconByName(ARRWeapon::GetClassDisplayName(WeaponClass)) : nullptr;
}

UTexture2D* URRIconLibrary::GetUpgradeCardIcon(const FRRUpgradeCard& Card)
{
	return Card.IsNewWeapon() ? GetWeaponIconByClass(Card.WeaponClass) : GetStatIcon(Card.Stat);
}

UTexture2D* URRIconLibrary::GetLockedSlotIcon()
{
	return LoadIcon(GetDefault<URRIconSettings>()->LockedSlotIcon);
}

UTexture2D* URRIconLibrary::LoadIcon(const TSoftObjectPtr<UTexture2D>& Icon)
{
	if (Icon.IsNull())
	{
		return nullptr;
	}
	if (UTexture2D* Loaded = Icon.Get())
	{
		return Loaded;
	}
	if (!FPackageName::DoesPackageExist(Icon.ToSoftObjectPath().GetLongPackageName()))
	{
		return nullptr;
	}
	return Icon.LoadSynchronous();
}
