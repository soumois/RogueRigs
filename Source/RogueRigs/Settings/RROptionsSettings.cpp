// Rogue Rigs - project data of the options screen (options-menu gdd): default asset paths and frame rate choices.

#include "Settings/RROptionsSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RROptionsSettings)

URROptionsSettings::URROptionsSettings()
{
	MasterSoundClass = FSoftObjectPath(TEXT("/Game/RogueRigs/Audio/SC_Master.SC_Master"));
	MusicSoundClass = FSoftObjectPath(TEXT("/Game/RogueRigs/Audio/SC_Music.SC_Music"));
	EffectsSoundClass = FSoftObjectPath(TEXT("/Game/RogueRigs/Audio/SC_Effects.SC_Effects"));
	VolumeSoundMix = FSoftObjectPath(TEXT("/Game/RogueRigs/Audio/Mix_Volumes.Mix_Volumes"));
	FrameRateLimitOptions = {30, 60, 120, 144, 0};
}

