// Rogue Rigs - game mode of a run: game over on the miner's death, then level restart.

#include "Player/RRGameMode.h"

#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Player/RRMiner.h"
#include "RogueRigs.h"
#include "TimerManager.h"

void ARRGameMode::RegisterMiner(ARRMiner* Miner)
{
	if (Miner)
	{
		Miner->OnDeath.AddUniqueDynamic(this, &ARRGameMode::HandleMinerDeath);
	}
}

float ARRGameMode::GetRestartTimeRemaining() const
{
	const FTimerManager& TimerManager = GetWorldTimerManager();
	return TimerManager.IsTimerActive(RestartTimerHandle) ? TimerManager.GetTimerRemaining(RestartTimerHandle) : -1.f;
}

void ARRGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(RestartTimerHandle);
	Super::EndPlay(EndPlayReason);
}

void ARRGameMode::HandleMinerDeath(AActor* DeathInstigator)
{
	if (bGameOver)
	{
		return;
	}
	bGameOver = true;

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (APlayerController* PlayerController = It->Get())
		{
			PlayerController->DisableInput(PlayerController);
		}
	}

	UE_LOG(LogRogueRigs, Log, TEXT("%s: game over (killed by %s), restart in %.2f s."), *GetName(), *GetNameSafe(DeathInstigator), RestartDelay);
	OnGameOver.Broadcast();

	GetWorldTimerManager().SetTimer(RestartTimerHandle, this, &ARRGameMode::RestartLevel, FMath::Max(RestartDelay, 0.1f), false);
}

void ARRGameMode::RestartLevel()
{
	const FString LevelName = UGameplayStatics::GetCurrentLevelName(this, true);
	UE_LOG(LogRogueRigs, Log, TEXT("%s: restarting level %s."), *GetName(), *LevelName);
	UGameplayStatics::OpenLevel(this, FName(*LevelName));
}
