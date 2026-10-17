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
	ShieldBreakDirection = FVector::ZeroVector;
}

float ARRShieldChaser::GetShield() const
{
	return HealthSet ? HealthSet->GetShield() : 0.f;
}

float ARRShieldChaser::GetShieldFraction() const
{
	return Stats.Shield > 0.f ? FMath::Clamp(GetShield() / Stats.Shield, 0.f, 1.f) : 0.f;
}

FVector ARRShieldChaser::GetShieldDebrisDirection() const
{
	const FVector Forward = GetActorForwardVector().GetSafeNormal2D(UE_SMALL_NUMBER, FVector::ForwardVector);
	const FVector Base = ShieldBreakDirection.IsZero() ? -Forward : ShieldBreakDirection;
	// From behind: the hit travels along the robot forward; the fragments follow it unturned.
	if (FVector::DotProduct(Base, Forward) > 0.f)
	{
		return Base;
	}
	// Rotation sign: Unreal yaw turns +X toward +Y, and +Y is the robot right when it faces +X. On this branch the base lies in
	// the robot back half-plane, so turning it toward the shield side is always yaw = -ShieldSide * angle. Worked example: robot
	// facing +X, hit straight from the front travels -X, base (-1, 0, 0), shield on the left (ShieldSide -1): yaw +20 gives
	// (cos 200, sin 200, 0) = (-0.94, -0.34, 0), thrown backward and toward -Y, the robot left, the shield side.
	const float Yaw = -static_cast<float>(FMath::Clamp(ShieldSide, -1, 1)) * ShieldDebrisSideAngleDeg;
	return FRotator(0.f, Yaw, 0.f).RotateVector(Base).GetSafeNormal2D();
}

void ARRShieldChaser::HandleShieldBroken()
{
	// The breaking hit recorded its direction before its effect was applied (projectiles, explosions, rr.Debug.BreakShields),
	// so the last hit direction is the one of the hit that broke the shield. Stored apart: later hits overwrite the last one.
	ShieldBreakDirection = GetLastHitDirection();
	++ShieldBreakCount;
	OnShieldBroken();
}
