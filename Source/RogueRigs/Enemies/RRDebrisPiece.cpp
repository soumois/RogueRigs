// Rogue Rigs - robot death debris data (enemy-death, task-166-02, task-173-01): the 2-to-4 piece draw.

#include "Enemies/RRDebrisPiece.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRDebrisPiece)

void FRRDebrisSelection::Choose(const TArray<FRRDebrisPiece>& Pieces, int32 MeshCount, TArray<int32>& OutPieceIndices)
{
	OutPieceIndices.Reset();
	if (MeshCount <= 0 || Pieces.IsEmpty())
	{
		return;
	}

	// First mesh of each group and kind (INDEX_NONE = absent).
	constexpr int32 GroupCount = static_cast<int32>(ERRDebrisGroup::LegR) + 1;
	constexpr int32 KindCount = static_cast<int32>(ERRDebrisKind::Lower) + 1;
	int32 Slots[GroupCount][KindCount];
	for (int32 Group = 0; Group < GroupCount; ++Group)
	{
		for (int32 Kind = 0; Kind < KindCount; ++Kind)
		{
			Slots[Group][Kind] = INDEX_NONE;
		}
	}
	for (int32 Index = 0; Index < Pieces.Num(); ++Index)
	{
		const int32 Group = static_cast<int32>(Pieces[Index].Group);
		const int32 Kind = static_cast<int32>(Pieces[Index].Kind);
		if (Group >= 0 && Group < GroupCount && Kind >= 0 && Kind < KindCount && Slots[Group][Kind] == INDEX_NONE)
		{
			Slots[Group][Kind] = Index;
		}
	}

	const int32 WholeSlot = static_cast<int32>(ERRDebrisKind::Whole);
	const int32 UpperSlot = static_cast<int32>(ERRDebrisKind::Upper);
	const int32 LowerSlot = static_cast<int32>(ERRDebrisKind::Lower);

	// Each attempt is the plain draw: groups in random order (Fisher-Yates), a fair coin per limb, stop at MeshCount. A cut
	// that would exceed MeshCount voids the attempt and the whole draw is redrawn (task-173-01): forcing that limb whole, as
	// before, turned every last-slot cut into a whole limb and threw limbs cut about one time in three instead of two. The
	// last attempt keeps the old fallback (limb whole) so the draw always ends; with the chaser data an attempt succeeds more
	// than two times in three.
	constexpr int32 MaxAttempts = 16;
	TArray<int32, TInlineAllocator<GroupCount>> Groups;
	for (int32 Attempt = 0; Attempt < MaxAttempts; ++Attempt)
	{
		const bool bLastAttempt = Attempt == MaxAttempts - 1;
		OutPieceIndices.Reset();

		Groups.Reset();
		for (int32 Group = 0; Group < GroupCount; ++Group)
		{
			if (Slots[Group][WholeSlot] != INDEX_NONE || (Slots[Group][UpperSlot] != INDEX_NONE && Slots[Group][LowerSlot] != INDEX_NONE))
			{
				Groups.Add(Group);
			}
		}
		for (int32 Last = Groups.Num() - 1; Last > 0; --Last)
		{
			Groups.Swap(Last, FMath::RandRange(0, Last));
		}

		bool bOvershot = false;
		for (const int32 Group : Groups)
		{
			const int32 Remaining = MeshCount - OutPieceIndices.Num();
			if (Remaining <= 0)
			{
				break;
			}
			const bool bHasWhole = Slots[Group][WholeSlot] != INDEX_NONE;
			const bool bHasHalves = Slots[Group][UpperSlot] != INDEX_NONE && Slots[Group][LowerSlot] != INDEX_NONE;
			// One time in two when both are possible; a cut counts as 2 meshes and never exceeds MeshCount.
			bool bCut = bHasHalves && (!bHasWhole || FMath::RandBool());
			if (bCut && Remaining < 2)
			{
				if (!bLastAttempt)
				{
					bOvershot = true;
					break;
				}
				bCut = false;
			}
			if (bCut)
			{
				OutPieceIndices.Add(Slots[Group][UpperSlot]);
				OutPieceIndices.Add(Slots[Group][LowerSlot]);
			}
			else if (bHasWhole)
			{
				OutPieceIndices.Add(Slots[Group][WholeSlot]);
			}
		}
		if (!bOvershot)
		{
			return;
		}
	}
}
