// Rogue Rigs - seeded pulse desync values (task-257-02): a per-object phase and rate for the M_XPNutGlow pulse of ore and heal pickups and of the deposit crystals.

#include "Pickups/RRPulseDesync.h"

namespace RRPulseDesyncPrivate
{
	/** lowbias32 integer mix (same as the grid hashes): full avalanche, deterministic. */
	uint32 Mix(uint32 Value)
	{
		Value ^= Value >> 16;
		Value *= 0x7feb352du;
		Value ^= Value >> 15;
		Value *= 0x846ca68bu;
		Value ^= Value >> 16;
		return Value;
	}

	/** Salts of the two values, so phase and rate of one object are independent. */
	constexpr uint32 PhaseSalt = 0x9e3779b9u;
	constexpr uint32 RateSalt = 0x68e31da4u;

	/** Top 24 bits of Hash as a float in [0, 1) (exact in single precision). */
	float ToUnit(uint32 Hash)
	{
		return static_cast<float>(Hash >> 8) * (1.f / 16777216.f);
	}
}

uint32 FRRPulseDesync::Hash(int32 Seed, uint32 Domain, uint32 Id, uint32 Salt)
{
	using namespace RRPulseDesyncPrivate;
	const uint32 SeedHash = Mix(static_cast<uint32>(Seed) * 0x85ebca6bu ^ Domain);
	return Mix(Mix(SeedHash ^ Salt) + Id * 0xc2b2ae35u);
}

float FRRPulseDesync::Phase(int32 Seed, uint32 Domain, uint32 Id)
{
	return RRPulseDesyncPrivate::ToUnit(Hash(Seed, Domain, Id, RRPulseDesyncPrivate::PhaseSalt));
}

float FRRPulseDesync::Rate(int32 Seed, uint32 Domain, uint32 Id, float RateMin, float RateMax)
{
	const float Low = FMath::Min(RateMin, RateMax);
	const float High = FMath::Max(RateMin, RateMax);
	// [0, 1] closed on both ends: (2^24 - 1) maps to exactly High.
	const float Alpha = static_cast<float>(Hash(Seed, Domain, Id, RRPulseDesyncPrivate::RateSalt) >> 8) * (1.f / 16777215.f);
	return FMath::Clamp(FMath::Lerp(Low, High, Alpha), Low, High);
}
