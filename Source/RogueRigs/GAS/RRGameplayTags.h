// Rogue Rigs - native gameplay tags shared by every system (naming: State., Effect.Damage., Data. roots).

#pragma once

#include "NativeGameplayTags.h"

/** Native tags of the project. Declared here, defined in RRGameplayTags.cpp; usable in constructors. */
namespace RRGameplayTags
{
	/** Actor state: the actor is dead. Damage GEs and statuses refuse targets carrying it. */
	ROGUERIGS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Dead);

	/** Actor state: the actor ignores damage (i-frames, spawn protection). Damage GEs refuse targets carrying it. */
	ROGUERIGS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Invulnerable);

	/** Damage kind: set as a dynamic asset tag on the executed damage spec when the hit is critical. */
	ROGUERIGS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Damage_Critical);

	/** SetByCaller key: flat damage amount of URRGE_DamageFlat (before the source's AttackPower scaling). */
	ROGUERIGS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Damage);

	/** SetByCaller key: duration in seconds of a duration GE set by its caller (URRGE_IFrames). */
	ROGUERIGS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Duration);

	/** SetByCaller key: magnitude of a runtime upgrade GE (URRUpgradeComponent; AddBase value, or 1 + value for MultiplyAdditive). */
	ROGUERIGS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_UpgradeValue);

	/** SetByCaller key: health restored by URRGE_Heal (clamped to MaxHealth by the health set). */
	ROGUERIGS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Heal);
}
