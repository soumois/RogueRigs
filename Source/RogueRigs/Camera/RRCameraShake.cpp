// Copyright Rogue Rigs. Shared light camera shake played on the local player's camera (task-310-01).

#include "Camera/RRCameraShake.h"
#include "Camera/RRCameraShakePattern.h"

URRCameraShake::URRCameraShake(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// A new hit restarts the shake instead of stacking several.
	bSingleInstance = true;

	URRCameraShakePattern* Pattern = CreateDefaultSubobject<URRCameraShakePattern>(TEXT("RootShakePattern"));
	SetRootShakePattern(Pattern);
}
