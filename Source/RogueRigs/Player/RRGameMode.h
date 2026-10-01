// Rogue Rigs - game mode of a run.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RRGameMode.generated.h"

/**
 * Game mode of a run. DefaultPawnClass (Miner Blueprint) and PlayerControllerClass (player controller
 * Blueprint) are set in the Blueprint subclass, because ARRMiner and ARRPlayerController are abstract
 * and need their assets. Run flow (start, death, end) comes in a later task.
 */
UCLASS(Abstract)
class ROGUERIGS_API ARRGameMode : public AGameModeBase
{
	GENERATED_BODY()
};
