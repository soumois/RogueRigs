// Rogue Rigs - rectangular landing footprint of the console and the shuttle: the grid cells a mesh covers around its anchor cell.

#include "Objectives/RRLandingFootprint.h"

#include "Grid/RRArenaGrid.h"

namespace RRLandingFootprint
{
	FIntPoint ClampSize(FIntPoint Size)
	{
		return FIntPoint(FMath::Max(Size.X, 1), FMath::Max(Size.Y, 1));
	}

	FIntPoint GetMinCell(FIntPoint Anchor, FIntPoint Size)
	{
		const FIntPoint Clamped = ClampSize(Size);
		return FIntPoint(Anchor.X - (Clamped.X - 1) / 2, Anchor.Y - (Clamped.Y - 1) / 2);
	}

	void GetCells(FIntPoint Anchor, FIntPoint Size, TArray<FIntPoint>& OutCells)
	{
		const FIntPoint Clamped = ClampSize(Size);
		const FIntPoint Min = GetMinCell(Anchor, Clamped);
		OutCells.Reset(Clamped.X * Clamped.Y);
		for (int32 Y = Min.Y; Y < Min.Y + Clamped.Y; ++Y)
		{
			for (int32 X = Min.X; X < Min.X + Clamped.X; ++X)
			{
				OutCells.Add(FIntPoint(X, Y));
			}
		}
	}

	FVector GetCenter(const ARRArenaGrid* Grid, FIntPoint Anchor, FIntPoint Size)
	{
		if (!Grid)
		{
			return FVector::ZeroVector;
		}
		const FIntPoint Clamped = ClampSize(Size);
		const FIntPoint Min = GetMinCell(Anchor, Clamped);
		const FIntPoint Max = Min + Clamped - FIntPoint(1, 1);
		return 0.5 * (Grid->GetCellCenter(Min.X, Min.Y) + Grid->GetCellCenter(Max.X, Max.Y));
	}

	bool Fits(FIntPoint Anchor, FIntPoint Size, TFunctionRef<bool(FIntPoint Cell)> IsOpen)
	{
		const FIntPoint Clamped = ClampSize(Size);
		const FIntPoint Min = GetMinCell(Anchor, Clamped);
		for (int32 Y = Min.Y; Y < Min.Y + Clamped.Y; ++Y)
		{
			for (int32 X = Min.X; X < Min.X + Clamped.X; ++X)
			{
				if (!IsOpen(FIntPoint(X, Y)))
				{
					return false;
				}
			}
		}
		return true;
	}
}
