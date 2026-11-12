// Rogue Rigs - tunables of the options screen (options-screen gdd, task-500-03).

#include "Settings/RROptionsSettings.h"

#include "Engine/Texture2D.h"
#include "Sound/SoundClass.h"
#include "UI/RROptionsWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RROptionsSettings)

URROptionsSettings::URROptionsSettings()
{
	MasterSoundClass = TSoftObjectPtr<USoundClass>(FSoftObjectPath(TEXT("/Game/RogueRigs/Audio/SC_Master.SC_Master")));
	MusicSoundClass = TSoftObjectPtr<USoundClass>(FSoftObjectPath(TEXT("/Game/RogueRigs/Audio/SC_Music.SC_Music")));
	EffectsSoundClass = TSoftObjectPtr<USoundClass>(FSoftObjectPath(TEXT("/Game/RogueRigs/Audio/SC_Effects.SC_Effects")));
	FrameRateLimits = {30, 60, 120, 144, 0};
	OptionsWidgetClass = TSoftClassPtr<URROptionsWidget>(FSoftObjectPath(TEXT("/Game/RogueRigs/UI/WBP_Options.WBP_Options_C")));
	ActiveTabTexture = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/RogueRigs/UI/Shop/T_Shop_ButtonContinue.T_Shop_ButtonContinue")));
	InactiveTabTexture = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/RogueRigs/UI/Options/T_Options_Tab.T_Options_Tab")));
}
