// Rogue Rigs - generic enemy movement: each tick asks the active path provider for a direction and feeds it to the pawn's movement input.

#include "FlowField/RREnemyMovementComponent.h"

#include "Engine/World.h"
#include "FlowField/RRFlowFieldSubsystem.h"
#include "GameFramework/Pawn.h"

URREnemyMovementComponent::URREnemyMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = TG_PrePhysics;
	PathProviderClass = URRFlowFieldSubsystem::StaticClass();
}

void URREnemyMovementComponent::SetPathProvider(TScriptInterface<IRRPathProvider> Provider)
{
	OverrideProvider = Provider;
}

IRRPathProvider* URREnemyMovementComponent::ResolveProvider()
{
	if (IRRPathProvider* Override = OverrideProvider.GetInterface())
	{
		return Override;
	}
	UClass* ProviderClass = *PathProviderClass;
	UWorld* World = GetWorld();
	if (!ProviderClass || !World)
	{
		return nullptr;
	}
	if (ProviderClass->IsChildOf(UWorldSubsystem::StaticClass()))
	{
		return Cast<IRRPathProvider>(World->GetSubsystemBase(TSubclassOf<UWorldSubsystem>(ProviderClass)));
	}
	if (ProviderClass->IsChildOf(UActorComponent::StaticClass()) && GetOwner())
	{
		return Cast<IRRPathProvider>(GetOwner()->FindComponentByClass(TSubclassOf<UActorComponent>(ProviderClass)));
	}
	return nullptr;
}

void URREnemyMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	LastMoveDirection = FVector::ZeroVector;
	bArrived = false;
	APawn* Pawn = Cast<APawn>(GetOwner());
	IRRPathProvider* Provider = bMovementEnabled ? ResolveProvider() : nullptr;
	FVector Goal;
	if (!Pawn || !Provider || !Provider->GetGoalLocation(Goal))
	{
		return;
	}

	const FVector Location = Pawn->GetActorLocation();
	FVector ToGoal = Goal - Location;
	ToGoal.Z = 0.f;
	const float GoalDistance = ToGoal.Size();
	if (GoalDistance <= ArrivalDistance)
	{
		bArrived = true;
		return;
	}

	FVector Direction = Provider->GetMoveDirection(Location);
	if ((GoalDistance <= DirectChaseRange || Direction.IsNearlyZero()) && Provider->HasDirectPath(Location, Goal))
	{
		Direction = ToGoal / GoalDistance;
	}
	if (DirectionFilter && !Direction.IsNearlyZero())
	{
		Direction = DirectionFilter(Direction, Goal);
	}
	if (!Direction.IsNearlyZero())
	{
		LastMoveDirection = Direction;
		Pawn->AddMovementInput(Direction, 1.f);
	}
}
