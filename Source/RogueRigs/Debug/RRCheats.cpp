// Rogue Rigs - cheat console variables with on-change sinks (rr.Cheat.*): drive PIE checks from tools that can set console variables but cannot run commands or call functions (Unreal MCP).

#include "AbilitySystemComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GAS/RRGameplayTags.h"
#include "HAL/IConsoleManager.h"
#include "Kismet/GameplayStatics.h"
#include "Player/RRMiner.h"
#include "Progression/RRLevelUpComponent.h"
#include "Progression/RRProgressionComponent.h"
#include "RogueRigs.h"

namespace RRCheats
{
	/** The world the cheats act on: the first PIE world, else the first game world (headless tests, -game). */
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

	/** The miner of player 0 in World, else the first live miner found. */
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
		for (TActorIterator<ARRMiner> It(World); It; ++It)
		{
			if (IsValid(*It) && !It->IsActorBeingDestroyed())
			{
				return *It;
			}
		}
		return nullptr;
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
		ARRMiner* Miner = FindPlayerMiner(FindCheatWorld());
		if (URRProgressionComponent* Progression = Miner ? Miner->GetProgression() : nullptr)
		{
			Progression->AddXP(Amount);
			UE_LOG(LogRogueRigs, Log, TEXT("rr.Cheat.GrantXP: %.1f raw XP to %s, level %d, XP %.1f / %.1f."),
				Amount, *Miner->GetName(), Progression->GetLevel(), Progression->GetXP(), Progression->GetXPToNextLevel());
		}
		else
		{
			UE_LOG(LogRogueRigs, Warning, TEXT("rr.Cheat.GrantXP: no miner with a progression in a PIE or game world."));
		}
		ResetVariable(Variable, TEXT("0"));
	}

	/** ASCs that hold one loose State.Invulnerable count added by rr.Cheat.Invulnerable. */
	TArray<TWeakObjectPtr<UAbilitySystemComponent>> InvulnerableASCs;

	/**
	 * rr.Cheat.Invulnerable sink: 1 adds one loose State.Invulnerable count to every miner of the cheat world not tagged yet,
	 * 0 removes the counts it added. Loose tags are counted, so i-frames and rr.Stress.Peace stay independent.
	 */
	void HandleInvulnerable(IConsoleVariable* Variable)
	{
		const bool bInvulnerable = Variable->GetInt() != 0;
		InvulnerableASCs.RemoveAll([](const TWeakObjectPtr<UAbilitySystemComponent>& ASC) { return !ASC.IsValid(); });
		if (!bInvulnerable)
		{
			for (const TWeakObjectPtr<UAbilitySystemComponent>& ASC : InvulnerableASCs)
			{
				ASC->RemoveLooseGameplayTag(RRGameplayTags::State_Invulnerable);
			}
			UE_LOG(LogRogueRigs, Log, TEXT("rr.Cheat.Invulnerable 0: %d miner(s) vulnerable again."), InvulnerableASCs.Num());
			InvulnerableASCs.Reset();
			return;
		}
		UWorld* World = FindCheatWorld();
		if (!World)
		{
			UE_LOG(LogRogueRigs, Warning, TEXT("rr.Cheat.Invulnerable: no PIE or game world."));
			return;
		}
		for (TActorIterator<ARRMiner> It(World); It; ++It)
		{
			UAbilitySystemComponent* ASC = It->GetAbilitySystemComponent();
			if (ASC && !InvulnerableASCs.Contains(ASC))
			{
				ASC->AddLooseGameplayTag(RRGameplayTags::State_Invulnerable);
				InvulnerableASCs.Add(ASC);
			}
		}
		UE_LOG(LogRogueRigs, Log, TEXT("rr.Cheat.Invulnerable 1: %d miner(s) invulnerable."), InvulnerableASCs.Num());
	}

	/** rr.Cheat.ChooseUpgrade sink: a value >= 0 picks that card of the open level-up screen, then the variable returns to -1. */
	void HandleChooseUpgrade(IConsoleVariable* Variable)
	{
		const int32 Index = Variable->GetInt();
		if (Index < 0)
		{
			return;
		}
		ARRMiner* Miner = FindPlayerMiner(FindCheatWorld());
		URRLevelUpComponent* LevelUp = Miner ? Miner->GetLevelUp() : nullptr;
		const bool bAccepted = LevelUp && LevelUp->ChooseUpgrade(Index);
		UE_LOG(LogRogueRigs, Log, TEXT("rr.Cheat.ChooseUpgrade %d: %s."), Index, bAccepted ? TEXT("picked") : TEXT("refused (no screen or bad index)"));
		ResetVariable(Variable, TEXT("-1"));
	}

	/** rr.Cheat.Reroll sink: 1 rerolls the open level-up screen, then the variable returns to 0. */
	void HandleReroll(IConsoleVariable* Variable)
	{
		if (Variable->GetInt() == 0)
		{
			return;
		}
		ARRMiner* Miner = FindPlayerMiner(FindCheatWorld());
		URRLevelUpComponent* LevelUp = Miner ? Miner->GetLevelUp() : nullptr;
		const bool bAccepted = LevelUp && LevelUp->RequestReroll();
		UE_LOG(LogRogueRigs, Log, TEXT("rr.Cheat.Reroll: %s."), bAccepted ? TEXT("rerolled") : TEXT("refused (no screen or reroll used)"));
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
}
