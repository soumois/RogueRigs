// Rogue Rigs - in-memory layout of one level (cells, size, spawn) and the thread-safe parser of the layout JSON (level-layout-schema).

#pragma once

#include "CoreMinimal.h"
#include "Grid/RRCellType.h"

/**
 * One level layout in memory: what ARRArenaGrid builds from. Filled by a layout provider (Run/RRLayoutProvider.h) off the
 * game thread, or by the grid itself when it reads its own LayoutName. Plain data, no UObject: safe to build on a worker
 * thread and to share read-only (TSharedPtr<const FRRLayoutData>) once complete.
 */
struct ROGUERIGS_API FRRLayoutData
{
	/** Layout name (file name without extension, or the generator's name for a generated layout). */
	FString Name;

	/** Width in cells (0 when invalid). */
	int32 Width = 0;

	/** Height in cells (0 when invalid). */
	int32 Height = 0;

	/** Cell types, row-major: index = Y * Width + X; unlisted cells are Floor. */
	TArray<ERRCellType> Cells;

	/** The miner's start cell; (-1, -1) when the layout has none (logged as a warning). */
	FIntPoint SpawnCell = FIntPoint(-1, -1);

	/** Why the layout is invalid (file missing, parse error...); empty when valid. */
	FString Error;

	/** Where the cells come from, for the logs ("generator, level seed ..." or "hand json ..."); empty when read by the grid itself. */
	FString Source;

	/** True when the layout parsed: positive size and Width * Height cells. */
	bool IsValid() const { return Error.IsEmpty() && Width > 0 && Height > 0 && Cells.Num() == Width * Height; }
};

/** Layout file helpers (level-layout-schema). Every function is thread-safe: no UObject, no game-thread state. */
namespace RRLayout
{
	/** Absolute path of Content/RogueRigs/Data/Levels/<LayoutName>.json. */
	ROGUERIGS_API FString GetLayoutFilePath(const FString& LayoutName);

	/** Converts a layout type string ("rock", "heal_ore"...) to a cell type. Returns false for an unknown string. */
	ROGUERIGS_API bool ParseCellType(const FString& TypeName, ERRCellType& OutType);

	/**
	 * Reads and parses the layout file FilePath into OutLayout (Name set to LayoutName). Returns false on any error, with
	 * OutLayout.Error set (the message names the file); a missing spawn is only a logged warning.
	 */
	ROGUERIGS_API bool LoadLayoutFile(const FString& LayoutName, const FString& FilePath, FRRLayoutData& OutLayout);
}
