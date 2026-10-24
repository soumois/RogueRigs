// Rogue Rigs - bonus chest objective: an optional console-like hack target that rewards ore and a lucky upgrade choice (direct XP off by default).

#include "Objectives/RRBonusChest.h"

#include "Engine/World.h"
#include "Pickups/RRPickup.h"
#include "Player/RRMiner.h"
#include "Progression/RRLevelUpComponent.h"
#include "Progression/RRProgressionComponent.h"
#include "Weapons/RRWeapon.h"
#include "RogueRigs.h"

ARRBonusChest::ARRBonusChest()
{
	HackDuration = 8.f;
	ZoneRadiusCells = 6.f;
	MarkedCellTypes.Reset();
	bDriveStateVisuals = false; // the chest keeps its own look (task-302-01)
	FootprintCells = FIntPoint(2, 2); // the chest keeps the 2 x 2 footprint, the console default is 3 x 3 (task-311-01)
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
