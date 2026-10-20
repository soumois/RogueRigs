// Rogue Rigs - seeded pulse desync values (task-257-02): a per-object phase and rate for the M_XPNutGlow pulse of ore and heal pickups and of the deposit crystals.

#pragma once

#include "CoreMinimal.h"

/**
 * Deterministic pulse values from a seed and a stable id (owner 2026-10-20, provisional): phase uniform in [0, 1) turns and
 * rate uniform in [RateMin, RateMax] (0.8..1.2 by default on the users). Pure integer hash (lowbias32 rounds): the same seed
 * and id give the same values on every run and platform, and it never touches the global FMath random stream (seeded by
 * other systems). Users: ARRPickup (id = pickup spawn counter of URRPickupSubsystem, Ore and Heal only; XP never written) and
 * ARRArenaGrid (id = deposit id for the deposit crystal formations, cell index for the per-cell crystal clusters). Each user
 * passes its own Domain so a pickup and a crystal with the same number do not share values.
 */
struct ROGUERIGS_API FRRPulseDesync
{
	/** Phase that leaves the material pulse unchanged (Custom Primitive Data / per-instance default). */
	static constexpr float NeutralPhase = 0.f;

	/** Rate that leaves the material pulse unchanged (Custom Primitive Data / per-instance default). */
	static constexpr float NeutralRate = 1.f;

	/** Domain of the ore and heal pickups (spawn counter ids). */
	static constexpr uint32 PickupDomain = 0x50494b55u;

	/** Domain of the deposit crystal formations (deposit ids). */
	static constexpr uint32 DepositDomain = 0x4445504fu;

	/** Domain of the per-cell crystal clusters (cell indices; used when a type has no deposit meshes). */
	static constexpr uint32 CellDomain = 0x43454c4cu;

	/** 32-bit hash of Seed, Domain, Id and Salt: equal inputs give equal outputs; any change of one input scrambles every bit. */
	static uint32 Hash(int32 Seed, uint32 Domain, uint32 Id, uint32 Salt);

	/** Pulse phase in turns, uniform in [0, 1), of object Id in Domain under Seed. */
	static float Phase(int32 Seed, uint32 Domain, uint32 Id);

	/** Pulse rate (scale of PulseFrequency), uniform in [min, max] of RateMin and RateMax, of object Id in Domain under Seed. */
	static float Rate(int32 Seed, uint32 Domain, uint32 Id, float RateMin, float RateMax);
};
