// Rogue Rigs - distance sounds (task-522).

#include "Audio/RRDistanceSound.h"

#include "Audio/RRAudioSettings.h"
#include "Audio/RRAudioSubsystem.h"
#include "Components/AudioComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogRRAudio, Log, All);

FRRDistanceSound::FRRDistanceSound(FName InEvent, FName InVariant)
	: Event(InVariant.IsNone() ? InEvent : FName(*FString::Printf(TEXT("%s.%s"), *InEvent.ToString(), *InVariant.ToString())))
{
}

UAudioComponent* FRRDistanceSound::PlayAt(const UObject* WorldContextObject, const FVector& Location) const
{
	URRAudioSubsystem* Subsystem = URRAudioSubsystem::Get(WorldContextObject);
	return Subsystem ? Subsystem->Play(WorldContextObject, Event, &Location, this) : nullptr;
}

float FRRDistanceSound::GetDistance(const FVector& PlayerLocation, const FVector& Location) const
{
	return FVector::Dist2D(PlayerLocation, Location);
}

float FRRDistanceSound::GetFade(const FRRSoundEventRow& Row, float Distance)
{
	if (Row.FadeOuter <= 0.f)
	{
		return 1.f;
	}
	const float Inner = FMath::Clamp(Row.FadeInner, 0.f, Row.FadeOuter);
	if (Distance <= Inner)
	{
		return 1.f;
	}
	if (Distance >= Row.FadeOuter)
	{
		return 0.f;
	}
	return 1.f - (Distance - Inner) / (Row.FadeOuter - Inner);
}

bool FRRDistanceSound::Measure(const UObject* WorldContextObject, const FRRSoundEventRow& Row, const FVector& Location, float& OutDistance, float& OutFade) const
{
	OutDistance = -1.f;
	OutFade = 1.f;
	if (Row.FadeOuter <= 0.f)
	{
		return false;
	}
	const UWorld* World = GEngine && WorldContextObject ? GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull) : nullptr;
	const APawn* Player = World ? UGameplayStatics::GetPlayerPawn(World, 0) : nullptr;
	if (Player == nullptr)
	{
		return false;
	}
	OutDistance = GetDistance(Player->GetActorLocation(), Location);
	OutFade = GetFade(Row, OutDistance);
	return true;
}

bool FRRDistanceSound::IsLoopPlaying() const
{
	const UAudioComponent* Component = LoopComponent.Get();
	return Component != nullptr && Component->IsPlaying();
}

void FRRDistanceSound::UpdateLoop(const UObject* WorldContextObject, bool bOn, const FVector& Location)
{
	if (!bOn)
	{
		ReleaseLoop(WorldContextObject, TEXT("released"));
		return;
	}
	URRAudioSubsystem* Subsystem = URRAudioSubsystem::Get(WorldContextObject);
	const FRRSoundEventRow* Row = GetDefault<URRAudioSettings>()->FindRow(Event);
	if (Subsystem == nullptr || Row == nullptr)
	{
		return;
	}
	float Distance = -1.f;
	float Fade = 1.f;
	Measure(WorldContextObject, *Row, Location, Distance, Fade);
	LoopFade = Fade;

	if (Fade <= 0.f)
	{
		// Out of hearing: no play is kept; it starts again (faded in) once the player comes back inside FadeOuter.
		ReleaseLoop(WorldContextObject, TEXT("player beyond FadeOuter"));
		return;
	}

	UAudioComponent* Component = LoopComponent.Get();
	if (Component == nullptr || !Component->IsPlaying())
	{
		// First call, back in range, after a pause, or a sound that does not loop and ended: one start.
		Component = Subsystem->StartDistanceLoop(WorldContextObject, *this, Location);
		LoopComponent = Component;
		LoggedStep = -1;
	}
	else
	{
		Component->SetVolumeMultiplier(Row->Volume * Fade);
		if (Row->bSpatial)
		{
			Component->SetWorldLocation(Location);
		}
	}

	const int32 Step = FMath::FloorToInt32(Fade * 10.f + KINDA_SMALL_NUMBER);
	if (Component != nullptr && Step != LoggedStep)
	{
		LoggedStep = Step;
		UE_LOG(LogRRAudio, Verbose, TEXT("Loop %s: %.0f uu from the player (edge radius %.0f), fade %.2f, volume %.3f."), *Event.ToString(),
			Distance, GetEdgeRadius(), Fade, Row->Volume * Fade);
	}
}

void FRRDistanceSound::StopLoop(const UObject* WorldContextObject)
{
	ReleaseLoop(WorldContextObject, TEXT("released"));
}

void FRRDistanceSound::ReleaseLoop(const UObject* WorldContextObject, const TCHAR* Reason)
{
	UAudioComponent* Component = LoopComponent.Get();
	LoopComponent.Reset();
	LoggedStep = -1;
	if (Component == nullptr || !Component->IsPlaying())
	{
		return;
	}
	if (URRAudioSubsystem* Subsystem = URRAudioSubsystem::Get(WorldContextObject))
	{
		Subsystem->ReleaseDistanceLoop(Event, Component, Reason);
	}
	else
	{
		Component->Stop();
	}
}

FRRExplosionSound::FRRExplosionSound(FName InEvent, float InRadius, FName InVariant)
	: FRRDistanceSound(InEvent, InVariant)
	, Radius(FMath::Max(InRadius, 0.f))
{
}

float FRRExplosionSound::GetDistance(const FVector& PlayerLocation, const FVector& Location) const
{
	return FMath::Max(FRRDistanceSound::GetDistance(PlayerLocation, Location) - Radius, 0.f);
}
