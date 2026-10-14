// Rogue Rigs - robot death debris data (enemy-death, task-166-02, task-173-01): one entry per debris mesh of a robot and the 2-to-4 piece draw.

#pragma once

#include "CoreMinimal.h"
#include "RRDebrisPiece.generated.h"

/** Body group a debris mesh belongs to; one draw takes each group at most once. */
UENUM(BlueprintType)
enum class ERRDebrisGroup : uint8
{
	/** Head, torso and pelvis as one block. */
	Torso,
	/** Left arm (robot's left). */
	ArmL,
	/** Right arm. */
	ArmR,
	/** Left leg. */
	LegL,
	/** Right leg. */
	LegR
};

/** Which part of its group a debris mesh is: the whole group, or one half of a limb cut at the elbow or knee. */
UENUM(BlueprintType)
enum class ERRDebrisKind : uint8
{
	/** The whole group in one mesh (torso block, whole arm, whole leg). */
	Whole,
	/** Upper half of a cut limb (upper arm, thigh). */
	Upper,
	/** Lower half of a cut limb (forearm, lower leg). */
	Lower
};

/**
 * One debris mesh a robot can throw on death (enemy-death). Set per robot Blueprint on ARREnemy::DebrisPieces (chaser: the 13
 * pieces of Art/Source/chaser-meshy/debris/debris_pieces.json, in their report order).
 */
USTRUCT(BlueprintType)
struct ROGUERIGS_API FRRDebrisPiece
{
	GENERATED_BODY()

	/** Body group of this mesh. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debris")
	ERRDebrisGroup Group = ERRDebrisGroup::Torso;

	/** Whole group, or upper / lower half of a cut limb. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debris")
	ERRDebrisKind Kind = ERRDebrisKind::Whole;

	/** Index of this mesh in the mesh renderer of the debris Niagara system (User.PieceMeshIndex value). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debris", meta = (ClampMin = 0))
	int32 MeshIndex = 0;

	/**
	 * Rest position of the mesh pivot in robot space, cm, from the floor point under the robot (front +X, Unreal axes): the
	 * report's centre_offset_cm_unreal. Rotated by the robot yaw and added to its feet at launch.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debris", meta = (Units = "cm"))
	FVector Offset = FVector::ZeroVector;
};

/** The random draw of the debris meshes thrown at a robot death. */
struct ROGUERIGS_API FRRDebrisSelection
{
	/**
	 * Draws the meshes thrown by one death into OutPieceIndices (indices into Pieces): MeshCount meshes at most (the caller draws
	 * 2 to 4). Groups are taken in random order, each at most once; a group with a Whole mesh and both halves is cut one time in
	 * two (2 meshes: Upper then Lower), otherwise thrown as it can be (whole, or cut when it only has halves). A cut that would
	 * exceed MeshCount voids the draw, which is redrawn from scratch (at most 16 attempts; the last one throws that limb whole,
	 * or skips it when it has no whole mesh). Fewer meshes when the data runs out. The first mesh of each group / kind in Pieces
	 * is used.
	 */
	static void Choose(const TArray<FRRDebrisPiece>& Pieces, int32 MeshCount, TArray<int32>& OutPieceIndices);
};
