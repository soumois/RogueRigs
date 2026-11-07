// Rogue Rigs - run-lost debug console commands (compiled out of shipping, task-484): rr.Run.Lost.Die (kills the player miner through
// the damage path: death sequence, then the run-lost screen), rr.Run.Lost.Shuttle (the shuttle leaves without the miner), rr.Run.Lost.Retry
// and rr.Run.Lost.MainMenu (the two buttons of the run-lost screen).

#include "CoreMinimal.h"

#if !UE_BUILD_SHIPPING

#include "AbilitySystemComponent.h"
#include "Damage/RRGE_DamageFlat.h"
#include "Debug/RRCheats.h"
#include "Engine/World.h"
#include "GAS/RRGameplayTags.h"
#include "GAS/RRHealthAttributeSet.h"
#include "HAL/IConsoleManager.h"
#include "Objectives/RRRunFlowSubsystem.h"
#include "Player/RRMiner.h"
#include "RogueRigs.h"
#include "Run/RRRunLostSubsystem.h"

namespace RRRunLostDebug
{
	/** rr.Run.Lost.Die: a flat damage of the miner's Health + 1 from the miner's own ability system (i-frames and the invulnerable cheat refuse it). */
	FAutoConsoleCommand DieCommand(
		TEXT("rr.Run.Lost.Die"),
		TEXT("rr.Run.Lost.Die: kills the player miner through the damage path (death sequence, then the run-lost screen)."),
		FConsoleCommandDelegate::CreateLambda([]()
		{
			ARRMiner* Miner = RRCheats::FindPlayerMiner(RRCheats::FindCheatWorld());
			UAbilitySystemComponent* ASC = Miner ? Miner->GetAbilitySystemComponent() : nullptr;
			if (!ASC || Miner->IsDead())
			{
				UE_LOG(LogRogueRigs, Warning, TEXT("rr.Run.Lost.Die: no living player miner."));
				return;
			}
			const float Health = ASC->GetNumericAttribute(URRHealthAttributeSet::GetHealthAttribute());
			FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(URRGE_DamageFlat::StaticClass(), 1.f, ASC->MakeEffectContext());
			Spec.Data->SetSetByCallerMagnitude(RRGameplayTags::Data_Damage, Health + 1.f);
			ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data);
			UE_LOG(LogRogueRigs, Log, TEXT("rr.Run.Lost.Die: %.1f damage applied, miner %s."), Health + 1.f, Miner->IsDead() ? TEXT("dead") : TEXT("alive (i-frames or invulnerable)"));
		}),
		ECVF_Cheat);

	/** rr.Run.Lost.Shuttle: URRRunFlowSubsystem::EndRun(false), the missed shuttle end (run-lost screen at once). */
	FAutoConsoleCommand ShuttleCommand(
		TEXT("rr.Run.Lost.Shuttle"),
		TEXT("rr.Run.Lost.Shuttle: ends the level as if the shuttle left without the miner (run-lost screen, missed shuttle texts)."),
		FConsoleCommandDelegate::CreateLambda([]()
		{
			UWorld* World = RRCheats::FindCheatWorld();
			URRRunFlowSubsystem* RunFlow = World ? World->GetSubsystem<URRRunFlowSubsystem>() : nullptr;
			UE_LOG(LogRogueRigs, Log, TEXT("rr.Run.Lost.Shuttle: %s."), RunFlow && RunFlow->EndRun(false) ? TEXT("run ended, shuttle missed") : TEXT("refused"));
		}),
		ECVF_Cheat);

	/** rr.Run.Lost.Retry: the run-lost screen Retry button. */
	FAutoConsoleCommand RetryCommand(
		TEXT("rr.Run.Lost.Retry"),
		TEXT("rr.Run.Lost.Retry: presses the run-lost screen Retry button (new run at level 1 through the loading screen)."),
		FConsoleCommandDelegate::CreateLambda([]()
		{
			URRRunLostSubsystem* RunLost = URRRunLostSubsystem::Get(RRCheats::FindCheatWorld());
			UE_LOG(LogRogueRigs, Log, TEXT("rr.Run.Lost.Retry: %s."), RunLost && RunLost->Retry() ? TEXT("new run requested") : TEXT("refused"));
		}),
		ECVF_Cheat);

	/** rr.Run.Lost.MainMenu: the run-lost screen Main Menu button. */
	FAutoConsoleCommand MainMenuCommand(
		TEXT("rr.Run.Lost.MainMenu"),
		TEXT("rr.Run.Lost.MainMenu: presses the run-lost screen Main Menu button (run reset, main menu map)."),
		FConsoleCommandDelegate::CreateLambda([]()
		{
			URRRunLostSubsystem* RunLost = URRRunLostSubsystem::Get(RRCheats::FindCheatWorld());
			UE_LOG(LogRogueRigs, Log, TEXT("rr.Run.Lost.MainMenu: %s."), RunLost && RunLost->ReturnToMainMenu() ? TEXT("main menu requested") : TEXT("refused"));
		}),
		ECVF_Cheat);
}

#endif // !UE_BUILD_SHIPPING
