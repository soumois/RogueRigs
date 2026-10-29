// Rogue Rigs - bonus chest objective: an optional console-like hack target that rewards ore and a lucky upgrade choice (direct XP off by default).

#include "Objectives/RRBonusChest.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Pickups/RRPickup.h"
#include "Player/RRMiner.h"
#include "Progression/RRLevelUpComponent.h"
#include "Progression/RRProgressionComponent.h"
#include "Weapons/RRWeapon.h"
#include "RogueRigs.h"

namespace RRChestLockLight
{
	/** Parameters of M_ConsoleLamp, parent of MI_ChestLockLight (bp-objective-console). */
	static const FName LampColorParam(TEXT("LampColor"));
	static const FName BlinkRateParam(TEXT("BlinkRate"));
	static const FName BlinkAmountParam(TEXT("BlinkAmount"));
}

ARRBonusChest::ARRBonusChest()
{
	HackDuration = 8.f;
	ZoneRadiusCells = 6.f;
	MarkedCellTypes.Reset();
	bDriveStateVisuals = false; // the console lamps, screen and head drive is off; the chest drives its lock light instead (task-308-02)
	FootprintCells = FIntPoint(2, 2); // the chest keeps the 2 x 2 footprint, the console default is 3 x 3 (task-311-01)
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
	GrantReward();
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
	const int32 OreChunks = PopOreChunks();
	URRLevelUpComponent* LevelUp = bMinerAlive ? Miner->GetLevelUp() : nullptr;
	const bool bChoiceQueued = LevelUp && LevelUp->QueueBonusChoice(BonusLuck);

	UE_LOG(LogRogueRigs, Log, TEXT("%s: reward %.1f XP, %d ore chunk(s), upgrade choice %s (+%.1f luck)."), *GetName(), RewardXP, OreChunks,
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
