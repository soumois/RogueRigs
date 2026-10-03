// Rogue Rigs - native gameplay tags shared by every system (naming: State., Effect.Damage., Data. roots).

#include "GAS/RRGameplayTags.h"

namespace RRGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(State_Dead, "State.Dead");
	UE_DEFINE_GAMEPLAY_TAG(State_Invulnerable, "State.Invulnerable");
	UE_DEFINE_GAMEPLAY_TAG(Effect_Damage_Critical, "Effect.Damage.Critical");
	UE_DEFINE_GAMEPLAY_TAG(Data_Damage, "Data.Damage");
	UE_DEFINE_GAMEPLAY_TAG(Data_Duration, "Data.Duration");
	UE_DEFINE_GAMEPLAY_TAG(Data_UpgradeValue, "Data.UpgradeValue");
	UE_DEFINE_GAMEPLAY_TAG(Data_Heal, "Data.Heal");
}
