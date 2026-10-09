// Rogue Rigs - minimap data: a one-pixel-per-cell texture of the seen terrain, updated only on change, plus the marker positions.

#include "UI/RRMinimap.h"

#include "Engine/Texture2D.h"
#include "GameFramework/Actor.h"
#include "Objectives/RRBonusChest.h"
#include "Objectives/RRConsole.h"
#include "Objectives/RRRunFlowSubsystem.h"
#include "Objectives/RRShuttle.h"
#include "Player/RRMiner.h"

bool URRMinimap::Initialize(ARRArenaGrid* InGrid, const FRRMinimapConfig& InConfig)
{
	Shutdown();
	if (!InGrid || InGrid->GetGridSize().X <= 0 || InGrid->GetGridSize().Y <= 0)
	{
		return false;
	}

	Grid = InGrid;
	Config = InConfig;
	GridSize = InGrid->GetGridSize();

	// Texture: width = cells along world Y, height = cells along world X (screen-aligned for camera yaw 0).
	const int32 TextureWidth = GridSize.Y;
	const int32 TextureHeight = GridSize.X;
	Pixels.Init(Config.UnseenColor, TextureWidth * TextureHeight);
	SeenCells.Init(false, GridSize.X * GridSize.Y);
	DirtyMin = DirtyMax = FIntPoint::ZeroValue;
	LastSeenCell = FIntPoint(INDEX_NONE, INDEX_NONE);

	const TConstArrayView64<uint8> InitialData(reinterpret_cast<const uint8*>(Pixels.GetData()), static_cast<int64>(Pixels.Num()) * sizeof(FColor));
	Texture = UTexture2D::CreateTransient(TextureWidth, TextureHeight, PF_B8G8R8A8, MakeUniqueObjectName(this, UTexture2D::StaticClass(), TEXT("MinimapTexture")), InitialData);
	if (!Texture)
	{
		Grid.Reset();
		return false;
	}
	Texture->Filter = TF_Nearest;
	Texture->SRGB = true;
	Texture->AddressX = TA_Clamp;
	Texture->AddressY = TA_Clamp;
	Texture->NeverStream = true;
	Texture->UpdateResource();

	InGrid->OnCellDestroyed.AddDynamic(this, &URRMinimap::HandleCellDestroyed);
	return true;
}

void URRMinimap::Shutdown()
{
	if (ARRArenaGrid* BoundGrid = Grid.Get())
	{
		BoundGrid->OnCellDestroyed.RemoveDynamic(this, &URRMinimap::HandleCellDestroyed);
	}
	Grid.Reset();
}

void URRMinimap::Tick(const AActor* RevealCenter)
{
	if (!Texture || !Grid.IsValid())
	{
		return;
	}
	UpdateSeenCells(RevealCenter);
	FlushDirtyPixels();
}

bool URRMinimap::GetState(const ARRMiner* Miner, const URRRunFlowSubsystem* RunFlow, FRRMinimapState& OutState) const
{
	OutState = FRRMinimapState();
	if (!Texture || !Grid.IsValid())
	{
		return false;
	}

	OutState.TextureSize = FIntPoint(GridSize.Y, GridSize.X);
	const FVector2D Size(OutState.TextureSize);
	OutState.ViewRadiusCells = Config.ViewRadiusCells;
	OutState.ViewRadiusUV = FVector2D(Config.ViewRadiusCells) / Size;
	if (Miner)
	{
		OutState.MinerPosition = WorldToTexture(Miner->GetActorLocation());
		OutState.MinerUV = OutState.MinerPosition / Size;
	}
	if (RunFlow)
	{
		if (const ARRConsole* Console = RunFlow->GetConsole())
		{
			OutState.bHasConsole = true;
			OutState.ConsolePosition = WorldToTexture(Console->GetActorLocation());
		}
		if (const ARRBonusChest* BonusChest = RunFlow->GetBonusChest())
		{
			OutState.bHasBonusChest = true;
			OutState.BonusChestPosition = WorldToTexture(BonusChest->GetActorLocation());
		}
		if (const ARRShuttle* Shuttle = RunFlow->GetShuttle())
		{
			OutState.bHasShuttle = true;
			OutState.ShuttlePosition = WorldToTexture(Shuttle->GetActorLocation());
		}
	}
	return true;
}

FVector2D URRMinimap::WorldToTexture(const FVector& WorldLocation) const
{
	const ARRArenaGrid* BoundGrid = Grid.Get();
	if (!BoundGrid || BoundGrid->GetCellSize() <= 0.f)
	{
		return FVector2D::ZeroVector;
	}
	const FVector Local = BoundGrid->GetActorTransform().InverseTransformPosition(WorldLocation) / BoundGrid->GetCellSize();
	return FVector2D(Local.Y, static_cast<double>(GridSize.X) - Local.X);
}

bool URRMinimap::IsCellSeen(FIntPoint Cell) const
{
	if (Cell.X < 0 || Cell.Y < 0 || Cell.X >= GridSize.X || Cell.Y >= GridSize.Y)
	{
		return false;
	}
	return SeenCells[Cell.Y * GridSize.X + Cell.X];
}

void URRMinimap::MarkCellSeen(FIntPoint Cell)
{
	const ARRArenaGrid* BoundGrid = Grid.Get();
	if (!BoundGrid || Cell.X < 0 || Cell.Y < 0 || Cell.X >= GridSize.X || Cell.Y >= GridSize.Y)
	{
		return;
	}
	FBitReference Seen = SeenCells[Cell.Y * GridSize.X + Cell.X];
	if (Seen)
	{
		return;
	}
	Seen = true;
	WritePixel(Cell, GetCellColor(BoundGrid->GetCellType(Cell)));
}

void URRMinimap::HandleCellDestroyed(FIntPoint Cell, ERRCellType Type, ERRTerrainDamageSource Source)
{
	const ARRArenaGrid* BoundGrid = Grid.Get();
	if (BoundGrid && IsCellSeen(Cell))
	{
		WritePixel(Cell, GetCellColor(BoundGrid->GetCellType(Cell)));
	}
}

FColor URRMinimap::GetCellColor(ERRCellType Type) const
{
	switch (Type)
	{
	case ERRCellType::Floor:
	case ERRCellType::Spawn:
		return Config.FloorColor;
	case ERRCellType::Void:
		return Config.VoidColor;
	case ERRCellType::HealOre:
		return Config.HealVeinColor;
	default:
		return Config.RockColor;
	}
}

int32 URRMinimap::GetPixelIndex(FIntPoint Cell) const
{
	if (Cell.X < 0 || Cell.Y < 0 || Cell.X >= GridSize.X || Cell.Y >= GridSize.Y)
	{
		return INDEX_NONE;
	}
	const int32 U = Cell.Y;
	const int32 V = GridSize.X - 1 - Cell.X;
	return V * GridSize.Y + U;
}

void URRMinimap::WritePixel(FIntPoint Cell, const FColor& Color)
{
	const int32 Index = GetPixelIndex(Cell);
	if (Index == INDEX_NONE || Pixels[Index] == Color)
	{
		return;
	}
	Pixels[Index] = Color;

	const FIntPoint Pixel(Cell.Y, GridSize.X - 1 - Cell.X);
	if (DirtyMin.X >= DirtyMax.X || DirtyMin.Y >= DirtyMax.Y)
	{
		DirtyMin = Pixel;
		DirtyMax = Pixel + FIntPoint(1, 1);
	}
	else
	{
		DirtyMin = DirtyMin.ComponentMin(Pixel);
		DirtyMax = DirtyMax.ComponentMax(Pixel + FIntPoint(1, 1));
	}
}

void URRMinimap::UpdateSeenCells(const AActor* RevealCenter)
{
	const ARRArenaGrid* BoundGrid = Grid.Get();
	if (!RevealCenter || !BoundGrid)
	{
		return;
	}
	const FIntPoint Center = BoundGrid->GetCellAtWorld(RevealCenter->GetActorLocation());
	if (Center == LastSeenCell)
	{
		return;
	}
	LastSeenCell = Center;

	// Disc of cell centers within the radius of the center cell's center: per row, the half-width is floor(sqrt(R^2 - dY^2)),
	// so the pass is about (2R + 1)^2 * pi / 4 bit tests, only when the miner enters another cell.
	const float Radius = FMath::Max(0.f, Config.RevealRadiusCells);
	const int32 RadiusCells = FMath::FloorToInt32(Radius);
	const float RadiusSquared = Radius * Radius;
	const int32 MinY = FMath::Max(0, Center.Y - RadiusCells);
	const int32 MaxY = FMath::Min(GridSize.Y - 1, Center.Y + RadiusCells);
	for (int32 Y = MinY; Y <= MaxY; ++Y)
	{
		const int32 DeltaY = Y - Center.Y;
		const int32 HalfWidth = FMath::FloorToInt32(FMath::Sqrt(FMath::Max(0.f, RadiusSquared - static_cast<float>(DeltaY * DeltaY))));
		const int32 MinX = FMath::Max(0, Center.X - HalfWidth);
		const int32 MaxX = FMath::Min(GridSize.X - 1, Center.X + HalfWidth);
		for (int32 X = MinX; X <= MaxX; ++X)
		{
			if (!SeenCells[Y * GridSize.X + X])
			{
				MarkCellSeen(FIntPoint(X, Y));
			}
		}
	}
}

void URRMinimap::FlushDirtyPixels()
{
	if (DirtyMin.X >= DirtyMax.X || DirtyMin.Y >= DirtyMax.Y || !Texture || !Texture->GetResource())
	{
		return;
	}

	// Copy the dirty rectangle: the render thread reads it later, while Pixels may change next frame. Without a resource the
	// rectangle stays dirty (UpdateTextureRegions would skip the cleanup and leak the copy).
	const int32 TextureWidth = GridSize.Y;
	const int32 Width = DirtyMax.X - DirtyMin.X;
	const int32 Height = DirtyMax.Y - DirtyMin.Y;
	FColor* Data = new FColor[Width * Height];
	for (int32 Row = 0; Row < Height; ++Row)
	{
		FMemory::Memcpy(Data + Row * Width, Pixels.GetData() + (DirtyMin.Y + Row) * TextureWidth + DirtyMin.X, Width * sizeof(FColor));
	}
	FUpdateTextureRegion2D* Region = new FUpdateTextureRegion2D(DirtyMin.X, DirtyMin.Y, 0, 0, Width, Height);
	Texture->UpdateTextureRegions(0, 1, Region, Width * sizeof(FColor), sizeof(FColor), reinterpret_cast<uint8*>(Data),
		[](uint8* SrcData, const FUpdateTextureRegion2D* Regions)
		{
			delete[] reinterpret_cast<FColor*>(SrcData);
			delete Regions;
		});
	DirtyMin = DirtyMax = FIntPoint::ZeroValue;
}
