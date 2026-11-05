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
}
