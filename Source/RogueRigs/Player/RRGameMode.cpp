// Rogue Rigs - game mode of a run: game over on the miner's death (the run-lost screen follows, task-484).

#include "Player/RRGameMode.h"

#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Player/RRMiner.h"
#include "RogueRigs.h"

void ARRGameMode::RegisterMiner(ARRMiner* Miner)
{
	if (Miner)
	{
		Miner->OnDeath.AddUniqueDynamic(this, &ARRGameMode::HandleMinerDeath);
	}
}

void ARRGameMode::HandleMinerDeath(AActor* DeathInstigator)
{
	if (bGameOver)
	{
		return;
	}
	bGameOver = true;

	// Controls cut (death-screen gdd, Death sequence); the run-lost screen's buttons are UMG and still take clicks and keys.
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (APlayerController* PlayerController = It->Get())
		{
			PlayerController->DisableInput(PlayerController);
		}
	}

	UE_LOG(LogRogueRigs, Log, TEXT("%s: game over (killed by %s): death sequence, then the run-lost screen."), *GetName(), *GetNameSafe(DeathInstigator));
	OnGameOver.Broadcast();
}
