// Rogue Rigs - sound event table (audio-direction, task-511).

#include "Audio/RRAudioSettings.h"

#include "Sound/SoundBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRAudioSettings)

const FRRSoundEventRow* URRAudioSettings::FindRow(FName Event) const
{
	FString Name = Event.ToString();
	while (!Name.IsEmpty())
	{
		const FName Key(*Name);
		if (const FRRSoundEventRow* Row = Events.FindByPredicate([&Key](const FRRSoundEventRow& Candidate) { return Candidate.Event == Key; }))
		{
			return Row;
		}
		int32 Dot = INDEX_NONE;
		if (!Name.FindLastChar(TEXT('.'), Dot))
		{
			break;
		}
		Name.LeftInline(Dot);
	}
	return nullptr;
}

const FRRMusicTrackRow* URRAudioSettings::FindMusic(FName Track) const
{
	return Music.FindByPredicate([Track](const FRRMusicTrackRow& Candidate) { return Candidate.Track == Track; });
}
