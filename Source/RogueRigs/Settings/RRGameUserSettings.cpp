// Rogue Rigs - player settings saved in GameUserSettings.ini (damage number toggle).

#include "Settings/RRGameUserSettings.h"

#include "Engine/Engine.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRGameUserSettings)

URRGameUserSettings* URRGameUserSettings::GetRRGameUserSettings()
{
	return GEngine ? Cast<URRGameUserSettings>(GEngine->GetGameUserSettings()) : nullptr;
}

void URRGameUserSettings::SetToDefaults()
{
	Super::SetToDefaults();
	bShowDamageNumbers = true;
}
