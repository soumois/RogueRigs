// Rogue Rigs - debris colour of a block type, shared by the drone's dig effects and the explosion wall debris (task-439-01).

#pragma once

#include "CoreMinimal.h"
#include "Grid/RRCellType.h"

/** Debris colours of the block types (color-code; values provisional). The single place these values are written. */
namespace RRBlockColors
{
	/** Rock: light grey tinted violet. */
	inline const FLinearColor Rock(0.35f, 0.28f, 0.5f, 1.f);
	/** Duskite ore (Ore and SpecialOre): glowing violet. */
	inline const FLinearColor Ore(0.6f, 0.15f, 1.f, 1.f);
	/** Heal ore: red. */
	inline const FLinearColor HealOre(0.9f, 0.06f, 0.05f, 1.f);
	/** Barrel: orange. */
	inline const FLinearColor Barrel(1.f, 0.35f, 0.05f, 1.f);

	/** Colour of Type; any other type gives Rock. */
	inline FLinearColor ForType(ERRCellType Type)
	{
		switch (Type)
		{
		case ERRCellType::Ore:
		case ERRCellType::SpecialOre:
			return Ore;
		case ERRCellType::HealOre:
			return HealOre;
		case ERRCellType::Barrel:
			return Barrel;
		default:
			return Rock;
		}
	}

	/**
	 * Debris tones of a broken rock block (color-code "Rock destruction debris", owner 2026-11-13, task-543; values provisional):
	 * lilac like the rock top, then mid and dark violet like its sides. Unlit sprites (M_DigChip): kept low so they never read as
	 * near white and never reach bloom. Index 0 is the lilac top tone.
	 */
	inline const FLinearColor RockDebrisTones[3] = {
		FLinearColor(0.12f, 0.075f, 0.2f, 1.f),
		FLinearColor(0.06f, 0.015f, 0.15f, 1.f),
		FLinearColor(0.025f, 0.006f, 0.06f, 1.f),
	};

	/** Debris tone Tone (0..2, wrapped) of a block of Type: a rock tone for rock (and any type ForType maps to Rock), else ForType(Type). */
	inline FLinearColor DebrisToneForType(ERRCellType Type, int32 Tone)
	{
		switch (Type)
		{
		case ERRCellType::Ore:
		case ERRCellType::SpecialOre:
		case ERRCellType::HealOre:
		case ERRCellType::Barrel:
			return ForType(Type);
		default:
			return RockDebrisTones[((Tone % 3) + 3) % 3];
		}
	}

	/** True when Color is one of the debris tones of a block of Type. */
	inline bool IsDebrisToneOf(const FLinearColor& Color, ERRCellType Type, float Tolerance = UE_KINDA_SMALL_NUMBER)
	{
		for (int32 Tone = 0; Tone < 3; ++Tone)
		{
			if (Color.Equals(DebrisToneForType(Type, Tone), Tolerance))
			{
				return true;
			}
		}
		return false;
	}
}
