// Rogue Rigs - shield chaser: a chaser whose CSV Shield (30) absorbs damage before health; Blueprint events and getters for its shell.

#include "Enemies/RRShieldChaser.h"

#include "GAS/RRHealthAttributeSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRShieldChaser)

ARRShieldChaser::ARRShieldChaser(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	StatsRowName = TEXT("ShieldChaser");
}

void ARRShieldChaser::BeginPlay()
{
	Super::BeginPlay();

	if (HealthSet)
	{
		ShieldBrokenHandle = HealthSet->OnShieldBroken.AddUObject(this, &ARRShieldChaser::HandleShieldBroken);
	}
}

void ARRShieldChaser::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (HealthSet)
	{
		HealthSet->OnShieldBroken.Remove(ShieldBrokenHandle);
	}
	ShieldBrokenHandle.Reset();

	Super::EndPlay(EndPlayReason);
}

void ARRShieldChaser::ResetForNewLife()
{
	Super::ResetForNewLife();
	ShieldBreakCount = 0;
}

float ARRShieldChaser::GetShield() const
{
	return HealthSet ? HealthSet->GetShield() : 0.f;
}

float ARRShieldChaser::GetShieldFraction() const
{
	return Stats.Shield > 0.f ? FMath::Clamp(GetShield() / Stats.Shield, 0.f, 1.f) : 0.f;
}

void ARRShieldChaser::HandleShieldBroken()
{
	++ShieldBreakCount;
	OnShieldBroken();
}
