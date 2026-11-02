// Copyright Rogue Rigs. Camera shake modifier that mixes the project shakes and caps their summed offset (task-416-01).

#include "Camera/RRCameraShakeModifier.h"
#include "Camera/CameraShakeBase.h"
#include "Camera/CameraTypes.h"

URRCameraShakeModifier::URRCameraShakeModifier(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UCameraShakeBase* URRCameraShakeModifier::AddCameraShake(TSubclassOf<UCameraShakeBase> NewShake, const FAddCameraShakeParams& Params)
{
	FAddCameraShakeParams MixedParams = Params;

	const UCameraShakeBase* ShakeCDO = NewShake ? GetDefault<UCameraShakeBase>(NewShake) : nullptr;
	if (ShakeCDO && ShakeCDO->bSingleInstance)
	{
		// Same class already playing: the engine restarts it with the new scale; keep the stronger one.
		for (const FActiveCameraShakeInfo& ShakeInfo : ActiveShakes)
		{
			const UCameraShakeBase* ShakeInstance = ShakeInfo.ShakeInstance;
			if (ShakeInstance && ShakeInstance->GetClass() == NewShake && ShakeInstance->IsActive() && !ShakeInstance->IsFinished())
			{
				MixedParams.Scale = FMath::Max(MixedParams.Scale, ShakeInstance->ShakeScale);
				break;
			}
		}
	}

	return Super::AddCameraShake(NewShake, MixedParams);
}

bool URRCameraShakeModifier::ModifyCamera(float DeltaTime, FMinimalViewInfo& InOutPOV)
{
	const FVector LocationBeforeShakes = InOutPOV.Location;
	const bool bStopChain = Super::ModifyCamera(DeltaTime, InOutPOV);

	// Each shake adds its offset to the view in turn; clamp the sum.
	const FVector SummedOffset = InOutPOV.Location - LocationBeforeShakes;
	if (SummedOffset.SizeSquared() > FMath::Square(MaxSummedTranslation))
	{
		InOutPOV.Location = LocationBeforeShakes + SummedOffset.GetClampedToMaxSize(MaxSummedTranslation);
	}

	return bStopChain;
}
