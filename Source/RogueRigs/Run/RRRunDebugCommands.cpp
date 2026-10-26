// Rogue Rigs - run debug console commands (compiled out of shipping): rr.Run.Status (logs the run state), rr.Run.Stats.Next (the
// stats screen Next level button, task-317-01), rr.Run.Continue (the shop screen Continue button), rr.Run.NextLevel (successful
// extraction, Next level, Continue), rr.Run.EndLevel (successful extraction, the stats screen stays open), rr.Run.Restart (the end
// screen restart button: run reset, level 1), rr.Shop.Buy and rr.Shop.Reroll (the shop card and reroll buttons, task-316-05).

#include "CoreMinimal.h"

#if !UE_BUILD_SHIPPING

#include "Debug/RRCheats.h"
#include "Engine/World.h"
#include "HAL/IConsoleManager.h"
#include "Objectives/RRRunFlowSubsystem.h"
#include "RogueRigs.h"
#include "Run/RRIntermissionShopSubsystem.h"
#include "Run/RRRunSubsystem.h"
#include "Run/RRRunTransitionSubsystem.h"

namespace RRRunDebug
{
	/** Display name of a transition phase for the logs. */
	const TCHAR* PhaseName(ERRRunTransitionPhase Phase)
	{
		switch (Phase)
		{
		case ERRRunTransitionPhase::Stats: return TEXT("stats");
		case ERRRunTransitionPhase::Shop: return TEXT("shop");
		case ERRRunTransitionPhase::Loading: return TEXT("loading");
		default: return TEXT("none");
		}
	}

	/** rr.Run.Status: logs level index, layout, last-level flag, pending snapshot, run ore and transition phase. */
	FAutoConsoleCommand StatusCommand(
		TEXT("rr.Run.Status"),
		TEXT("rr.Run.Status: logs the run state (level index, layout, last level, pending snapshot, run secured ore, transition phase)."),
		FConsoleCommandDelegate::CreateLambda([]()
		{
			UWorld* World = RRCheats::FindCheatWorld();
			const URRRunSubsystem* Run = URRRunSubsystem::Get(World);
			const URRRunTransitionSubsystem* Transition = URRRunTransitionSubsystem::Get(World);
			if (!Run)
			{
				UE_LOG(LogRogueRigs, Warning, TEXT("rr.Run.Status: no game instance run subsystem."));
				return;
			}
			UE_LOG(LogRogueRigs, Log, TEXT("rr.Run.Status: run %s, level %d/%d, layout %s, last level %d, snapshot pending %d, run secured ore %d, transition %s, run won %d."),
				Run->IsRunInProgress() ? TEXT("in progress") : TEXT("none"), Run->GetLevelIndex(), Run->GetLevelCount(), *Run->GetCurrentLayoutName(),
				Run->IsLastLevel() ? 1 : 0, Run->HasPendingSnapshot() ? 1 : 0, Run->GetRunSecuredOre(),
				Transition ? PhaseName(Transition->GetPhase()) : TEXT("no subsystem"), Transition && Transition->IsRunWon() ? 1 : 0);
		}),
		ECVF_Cheat);

	/** rr.Run.Stats.Next: the stats screen Next level button (URRRunTransitionSubsystem::NextLevel: shop screen, or run won end screen). */
	FAutoConsoleCommand StatsNextCommand(
		TEXT("rr.Run.Stats.Next"),
		TEXT("rr.Run.Stats.Next: presses the stats screen Next level button (cards shop screen, or the run won end screen on the last level)."),
		FConsoleCommandDelegate::CreateLambda([]()
		{
			URRRunTransitionSubsystem* Transition = URRRunTransitionSubsystem::Get(RRCheats::FindCheatWorld());
			const bool bAccepted = Transition && Transition->NextLevel();
			UE_LOG(LogRogueRigs, Log, TEXT("rr.Run.Stats.Next: %s."), bAccepted ? TEXT("accepted") : TEXT("refused (not on the stats screen)"));
		}),
		ECVF_Cheat);

	/** rr.Run.Continue: the shop screen Continue button (URRRunTransitionSubsystem::Continue; refused outside the shop screen). */
	FAutoConsoleCommand ContinueCommand(
		TEXT("rr.Run.Continue"),
		TEXT("rr.Run.Continue: presses the shop screen Continue button (loading screen, then the next level)."),
		FConsoleCommandDelegate::CreateLambda([]()
		{
			URRRunTransitionSubsystem* Transition = URRRunTransitionSubsystem::Get(RRCheats::FindCheatWorld());
			const bool bAccepted = Transition && Transition->Continue();
			UE_LOG(LogRogueRigs, Log, TEXT("rr.Run.Continue: %s."), bAccepted ? TEXT("accepted") : TEXT("refused (not on the shop screen)"));
		}),
		ECVF_Cheat);

	/**
	 * rr.Run.NextLevel: EndRun(true) when the level is still running (as a boarded shuttle: ore secured, stats screen), then the
	 * stats screen Next level button (levels 1 to N-1: shop screen, then Continue; last level: run won end screen).
	 */
	FAutoConsoleCommand NextLevelCommand(
		TEXT("rr.Run.NextLevel"),
		TEXT("rr.Run.NextLevel: ends the level as a successful extraction, presses Next level on the stats screen and Continue on the shop screen (run won end screen on the last level)."),
		FConsoleCommandDelegate::CreateLambda([]()
		{
			UWorld* World = RRCheats::FindCheatWorld();
			URRRunFlowSubsystem* RunFlow = World ? World->GetSubsystem<URRRunFlowSubsystem>() : nullptr;
			URRRunTransitionSubsystem* Transition = URRRunTransitionSubsystem::Get(World);
			if (!RunFlow || !Transition)
			{
				UE_LOG(LogRogueRigs, Warning, TEXT("rr.Run.NextLevel: refused (no run flow or no transition subsystem)."));
				return;
			}
			if (!RunFlow->IsRunEnded())
			{
				RunFlow->EndRun(true);
			}
			if (Transition->IsInStatsScreen())
			{
				Transition->NextLevel();
			}
			if (Transition->IsInShopScreen())
			{
				Transition->Continue();
			}
			else if (!Transition->IsLevelLoading())
			{
				UE_LOG(LogRogueRigs, Log, TEXT("rr.Run.NextLevel: no next level (%s)."), Transition->IsRunWon() ? TEXT("run won") : TEXT("level not extracted"));
			}
		}),
		ECVF_Cheat);

	/** rr.Run.EndLevel: EndRun(true) when the level is still running (as a boarded shuttle), without any button: the stats screen stays open. */
	FAutoConsoleCommand EndLevelCommand(
		TEXT("rr.Run.EndLevel"),
		TEXT("rr.Run.EndLevel: ends the level as a successful extraction and leaves the stats screen open (rr.Run.Stats.Next goes on)."),
		FConsoleCommandDelegate::CreateLambda([]()
		{
			UWorld* World = RRCheats::FindCheatWorld();
			URRRunFlowSubsystem* RunFlow = World ? World->GetSubsystem<URRRunFlowSubsystem>() : nullptr;
			if (!RunFlow || RunFlow->IsRunEnded())
			{
				UE_LOG(LogRogueRigs, Warning, TEXT("rr.Run.EndLevel: refused (no run flow or the level already ended)."));
				return;
			}
			RunFlow->EndRun(true);
			const URRRunTransitionSubsystem* Transition = URRRunTransitionSubsystem::Get(World);
			UE_LOG(LogRogueRigs, Log, TEXT("rr.Run.EndLevel: level extracted, transition %s."), Transition ? PhaseName(Transition->GetPhase()) : TEXT("no subsystem"));
		}),
		ECVF_Cheat);

	/** rr.Shop.Buy <Slot>: the intermission shop card button of slot Slot (0-based, URRIntermissionShopSubsystem::BuySlot). */
	FAutoConsoleCommand ShopBuyCommand(
		TEXT("rr.Shop.Buy"),
		TEXT("rr.Shop.Buy <Slot>: buys the intermission shop card of slot Slot (0-based), like its card button."),
		FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
		{
			URRIntermissionShopSubsystem* Shop = URRIntermissionShopSubsystem::Get(RRCheats::FindCheatWorld());
			if (!Shop || Args.Num() < 1)
			{
				UE_LOG(LogRogueRigs, Warning, TEXT("rr.Shop.Buy: refused (%s)."), Shop ? TEXT("usage: rr.Shop.Buy <Slot>") : TEXT("no shop subsystem"));
				return;
			}
			const bool bBought = Shop->BuySlot(FCString::Atoi(*Args[0]));
			UE_LOG(LogRogueRigs, Log, TEXT("rr.Shop.Buy %s: %s, balance %d."), *Args[0], bBought ? TEXT("bought") : TEXT("refused"), Shop->GetOreBalance());
		}),
		ECVF_Cheat);

	/** rr.Shop.Reroll: the intermission shop reroll button (URRIntermissionShopSubsystem::RequestReroll). */
	FAutoConsoleCommand ShopRerollCommand(
		TEXT("rr.Shop.Reroll"),
		TEXT("rr.Shop.Reroll: pays and rerolls the intermission shop, like its reroll button."),
		FConsoleCommandDelegate::CreateLambda([]()
		{
			URRIntermissionShopSubsystem* Shop = URRIntermissionShopSubsystem::Get(RRCheats::FindCheatWorld());
			const bool bRerolled = Shop && Shop->RequestReroll();
			UE_LOG(LogRogueRigs, Log, TEXT("rr.Shop.Reroll: %s, balance %d."), bRerolled ? TEXT("paid") : TEXT("refused"), Shop ? Shop->GetOreBalance() : 0);
		}),
		ECVF_Cheat);

	/** rr.Run.Restart: the end screen restart button (URRRunFlowSubsystem::RestartLevel: run reset, map reopened at level 1). */
	FAutoConsoleCommand RestartCommand(
		TEXT("rr.Run.Restart"),
		TEXT("rr.Run.Restart: presses the end screen restart button (run reset, the map reopens at level 1)."),
		FConsoleCommandDelegate::CreateLambda([]()
		{
			UWorld* World = RRCheats::FindCheatWorld();
			if (URRRunFlowSubsystem* RunFlow = World ? World->GetSubsystem<URRRunFlowSubsystem>() : nullptr)
			{
				RunFlow->RestartLevel();
			}
		}),
		ECVF_Cheat);
}

#endif
