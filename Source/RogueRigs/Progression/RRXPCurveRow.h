// Rogue Rigs - DataTable row of the XP curve (XPCurve.csv): XP needed to leave one level.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "RRXPCurveRow.generated.h"

/**
 * One row of Content/RogueRigs/Data/Tables/XPCurve.csv. The row name is the level (1, 2, 3...); XPToNextLevel is the XP
 * needed to go from that level to the next. Read once by URRProgressionComponent; levels above the last row reuse the
 * last row's value.
 */
USTRUCT(BlueprintType)
struct ROGUERIGS_API FRRXPCurveRow : public FTableRowBase
{
	GENERATED_BODY()

	/** XP needed to go from this row's level to the next one (> 0). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "XP Curve")
	float XPToNextLevel = 0.f;
};
