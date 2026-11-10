// Rogue Rigs - text content subsystem: loads the validated text files once per game world and serves flavor, alert, bark and screen texts with fallback.

#include "Text/RRTextContentSubsystem.h"

#include "Engine/Engine.h"
#include "Engine/World.h"
#include "RogueRigs.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRTextContentSubsystem)

#define LOCTEXT_NAMESPACE "RRTextContent"

void URRTextContentSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	RandomStream.Initialize(FMath::Rand());
}

bool URRTextContentSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

URRTextContentSubsystem* URRTextContentSubsystem::Get(const UObject* WorldContext)
{
	const UWorld* World = WorldContext ? GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::ReturnNull) : nullptr;
	return World ? World->GetSubsystem<URRTextContentSubsystem>() : nullptr;
}

FText URRTextContentSubsystem::ComposeUpgradeDescription(const FText& StatName, const FText& Flavor)
{
	return FText::Format(LOCTEXT("UpgradeDescription", "{0}\n{1}"), StatName, Flavor);
}

void URRTextContentSubsystem::EnsureLoaded()
{
	if (!bLoaded)
	{
		bLoaded = true;
		Library.LoadFromDirectory(FRRTextContentLibrary::GetDefaultDirectory());
	}
}

const FRRTextContentLibrary& URRTextContentSubsystem::GetLibrary()
{
	EnsureLoaded();
	return Library;
}

int32 URRTextContentSubsystem::ReloadFromDirectory(const FString& Directory)
{
	bLoaded = true;
	CurrentAlertLines.Reset();
	WarnedKeys.Reset();
	return Library.LoadFromDirectory(Directory.IsEmpty() ? FRRTextContentLibrary::GetDefaultDirectory() : Directory);
}

void URRTextContentSubsystem::WarnOnce(const FString& Key, const FString& Message)
{
	bool bAlreadyWarned = false;
	WarnedKeys.Add(Key, &bAlreadyWarned);
	if (!bAlreadyWarned)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("URRTextContentSubsystem: %s"), *Message);
	}
}

bool URRTextContentSubsystem::GetUpgradeFlavor(const FString& StatNameOrId, FText& OutText)
{
	EnsureLoaded();
	FString Flavor;
	if (!Library.FindUpgradeFlavor(StatNameOrId, Flavor))
	{
		OutText = FText::GetEmpty();
		// A missing file was already reported at load: warn only about ids absent from a loaded file.
		if (Library.GetUpgradeFlavorCount() > 0)
		{
			WarnOnce(TEXT("upgrade|") + StatNameOrId.ToLower(), FString::Printf(TEXT("no flavor line for stat '%s', functional text kept."), *StatNameOrId));
		}
		return false;
	}
	OutText = FText::FromString(Flavor);
	return true;
}

bool URRTextContentSubsystem::GetWeaponFlavor(const FString& WeaponNameOrId, FText& OutText)
{
	EnsureLoaded();
	FString Flavor;
	if (!Library.FindWeaponFlavor(WeaponNameOrId, Flavor))
	{
		OutText = FText::GetEmpty();
		// A missing file was already reported at load: warn only about weapons absent from a loaded file.
		if (Library.GetWeaponFlavorCount() > 0)
		{
			WarnOnce(TEXT("weapon|") + WeaponNameOrId.ToLower(), FString::Printf(TEXT("no lore line for weapon '%s', card lore left empty."), *WeaponNameOrId));
		}
		return false;
	}
	OutText = FText::FromString(Flavor);
	return true;
}

bool URRTextContentSubsystem::GetAlertLine(FName EventId, FText& OutText)
{
	EnsureLoaded();
	FString Line;
	if (!Library.PickAlertLine(EventId, RandomStream, Line))
	{
		OutText = FText::GetEmpty();
		CurrentAlertLines.Remove(EventId);
		if (Library.GetAlertLineCount() > 0)
		{
			WarnOnce(TEXT("alert|") + EventId.ToString().ToLower(), FString::Printf(TEXT("no alert line for event '%s', functional text kept."), *EventId.ToString()));
		}
		return false;
	}
	OutText = FText::FromString(Line);
	CurrentAlertLines.Add(EventId, OutText);
	return true;
}

FText URRTextContentSubsystem::GetCurrentAlertLine(FName EventId) const
{
	const FText* Found = CurrentAlertLines.Find(EventId);
	return Found ? *Found : FText::GetEmpty();
}

bool URRTextContentSubsystem::GetBark(FName TriggerId, FName SpeakerId, FText& OutText)
{
	EnsureLoaded();
	FString Text;
	if (!Library.PickBark(TriggerId, SpeakerId, RandomStream, Text))
	{
		OutText = FText::GetEmpty();
		if (Library.GetBarkCount() > 0)
		{
			WarnOnce(TEXT("bark|") + TriggerId.ToString().ToLower() + TEXT("|") + SpeakerId.ToString().ToLower(),
				FString::Printf(TEXT("no bark for trigger '%s' speaker '%s', nothing shown."), *TriggerId.ToString(), *SpeakerId.ToString()));
		}
		return false;
	}
	OutText = FText::FromString(Text);
	return true;
}

bool URRTextContentSubsystem::PickBarkLine(FName TriggerId, FName& OutSpeakerId, FText& OutText)
{
	EnsureLoaded();
	FString Text;
	OutSpeakerId = NAME_None;
	if (!Library.PickBark(TriggerId, NAME_None, RandomStream, Text, &OutSpeakerId))
	{
		OutText = FText::GetEmpty();
		if (Library.GetBarkCount() > 0)
		{
			WarnOnce(TEXT("bark|") + TriggerId.ToString().ToLower() + TEXT("|none"),
				FString::Printf(TEXT("no bark for trigger '%s', nothing shown."), *TriggerId.ToString()));
		}
		return false;
	}
	OutText = FText::FromString(Text);
	return true;
}

bool URRTextContentSubsystem::GetScreenText(FName ScreenId, FRRScreenText& OutScreenText)
{
	EnsureLoaded();
	if (const FRRScreenText* Found = Library.FindScreenText(ScreenId))
	{
		OutScreenText = *Found;
		return true;
	}
	OutScreenText = FRRScreenText();
	// A missing file was already reported at load: warn only about ids absent from a loaded file.
	if (Library.GetScreenTextCount() > 0)
	{
		WarnOnce(TEXT("screen|") + ScreenId.ToString().ToLower(), FString::Printf(TEXT("no screen text '%s', widget text kept."), *ScreenId.ToString()));
	}
	return false;
}

FText URRTextContentSubsystem::GetScreenTitle(FName ScreenId)
{
	FRRScreenText Screen;
	return GetScreenText(ScreenId, Screen) ? Screen.Title : FText::GetEmpty();
}

FText URRTextContentSubsystem::GetScreenSubtitle(FName ScreenId)
{
	FRRScreenText Screen;
	return GetScreenText(ScreenId, Screen) ? Screen.Subtitle : FText::GetEmpty();
}

#undef LOCTEXT_NAMESPACE
