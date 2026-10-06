// Rogue Rigs - cheat actions and the rr.Cheat.* console variables with on-change sinks; the same actions serve the polled URRCheatSettings route.

#include "Debug/RRCheats.h"

#include "AbilitySystemComponent.h"
#include "Debug/RRCheatSettings.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GAS/RRGameplayTags.h"
#include "HAL/IConsoleManager.h"
#include "Kismet/GameplayStatics.h"
#include "Objectives/RRRunFlowSubsystem.h"
#include "Player/RRMiner.h"
#include "Progression/RRLevelUpComponent.h"
#include "Progression/RRProgressionComponent.h"
#include "RogueRigs.h"
#include "World/RRWorld.h"

namespace RRCheats
{
	UWorld* FindCheatWorld()
	{
		if (!GEngine)
		{
			return nullptr;
		}
		UWorld* GameWorld = nullptr;
		for (const FWorldContext& Context : GEngine->GetWorldContexts())
		{
			UWorld* World = Context.World();
			if (!World)
			{
				continue;
			}
			if (Context.WorldType == EWorldType::PIE)
			{
				return World;
			}
			if (Context.WorldType == EWorldType::Game && !GameWorld)
			{
				GameWorld = World;
			}
		}
		return GameWorld;
	}

	ARRMiner* FindPlayerMiner(UWorld* World)
	{
		if (!World)
		{
			return nullptr;
		}
		if (ARRMiner* Miner = Cast<ARRMiner>(UGameplayStatics::GetPlayerPawn(World, 0)))
		{
			return Miner;
		}
		return RRWorld::FindMiner(World, /*bLivingOnly*/ false);
	}

	bool GrantXP(UWorld* World, float Amount, const TCHAR* Source)
	{
		ARRMiner* Miner = FindPlayerMiner(World);
		URRProgressionComponent* Progression = Miner ? Miner->GetProgression() : nullptr;
		if (!Progression)
		{
			UE_LOG(LogRogueRigs, Warning, TEXT("rr.Cheat.GrantXP (%s): no miner with a progression."), Source);
			return false;
		}
		Progression->AddXP(Amount);
		UE_LOG(LogRogueRigs, Log, TEXT("rr.Cheat.GrantXP (%s): %.1f raw XP to %s, level %d, XP %.1f / %.1f."),
			Source, Amount, *Miner->GetName(), Progression->GetLevel(), Progression->GetXP(), Progression->GetXPToNextLevel());
		return true;
	}

	/** ASCs that hold one loose State.Invulnerable count added by the invulnerable cheat (both routes). */
	TArray<TWeakObjectPtr<UAbilitySystemComponent>> InvulnerableASCs;

	int32 SetInvulnerable(UWorld* World, bool bInvulnerable, const TCHAR* Source)
	{
		InvulnerableASCs.RemoveAll([](const TWeakObjectPtr<UAbilitySystemComponent>& ASC) { return !ASC.IsValid(); });
		if (!bInvulnerable)
		{
			const int32 Removed = InvulnerableASCs.Num();
			for (const TWeakObjectPtr<UAbilitySystemComponent>& ASC : InvulnerableASCs)
			{
				ASC->RemoveLooseGameplayTag(RRGameplayTags::State_Invulnerable);
			}
			InvulnerableASCs.Reset();
			UE_LOG(LogRogueRigs, Log, TEXT("rr.Cheat.Invulnerable 0 (%s): %d miner(s) vulnerable again."), Source, Removed);
			return Removed;
		}
		if (!World)
		{
			UE_LOG(LogRogueRigs, Warning, TEXT("rr.Cheat.Invulnerable (%s): no PIE or game world."), Source);
			return 0;
		}
		int32 Tagged = 0;
		for (TActorIterator<ARRMiner> It(World); It; ++It)
		{
			UAbilitySystemComponent* ASC = It->GetAbilitySystemComponent();
			if (!ASC)
			{
				continue;
			}
			if (!InvulnerableASCs.Contains(ASC))
			{
				ASC->AddLooseGameplayTag(RRGameplayTags::State_Invulnerable);
				InvulnerableASCs.Add(ASC);
			}
			++Tagged;
		}
		UE_LOG(LogRogueRigs, Log, TEXT("rr.Cheat.Invulnerable 1 (%s): %d miner(s) invulnerable."), Source, Tagged);
		return Tagged;
	}

	bool ChooseUpgrade(UWorld* World, int32 Index, const TCHAR* Source)
	{
		ARRMiner* Miner = FindPlayerMiner(World);
		URRLevelUpComponent* LevelUp = Miner ? Miner->GetLevelUp() : nullptr;
		const bool bAccepted = LevelUp && LevelUp->ChooseUpgrade(Index);
		UE_LOG(LogRogueRigs, Log, TEXT("rr.Cheat.ChooseUpgrade %d (%s): %s."), Index, Source, bAccepted ? TEXT("picked") : TEXT("refused (no screen or bad index)"));
		return bAccepted;
	}

	bool Reroll(UWorld* World, const TCHAR* Source)
	{
		ARRMiner* Miner = FindPlayerMiner(World);
		URRLevelUpComponent* LevelUp = Miner ? Miner->GetLevelUp() : nullptr;
		const bool bAccepted = LevelUp && LevelUp->RequestReroll();
		UE_LOG(LogRogueRigs, Log, TEXT("rr.Cheat.Reroll (%s): %s."), Source, bAccepted ? TEXT("rerolled") : TEXT("refused (no screen or reroll used)"));
		return bAccepted;
	}

	bool SkipToConsole(UWorld* World, const TCHAR* Source)
	{
		URRRunFlowSubsystem* RunFlow = World ? World->GetSubsystem<URRRunFlowSubsystem>() : nullptr;
		const ARRConsole* Console = RunFlow ? RunFlow->SkipToConsole() : nullptr;
		UE_LOG(LogRogueRigs, Log, TEXT("rr.Cheat.SkipToConsole (%s): %s."), Source, Console ? *FString::Printf(TEXT("console on (%d, %d)"), Console->GetCell().X, Console->GetCell().Y) : TEXT("refused (no run flow, grid, miner or cell)"));
		return Console != nullptr;
	}

	bool CompleteConsoleHack(UWorld* World, const TCHAR* Source)
	{
		URRRunFlowSubsystem* RunFlow = World ? World->GetSubsystem<URRRunFlowSubsystem>() : nullptr;
		const bool bDone = RunFlow && RunFlow->CompleteConsoleHack();
		UE_LOG(LogRogueRigs, Log, TEXT("rr.Cheat.CompleteHack (%s): %s."), Source, bDone ? TEXT("console hacked") : TEXT("refused (no pending console)"));
		return bDone;
	}

	bool SkipToExtraction(UWorld* World, const TCHAR* Source)
	{
		URRRunFlowSubsystem* RunFlow = World ? World->GetSubsystem<URRRunFlowSubsystem>() : nullptr;
		const bool bReady = RunFlow && RunFlow->SkipToExtraction();
		UE_LOG(LogRogueRigs, Log, TEXT("rr.Cheat.SkipToExtraction (%s): %s."), Source, bReady ? TEXT("extraction ready") : TEXT("refused (no run flow, grid or miner)"));
		return bReady;
	}

	/** Sets Variable back to Value with the priority of its last setter (a lower priority would be refused); runs the sink once more. */
	void ResetVariable(IConsoleVariable* Variable, const TCHAR* Value)
	{
		Variable->Set(Value, static_cast<EConsoleVariableFlags>(Variable->GetFlags() & ECVF_SetByMask));
	}

	/** rr.Cheat.GrantXP sink: a value > 0 adds that raw XP to the player miner, then the variable returns to 0. */
	void HandleGrantXP(IConsoleVariable* Variable)
	{
		const float Amount = Variable->GetFloat();
		if (Amount <= 0.f)
		{
			return;
		}
		GrantXP(FindCheatWorld(), Amount, TEXT("rr.Cheat"));
		ResetVariable(Variable, TEXT("0"));
	}

	/** rr.Cheat.Invulnerable sink: 1 tags every miner of the cheat world, 0 removes the counts the cheat added. */
	void HandleInvulnerable(IConsoleVariable* Variable)
	{
		SetInvulnerable(FindCheatWorld(), Variable->GetInt() != 0, TEXT("rr.Cheat"));
	}

	/** rr.Cheat.ChooseUpgrade sink: a value >= 0 picks that card of the open level-up screen, then the variable returns to -1. */
	void HandleChooseUpgrade(IConsoleVariable* Variable)
	{
		const int32 Index = Variable->GetInt();
		if (Index < 0)
		{
			return;
		}
		ChooseUpgrade(FindCheatWorld(), Index, TEXT("rr.Cheat"));
		ResetVariable(Variable, TEXT("-1"));
	}

	/** rr.Cheat.Reroll sink: 1 rerolls the open level-up screen, then the variable returns to 0. */
	void HandleReroll(IConsoleVariable* Variable)
	{
		if (Variable->GetInt() == 0)
		{
			return;
		}
		Reroll(FindCheatWorld(), TEXT("rr.Cheat"));
		ResetVariable(Variable, TEXT("0"));
	}

	/** rr.Cheat.AlertSpeed sink: writes the multiplier into the URRCheatSettings default object (read by the run flow). */
	void HandleAlertSpeed(IConsoleVariable* Variable)
	{
		GetMutableDefault<URRCheatSettings>()->AlertSpeedMultiplier = Variable->GetFloat();
		UE_LOG(LogRogueRigs, Log, TEXT("rr.Cheat.AlertSpeed (rr.Cheat): alert gain x%.2f."), Variable->GetFloat());
	}

	/** rr.Cheat.LightHalo sink: writes bLightHalo into the URRCheatSettings default object (applied by URRCheatSubsystem next frame). */
	void HandleLightHalo(IConsoleVariable* Variable)
	{
		GetMutableDefault<URRCheatSettings>()->bLightHalo = Variable->GetInt() != 0;
		UE_LOG(LogRogueRigs, Log, TEXT("rr.Cheat.LightHalo (rr.Cheat): requested %d."), Variable->GetInt() != 0 ? 1 : 0);
	}

	/** rr.Cheat.SkipToConsole sink: 1 drops the console now, then the variable returns to 0. */
	void HandleSkipToConsole(IConsoleVariable* Variable)
	{
		if (Variable->GetInt() == 0)
		{
			return;
		}
		SkipToConsole(FindCheatWorld(), TEXT("rr.Cheat"));
		ResetVariable(Variable, TEXT("0"));
	}

	/** rr.Cheat.CompleteHack sink: 1 completes the pending console, then the variable returns to 0. */
	void HandleCompleteHack(IConsoleVariable* Variable)
	{
		if (Variable->GetInt() == 0)
		{
			return;
		}
		CompleteConsoleHack(FindCheatWorld(), TEXT("rr.Cheat"));
		ResetVariable(Variable, TEXT("0"));
	}

	/** rr.Cheat.SkipToExtraction sink: 1 completes the objective and fills the alert, then the variable returns to 0. */
	void HandleSkipToExtraction(IConsoleVariable* Variable)
	{
		if (Variable->GetInt() == 0)
		{
			return;
		}
		SkipToExtraction(FindCheatWorld(), TEXT("rr.Cheat"));
		ResetVariable(Variable, TEXT("0"));
	}

	TAutoConsoleVariable<float> CVarGrantXP(TEXT("rr.Cheat.GrantXP"), 0.f,
		TEXT("Set to N > 0: adds N raw XP (XPGain applies) to the player miner, then resets to 0."),
		FConsoleVariableDelegate::CreateStatic(&HandleGrantXP), ECVF_Cheat);

	TAutoConsoleVariable<int32> CVarInvulnerable(TEXT("rr.Cheat.Invulnerable"), 0,
		TEXT("1: every miner gets the loose State.Invulnerable tag (no damage); 0: removed. Weapons keep firing. Set again after a restart."),
		FConsoleVariableDelegate::CreateStatic(&HandleInvulnerable), ECVF_Cheat);

	TAutoConsoleVariable<int32> CVarChooseUpgrade(TEXT("rr.Cheat.ChooseUpgrade"), -1,
		TEXT("Set to 0..2: picks that card of the open level-up screen (as the card button), then resets to -1."),
		FConsoleVariableDelegate::CreateStatic(&HandleChooseUpgrade), ECVF_Cheat);

	TAutoConsoleVariable<int32> CVarReroll(TEXT("rr.Cheat.Reroll"), 0,
		TEXT("Set to 1: rerolls the open level-up screen (as the Relancer button), then resets to 0."),
		FConsoleVariableDelegate::CreateStatic(&HandleReroll), ECVF_Cheat);

	TAutoConsoleVariable<float> CVarAlertSpeed(TEXT("rr.Cheat.AlertSpeed"), 1.f,
		TEXT("Alert gain multiplier (time and kills); writes URRCheatSettings::AlertSpeedMultiplier. 1 = normal."),
		FConsoleVariableDelegate::CreateStatic(&HandleAlertSpeed), ECVF_Cheat);

	TAutoConsoleVariable<int32> CVarLightHalo(TEXT("rr.Cheat.LightHalo"), 0,
		TEXT("1: point light halo above the player miner, sun and sky dimmed, exposure bias (URRCheatSettings LightHalo* values); 0: restored. Writes URRCheatSettings::bLightHalo."),
		FConsoleVariableDelegate::CreateStatic(&HandleLightHalo), ECVF_Cheat);

	TAutoConsoleVariable<int32> CVarSkipToConsole(TEXT("rr.Cheat.SkipToConsole"), 0,
		TEXT("Set to 1: the alert jumps to the console threshold and the console drops now, then resets to 0."),
		FConsoleVariableDelegate::CreateStatic(&HandleSkipToConsole), ECVF_Cheat);

	TAutoConsoleVariable<int32> CVarCompleteHack(TEXT("rr.Cheat.CompleteHack"), 0,
		TEXT("Set to 1: the pending console is hacked at once (alert resumes), then resets to 0."),
		FConsoleVariableDelegate::CreateStatic(&HandleCompleteHack), ECVF_Cheat);

	TAutoConsoleVariable<int32> CVarSkipToExtraction(TEXT("rr.Cheat.SkipToExtraction"), 0,
		TEXT("Set to 1: completes or skips the console and fills the alert (extraction ready), then resets to 0."),
		FConsoleVariableDelegate::CreateStatic(&HandleSkipToExtraction), ECVF_Cheat);
}
