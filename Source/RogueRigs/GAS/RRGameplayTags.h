// Rogue Rigs - native gameplay tags shared by every system (naming: State., Effect.Damage. roots).

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
}
