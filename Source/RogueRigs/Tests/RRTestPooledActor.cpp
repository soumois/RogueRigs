// Rogue Rigs - test-only pooled actor for RogueRigs.ActorPool.*: records the pool reset hooks, BeginPlay and the init callback order.

#include "Tests/RRTestPooledActor.h"

#include "Components/SceneComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRTestPooledActor)

ARRTestPooledActor::ARRTestPooledActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

void ARRTestPooledActor::BeginPlay()
{
	Super::BeginPlay();
	++BeginPlayCount;
	InitValueAtBeginPlay = InitValue;
}

void ARRTestPooledActor::OnPoolAcquired_Implementation()
{
	++AcquiredCount;
	InitValueAtAcquire = InitValue;
}

void ARRTestPooledActor::OnPoolReleased_Implementation()
{
	++ReleasedCount;
}
