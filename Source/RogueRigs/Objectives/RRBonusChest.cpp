// Rogue Rigs - bonus chest objective: an optional console-like hack target that opens (latch, lid slam, inner light) and rewards XP chips, ore and a lucky upgrade choice.

#include "Objectives/RRBonusChest.h"

#include "Camera/CameraShakeBase.h"
#include "Camera/RRCameraShakeLibrary.h"
#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "NiagaraComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "UObject/UObjectHash.h"
#include "Pickups/RRPickup.h"
#include "Player/RRMiner.h"
#include "Progression/RRLevelUpComponent.h"
#include "Progression/RRProgressionComponent.h"
#include "Text/RRSubtitleSubsystem.h"
#include "Weapons/RRWeapon.h"
#include "World/RRBlobShadowComponent.h"
#include "World/RRShadows.h"
#include "RogueRigs.h"

namespace RRChestLockLight
{
	/** Parameters of M_ConsoleLamp, parent of MI_ChestLockLight (bp-objective-console). */
	static const FName LampColorParam(TEXT("LampColor"));
	static const FName BlinkRateParam(TEXT("BlinkRate"));
	static const FName BlinkAmountParam(TEXT("BlinkAmount"));
}

const FName ARRBonusChest::OpeningEffectTag(TEXT("RRBonusChestOpeningEffect"));

ARRBonusChest::ARRBonusChest()
{
	HackDuration = 8.f;
	ZoneRadiusCells = 6.f;
	MarkedCellTypes.Reset();
	bDriveStateVisuals = false; // the console lamps, screen and head drive is off; the chest drives its lock light instead (task-308-02)
	FootprintCells = FIntPoint(2, 2); // the chest keeps the 2 x 2 footprint, the console default is 3 x 3 (task-311-01)

	// task-309-01: the XP burst (3 chips of 8, inherited defaults) comes at the lid opening, not at the completion. The chips start
	// 80 uu out (implementation choice: clears the 130 x 70 uu body, half diagonal about 74 uu; the console uses 140 for its 3 x 3 mesh).
	bXPBurstOnComplete = false;
	XPBurstSpawnRadius = 80.f;

	// Inner light: hidden until the lid opening; colour, intensity, radius and place are Blueprint defaults (task-309-05).
	InnerLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("InnerLight"));
	InnerLight->SetupAttachment(Mesh);
	InnerLight->SetRelativeLocation(FVector(0.f, 0.f, 30.f)); // inside the cavity (floor 19.25, rim 41.25 uu); the Blueprint may move it
	InnerLight->SetVisibleFlag(false);
	InnerLight->SetCastShadows(false);

	// task-472-01: light-independent shadow, fitted to the chest meshes at BeginPlay (consoles have none, Q1).
	BlobShadow = CreateDefaultSubobject<URRBlobShadowComponent>(TEXT("BlobShadow"));
	BlobShadow->SetupAttachment(RootComponent);
}

void ARRBonusChest::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	RRShadows::DisableCastShadows(this);
}

void ARRBonusChest::BeginPlay()
{
	Super::BeginPlay();
	BlobShadow->FitToOwnerMeshes();
	if (InnerLight)
	{
		InnerLightOnIntensity = InnerLight->Intensity;
		InnerLight->SetVisibility(false);
	}
}

void ARRBonusChest::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (bOpeningStarted && !bOpeningFinished)
	{
		UpdateOpening(DeltaSeconds);
	}
}

float ARRBonusChest::GetLatchAngleAt(float SecondsSinceCompletion) const
{
	if (SecondsSinceCompletion <= 0.f)
	{
		return 0.f;
	}
	if (SecondsSinceCompletion >= LatchSwingDuration)
	{
		return LatchOpenAngle;
	}
	return FMath::InterpEaseOut(0.f, LatchOpenAngle, SecondsSinceCompletion / LatchSwingDuration, FMath::Max(LatchEaseExponent, 1.f));
}

float ARRBonusChest::GetLidAngleAt(float SecondsSinceLidStart) const
{
	if (SecondsSinceLidStart <= 0.f)
	{
		return 0.f;
	}
	float Time = SecondsSinceLidStart;
	if (Time < LidRiseDuration)
	{
		return FMath::InterpEaseOut(0.f, LidPeakAngle, Time / LidRiseDuration, FMath::Max(LidRiseEaseExponent, 1.f));
	}
	Time -= LidRiseDuration;
	if (Time < LidHoldDuration)
	{
		return LidPeakAngle;
	}
	Time -= LidHoldDuration;
	if (Time < LidSettleDuration)
	{
		return FMath::InterpEaseInOut(LidPeakAngle, LidOpenAngle, Time / LidSettleDuration, 2.f);
	}
	return LidOpenAngle;
}

void ARRBonusChest::SetupOpeningComponents()
{
	TInlineComponentArray<UStaticMeshComponent*> MeshComponents(this);
	for (UStaticMeshComponent* Component : MeshComponents)
	{
		if (!Component)
		{
			continue;
		}
		if (!LidComp && Component->GetFName() == LidComponentName)
		{
			LidComp = Component;
		}
		else if (!LatchComp && Component->GetFName() == LatchComponentName)
		{
			LatchComp = Component;
		}
	}
	LidClosedRotation = LidComp ? LidComp->GetRelativeRotation().Quaternion() : FQuat::Identity;
	LatchClosedRotation = LatchComp ? LatchComp->GetRelativeRotation().Quaternion() : FQuat::Identity;
	UE_LOG(LogRogueRigs, Verbose, TEXT("%s: opening lid %s, latch %s."), *GetName(), LidComp ? TEXT("yes") : TEXT("no"), LatchComp ? TEXT("yes") : TEXT("no"));
}

void ARRBonusChest::ApplyRoll(USceneComponent* Component, const FQuat& ClosedRotation, float Sign, float AngleDegrees)
{
	if (Component)
	{
		// Relative Roll = rotation about the component's own X (the hinge, or the latch top edge), after its closed rotation.
		Component->SetRelativeRotation(ClosedRotation * FQuat(FRotator(0.f, 0.f, Sign * AngleDegrees)));
	}
}

void ARRBonusChest::UpdateOpening(float DeltaSeconds)
{
	OpeningElapsed += FMath::Max(DeltaSeconds, 0.f);

	LatchAngle = GetLatchAngleAt(OpeningElapsed);
	ApplyRoll(LatchComp, LatchClosedRotation, LatchRollSign, LatchAngle);

	if (!bLidOpened && OpeningElapsed >= LidOpenDelay)
	{
		OpenLid();
	}
	if (!bLidOpened)
	{
		return;
	}

	const float LidTime = OpeningElapsed - LidOpenDelay;
	LidAngle = GetLidAngleAt(LidTime);
	ApplyRoll(LidComp, LidClosedRotation, LidRollSign, LidAngle);

	// Slam: the hold at LidPeakAngle starts (ground ring), then the light shake at its end (chest only, no time dilation).
	if (!bLidSlamFired && LidTime >= LidRiseDuration)
	{
		bLidSlamFired = true;
		UE_LOG(LogRogueRigs, Verbose, TEXT("%s: lid slam at %.2f s."), *GetName(), OpeningElapsed);
		TMap<const UFXSystemComponent*, bool> EffectsBefore;
		CollectWorldEffects(EffectsBefore);
		OnLidSlam(GetActorLocation());
		TrackEffectsSpawnedSince(EffectsBefore, TEXT("OnLidSlam"));
	}
	if (bLidSlamFired && !bSlamShakePlayed && LidTime >= LidRiseDuration + LidHoldDuration)
	{
		bSlamShakePlayed = true;
		if (LidSlamShakeScale > 0.f)
		{
			URRCameraShakeLibrary::PlayCameraShake(this, LidSlamShakeScale, LidSlamShakeClass);
		}
	}

	// Inner light: Blueprint intensity for InnerLightOnDuration, then a linear fade to 0 and hidden; the interior glow stays.
	if (!bInnerLightFaded)
	{
		const float FadeTime = LidTime - InnerLightOnDuration;
		if (FadeTime >= 0.f)
		{
			const float Alpha = InnerLightFadeDuration > 0.f ? FMath::Clamp(FadeTime / InnerLightFadeDuration, 0.f, 1.f) : 1.f;
			if (InnerLight)
			{
				InnerLight->SetIntensity(InnerLightOnIntensity * (1.f - Alpha));
			}
			if (Alpha >= 1.f)
			{
				bInnerLightFaded = true;
				if (InnerLight)
				{
					InnerLight->SetVisibility(false);
				}
			}
		}
	}

	const bool bLidSettled = LidTime >= LidRiseDuration + LidHoldDuration + LidSettleDuration;
	if (bLidSettled && bSlamShakePlayed && bInnerLightFaded && OpeningElapsed >= LatchSwingDuration)
	{
		bOpeningFinished = true;
		SetActorTickEnabled(false);
		UE_LOG(LogRogueRigs, Verbose, TEXT("%s: opening finished at %.2f s."), *GetName(), OpeningElapsed);
	}
}

void ARRBonusChest::OpenLid()
{
	if (bLidOpened)
	{
		return;
	}
	bLidOpened = true;
	if (InnerLight)
	{
		InnerLight->SetIntensity(InnerLightOnIntensity);
		InnerLight->SetVisibility(true);
	}
	ApplyInteriorGlow();
	TMap<const UFXSystemComponent*, bool> EffectsBefore;
	CollectWorldEffects(EffectsBefore);
	OnLidOpened(GetActorLocation());
	TrackEffectsSpawnedSince(EffectsBefore, TEXT("OnLidOpened"));
	if (URRSubtitleSubsystem* Subtitles = URRSubtitleSubsystem::Get(this))
	{
		Subtitles->ShowBark(FName(TEXT("bonus_chest_opened")));
	}
	GrantReward();
}

void ARRBonusChest::CollectWorldEffects(TMap<const UFXSystemComponent*, bool>& Out) const
{
	const UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	ForEachObjectOfClass(UFXSystemComponent::StaticClass(), [World, &Out](UObject* Object)
	{
		const UFXSystemComponent* Effect = static_cast<const UFXSystemComponent*>(Object);
		if (IsValid(Effect) && !Effect->IsTemplate() && Effect->GetWorld() == World)
		{
			Out.Add(Effect, Effect->IsActive());
		}
	});
}

int32 ARRBonusChest::TrackEffectsSpawnedSince(const TMap<const UFXSystemComponent*, bool>& Before, const TCHAR* EventName)
{
	TMap<const UFXSystemComponent*, bool> After;
	CollectWorldEffects(After);
	TArray<FString> Names;
	for (const TPair<const UFXSystemComponent*, bool>& Pair : After)
	{
		const UFXSystemComponent* Effect = Pair.Key;
		const bool* bWasActive = Before.Find(Effect);
		// task-526-01: a component created by the event counts even when not active yet. The first NS_BonusChestOpen of a
		// session was still compiling at the spawn (Niagara defers the activation), so it was missed and never cleared.
		if (bWasActive && (*bWasActive || !Pair.Value))
		{
			continue;
		}
		// Spawned by the Blueprint event just now: the chest does not own it (SpawnSystemAtLocation outers it to the world).
		UFXSystemComponent* Tracked = const_cast<UFXSystemComponent*>(Effect);
		Tracked->ComponentTags.AddUnique(OpeningEffectTag);
		OpeningEffects.Add(Tracked);
		Names.Add(GetNameSafe(Tracked->GetFXSystemAsset()));
	}
	UE_LOG(LogRogueRigs, Log, TEXT("%s: %s spawned %d world effect(s) [%s] (tracked %d)."), *GetName(), EventName, Names.Num(), *FString::Join(Names, TEXT(", ")),
		OpeningEffects.Num());
	return Names.Num();
}

bool ARRBonusChest::IsOpeningEffectAlive(const UFXSystemComponent* Effect)
{
	if (!IsValid(Effect))
	{
		return false;
	}
	if (Effect->IsActive())
	{
		return true;
	}
	// A Niagara component whose activation waits for its system (compile, PSOs) is not active yet but ticks until it starts.
	const UNiagaraComponent* Niagara = Cast<UNiagaraComponent>(Effect);
	return Niagara && Niagara->IsComponentTickEnabled() && !Niagara->IsComplete();
}

int32 ARRBonusChest::ClearOpeningEffects()
{
	int32 Cleared = 0;
	TArray<FString> Names;
	for (const TWeakObjectPtr<UFXSystemComponent>& WeakEffect : OpeningEffects)
	{
		UFXSystemComponent* Effect = WeakEffect.Get();
		if (!IsValid(Effect) || Effect->IsBeingDestroyed())
		{
			continue;
		}
		if (IsOpeningEffectAlive(Effect))
		{
			++Cleared;
			Names.Add(GetNameSafe(Effect->GetFXSystemAsset()));
		}
		Effect->ComponentTags.Remove(OpeningEffectTag);
		// Deactivate first: on Niagara it also cancels an activation still waiting for the system (DeactivateImmediate does not).
		Effect->Deactivate();
		Effect->DeactivateImmediate();
		// Pooled Niagara components go back to their pool (auto release on completion, manual release here); the others are destroyed.
		const UNiagaraComponent* Niagara = Cast<UNiagaraComponent>(Effect);
		const ENCPoolMethod PoolMethod = Niagara ? Niagara->PoolingMethod : ENCPoolMethod::None;
		if (PoolMethod == ENCPoolMethod::ManualRelease)
		{
			Effect->ReleaseToPool();
		}
		else if (PoolMethod == ENCPoolMethod::None)
		{
			Effect->DestroyComponent();
		}
	}
	OpeningEffects.Reset();

	if (InnerLight && InnerLight->IsVisible())
	{
		++Cleared;
		Names.Add(TEXT("InnerLight"));
		InnerLight->SetVisibility(false);
	}
	if (Cleared > 0)
	{
		UE_LOG(LogRogueRigs, Log, TEXT("%s: opening effects still alive removed: %d [%s]."), *GetName(), Cleared, *FString::Join(Names, TEXT(", ")));
	}
	return Cleared;
}

int32 ARRBonusChest::CountLiveOpeningEffects(const UWorld* World)
{
	if (!World)
	{
		return 0;
	}
	int32 Count = 0;
	ForEachObjectOfClass(UFXSystemComponent::StaticClass(), [World, &Count](UObject* Object)
	{
		const UFXSystemComponent* Effect = static_cast<const UFXSystemComponent*>(Object);
		if (IsValid(Effect) && !Effect->IsTemplate() && Effect->GetWorld() == World && Effect->ComponentHasTag(OpeningEffectTag) && IsOpeningEffectAlive(Effect))
		{
			++Count;
		}
	});
	for (TActorIterator<ARRBonusChest> It(const_cast<UWorld*>(World)); It; ++It)
	{
		const UPointLightComponent* Light = IsValid(*It) ? It->GetInnerLight() : nullptr;
		if (Light && Light->IsVisible())
		{
			++Count;
		}
	}
	return Count;
}

void ARRBonusChest::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ClearOpeningEffects();
	Super::EndPlay(EndPlayReason);
}

void ARRBonusChest::ApplyInteriorGlow()
{
	if (!InteriorMID)
	{
		const int32 Index = Mesh && !InteriorSlotName.IsNone() ? Mesh->GetMaterialIndex(InteriorSlotName) : INDEX_NONE;
		if (Index != INDEX_NONE && Mesh->GetMaterial(Index))
		{
			InteriorMID = Mesh->CreateDynamicMaterialInstance(Index);
		}
	}
	if (InteriorMID)
	{
		InteriorMID->SetScalarParameterValue(InteriorGlowParameterName, InteriorGlowOnValue);
	}
}

float ARRBonusChest::GetHackingBlinkRate(float Progress) const
{
	const float Alpha = FMath::Clamp(Progress, 0.f, 1.f);
	if (LockHackBlinkRateSteps.IsEmpty())
	{
		return FMath::Lerp(LockHackBlinkRateStart, LockHackBlinkRateEnd, Alpha);
	}
	const int32 Count = LockHackBlinkRateSteps.Num();
	const int32 Band = FMath::Min(FMath::FloorToInt32(Alpha * static_cast<float>(Count)), Count - 1);
	return FMath::Max(LockHackBlinkRateSteps[Band], 0.f);
}

void ARRBonusChest::ApplyStateVisuals()
{
	Super::ApplyStateVisuals();
	RefreshLockLight();
}

void ARRBonusChest::ApplyProgressVisuals()
{
	Super::ApplyProgressVisuals();
	// Only while the drone is linked: the progress write that precedes a state change (leaving the zone, completion) keeps the old look until the state visual runs.
	if (ConsoleState == ERRConsoleState::Hacking && bDroneLinked)
	{
		RefreshLockLight();
	}
}

void ARRBonusChest::SetupLockLight()
{
	if (bLockLightSetUp)
	{
		return;
	}
	bLockLightSetUp = true;

	UStaticMeshComponent* Latch = nullptr;
	TInlineComponentArray<UStaticMeshComponent*> MeshComponents(this);
	for (UStaticMeshComponent* Component : MeshComponents)
	{
		if (Component && Component->GetFName() == LatchComponentName)
		{
			Latch = Component;
			break;
		}
	}
	// No latch or slot (headless tests use the C++ class): the colour and rate are still computed, nothing is written.
	const int32 Index = Latch && !LockLightSlotName.IsNone() ? Latch->GetMaterialIndex(LockLightSlotName) : INDEX_NONE;
	if (Index != INDEX_NONE && Latch->GetMaterial(Index))
	{
		LockLightMID = Latch->CreateDynamicMaterialInstance(Index);
	}
	UE_LOG(LogRogueRigs, Verbose, TEXT("%s: lock light latch %s, instance %s."), *GetName(), Latch ? TEXT("yes") : TEXT("no"), LockLightMID ? TEXT("yes") : TEXT("no"));
}

void ARRBonusChest::RefreshLockLight()
{
	SetupLockLight();

	FLinearColor NewColor = LockIdleColor;
	float NewRate = LockIdleBlinkRate;
	if (ConsoleState == ERRConsoleState::Hacking)
	{
		NewColor = LockHackColor;
		NewRate = GetHackingBlinkRate(HackProgress);
	}
	else if (ConsoleState == ERRConsoleState::Complete)
	{
		NewColor = LockCompleteColor;
		NewRate = 0.f;
	}
	NewRate = FMath::Max(NewRate, 0.f);

	// Write only on change: every BlinkRate write shifts the material blink phase (Sine(Time x BlinkRate)).
	if (bLockLightWritten && NewColor == LockLightColor && NewRate == LockLightBlinkRate)
	{
		return;
	}
	bLockLightWritten = true;
	LockLightColor = NewColor;
	LockLightBlinkRate = NewRate;
	if (LockLightMID)
	{
		LockLightMID->SetVectorParameterValue(RRChestLockLight::LampColorParam, LockLightColor);
		LockLightMID->SetScalarParameterValue(RRChestLockLight::BlinkRateParam, LockLightBlinkRate);
		LockLightMID->SetScalarParameterValue(RRChestLockLight::BlinkAmountParam, LockLightBlinkRate > 0.f ? LockBlinkAmount : 0.f);
	}
}

void ARRBonusChest::CompleteHack()
{
	if (IsComplete())
	{
		return;
	}
	Super::CompleteHack();
	// task-309-01: the latch swings now; the lid opening (and the reward) comes LidOpenDelay later, in the chest tick.
	bOpeningStarted = true;
	OpeningElapsed = 0.f;
	SetupOpeningComponents();
	SetActorTickEnabled(true);
	UpdateOpening(0.f);
}

void ARRBonusChest::GrantReward()
{
	if (bRewardGranted)
	{
		return;
	}
	bRewardGranted = true;

	const bool bMinerAlive = IsValid(Miner) && !Miner->IsDead();
	if (URRProgressionComponent* Progression = bMinerAlive ? Miner->GetProgression() : nullptr)
	{
		RewardXP = XPNeedFraction * Progression->GetXPToNextLevel();
		if (RewardXP > 0.f)
		{
			Progression->AddXP(RewardXP);
		}
	}
	XPBurstChipsSpawned = SpawnXPBurst(ChipScalePop);
	const int32 OreChunks = PopOreChunks();
	URRLevelUpComponent* LevelUp = bMinerAlive ? Miner->GetLevelUp() : nullptr;
	const bool bChoiceQueued = LevelUp && LevelUp->QueueBonusChoice(BonusLuck);

	UE_LOG(LogRogueRigs, Log, TEXT("%s: reward %.1f XP, %d XP chip(s) of %.1f, %d ore chunk(s), upgrade choice %s (+%.1f luck)."), *GetName(), RewardXP,
		XPBurstChipsSpawned, XPBurstChipXP, OreChunks,
		bChoiceQueued ? TEXT("queued") : TEXT("not queued"), BonusLuck);
	OnRewardGranted(RewardXP, OreChunks, bChoiceQueued);
}

int32 ARRBonusChest::PopOreChunks()
{
	UWorld* World = GetWorld();
	const TSubclassOf<ARRPickup> PickupClass = OrePickupClass ? OrePickupClass : (IsValid(Grid) ? Grid->GetDropPickupClass(ERRCellType::Ore) : nullptr);
	if (OreChunkCount <= 0 || !World || !World->IsGameWorld())
	{
		return 0;
	}
	if (!PickupClass)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("%s: no ore pickup class (chest or grid); %d ore chunk(s) not dropped."), *GetName(), OreChunkCount);
		return 0;
	}

	// Evenly spread directions from a random start angle, like a robot's XP burst.
	const float Spacing = 360.f / static_cast<float>(OreChunkCount);
	const float StartYaw = FMath::FRandRange(0.f, 360.f);
	const FTransform SpawnTransform(FRotator::ZeroRotator, GetActorLocation());
	int32 Spawned = 0;
	for (int32 Index = 0; Index < OreChunkCount; ++Index)
	{
		ARRPickup* Chunk = World->SpawnActorDeferred<ARRPickup>(PickupClass, SpawnTransform, this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (!Chunk)
		{
			continue;
		}
		Chunk->SetPopYaw(StartYaw + Spacing * static_cast<float>(Index));
		Chunk->FinishSpawning(SpawnTransform);
		++Spawned;
	}
	return Spawned;
}
