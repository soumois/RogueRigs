// Rogue Rigs - run lost (death-screen gdd, task-484): death sequence, run-lost recap, Retry and Main Menu.

#include "Run/RRRunLostSubsystem.h"

#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/WorldSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Objectives/RRRunFlowSubsystem.h"
#include "Player/RRMiner.h"
#include "Progression/RRLevelUpComponent.h"
#include "RogueRigs.h"
#include "Run/RRRunSettings.h"
#include "Run/RRRunSubsystem.h"
#include "Run/RRRunTransitionSubsystem.h"
#include "RunLog/RRRunLogSubsystem.h"
#include "Text/RRScreenText.h"
#include "Text/RRTextContentSubsystem.h"

namespace RRRunLost
{
	/** screen_texts.json items of the two losses (death-screen gdd, Texts and Missed shuttle). */
	const FName DeathScreenTextId(TEXT("run_lost_death"));
	const FName ShuttleScreenTextId(TEXT("run_lost_shuttle"));

	/** Title shown when the screen text item is missing (death-screen gdd, Texts: fallback). */
	const TCHAR* FallbackTitle = TEXT("RUN LOST");
}

URRRunLostSubsystem* URRRunLostSubsystem::Get(const UObject* WorldContext)
{
	const UWorld* World = GEngine ? GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::ReturnNull) : nullptr;
	return World ? World->GetSubsystem<URRRunLostSubsystem>() : nullptr;
}

bool URRRunLostSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

void URRRunLostSubsystem::Deinitialize()
{
	RestoreTimeDilation();
	bDeathSequence = false;
	Super::Deinitialize();
}

TStatId URRRunLostSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(URRRunLostSubsystem, STATGROUP_Tickables);
}

void URRRunLostSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	const UWorld* World = GetWorld();
	if (!bDeathSequence || !World)
	{
		return;
	}
	const float SequenceSeconds = FMath::Max(0.f, GetDefault<URRRunSettings>()->DeathSequenceRealSeconds);
	if (World->GetRealTimeSeconds() - DeathRealSeconds >= SequenceSeconds)
	{
		FinishDeathSequence();
	}
}

bool URRRunLostSubsystem::BeginDeathSequence(int32 DuskiteHeld)
{
	UWorld* World = GetWorld();
	if (!World || bDeathSequence || bRunLostShown)
	{
		return false;
	}
	bDeathSequence = true;
	PendingDuskiteHeld = DuskiteHeld;
	DeathRealSeconds = World->GetRealTimeSeconds();
	DeathGameSeconds = World->GetTimeSeconds();

	// The run-lost screen wins over a pending or open level-up offer (as the end screen): no card screen during the sequence.
	const URRRunFlowSubsystem* RunFlow = World->GetSubsystem<URRRunFlowSubsystem>();
	const ARRMiner* Miner = RunFlow ? RunFlow->GetMiner() : nullptr;
	if (URRLevelUpComponent* LevelUp = Miner ? Miner->GetLevelUp() : nullptr)
	{
		LevelUp->CancelChoices();
	}

	const URRRunSettings* Settings = GetDefault<URRRunSettings>();
	if (AWorldSettings* WorldSettings = World->GetWorldSettings())
	{
		TimeDilationBefore = WorldSettings->TimeDilation;
		WorldSettings->SetTimeDilation(FMath::Clamp(Settings->DeathTimeDilation, 0.01f, 1.f));
		bTimeDilated = true;
	}
	UE_LOG(LogRogueRigs, Log, TEXT("URRRunLostSubsystem: miner died at game time %.3f s: death sequence, time dilation %.2f for %.2f s real time, Duskite held %d."),
		DeathGameSeconds, Settings->DeathTimeDilation, Settings->DeathSequenceRealSeconds, DuskiteHeld);
	return true;
}

void URRRunLostSubsystem::FinishDeathSequence()
{
	UWorld* World = GetWorld();
	bDeathSequence = false;
	RestoreTimeDilation();
	PauseForScreen();
	UE_LOG(LogRogueRigs, Log, TEXT("URRRunLostSubsystem: death sequence over after %.3f s real time, %.3f s game time: run-lost screen, game paused."),
		World ? World->GetRealTimeSeconds() - DeathRealSeconds : 0.0, World ? World->GetTimeSeconds() - DeathGameSeconds : 0.0);
	ShowRunLost(BuildRecap(/*bMinerDied*/ true, PendingDuskiteHeld));
}

bool URRRunLostSubsystem::ShowShuttleMissed(int32 DuskiteHeld)
{
	if (bRunLostShown || bDeathSequence)
	{
		return false;
	}
	// URRRunFlowSubsystem::EndRun paused the game already; the same pause call here is a no-op then.
	PauseForScreen();
	ShowRunLost(BuildRecap(/*bMinerDied*/ false, DuskiteHeld));
	return true;
}

void URRRunLostSubsystem::ShowRunLost(const FRRRunLostRecap& InRecap)
{
	Recap = InRecap;
	bRunLostShown = true;
	UE_LOG(LogRogueRigs, Log, TEXT("URRRunLostSubsystem: run lost (%s): '%s' / '%s', %s, robots destroyed %d, Duskite held %d."),
		Recap.bMinerDied ? TEXT("miner died") : TEXT("shuttle missed"), *Recap.Title.ToString(), *Recap.Subtitle.ToString(),
		*Recap.LevelText.ToString(), Recap.RobotsDestroyed, Recap.DuskiteHeld);
	OnRunLost.Broadcast(Recap);
}

FRRRunLostRecap URRRunLostSubsystem::BuildRecap(bool bMinerDied, int32 DuskiteHeld) const
{
	UWorld* World = GetWorld();
	FRRRunLostRecap Out;
	Out.bMinerDied = bMinerDied;
	Out.ScreenTextId = bMinerDied ? RRRunLost::DeathScreenTextId : RRRunLost::ShuttleScreenTextId;

	FRRScreenText ScreenText;
	URRTextContentSubsystem* Texts = URRTextContentSubsystem::Get(World);
	if (Texts && Texts->GetScreenText(Out.ScreenTextId, ScreenText) && !ScreenText.Title.IsEmpty())
	{
		Out.Title = ScreenText.Title;
		Out.Subtitle = ScreenText.Subtitle;
	}
	else
	{
		Out.Title = FText::FromString(RRRunLost::FallbackTitle);
	}

	const URRRunSubsystem* Run = URRRunSubsystem::Get(World);
	Out.LevelCount = FMath::Max(1, Run ? Run->GetLevelCount() : GetDefault<URRRunSettings>()->LevelLayouts.Num());
	Out.LevelReached = (Run && Run->IsRunInProgress()) ? FMath::Max(1, Run->GetLevelIndex()) : 1;

	// Run total from the run log (the stats screens count the same kills); a world without one counts this level's kills.
	const URRRunLogSubsystem* RunLog = URRRunLogSubsystem::Get(World);
	const int32 RunKills = RunLog ? RunLog->GetRunKillCount(World) : INDEX_NONE;
	const URRRunFlowSubsystem* RunFlow = World ? World->GetSubsystem<URRRunFlowSubsystem>() : nullptr;
	Out.RobotsDestroyed = RunKills != INDEX_NONE ? RunKills : (RunFlow ? RunFlow->GetKillCount() : 0);
	Out.DuskiteHeld = FMath::Max(0, DuskiteHeld);

	Out.LevelText = FText::FromString(FString::Printf(TEXT("Level %d of %d"), Out.LevelReached, Out.LevelCount));
	Out.RobotsText = FText::AsNumber(Out.RobotsDestroyed, &FNumberFormattingOptions::DefaultNoGrouping());
	Out.DuskiteText = FText::AsNumber(Out.DuskiteHeld, &FNumberFormattingOptions::DefaultNoGrouping());
	return Out;
}

bool URRRunLostSubsystem::Retry()
{
	UWorld* World = GetWorld();
	if (!World || !bRunLostShown || bLeaving)
	{
		UE_LOG(LogRogueRigs, Log, TEXT("URRRunLostSubsystem: Retry refused (%s)."),
			!World ? TEXT("no world") : (bLeaving ? TEXT("already leaving") : TEXT("the run-lost screen is not shown")));
		return false;
	}
	const FString& MapName = GetDefault<URRRunSettings>()->NewRunMapName;
	if (MapName.IsEmpty())
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("URRRunLostSubsystem: Retry refused (URRRunSettings::NewRunMapName is empty)."));
		return false;
	}
	RestoreTimeDilation();
	// As the main menu New Run (death-screen gdd, Buttons): run reset, loading screen "DROPPING IN", level 1, fade in.
	if (URRRunTransitionSubsystem* Transition = URRRunTransitionSubsystem::Get(World))
	{
		UE_LOG(LogRogueRigs, Log, TEXT("URRRunLostSubsystem: Retry: new run, loading %s (level 1) through the run transition."), *MapName);
		bLeaving = Transition->StartNewRun(World, MapName);
		return bLeaving;
	}
	// No game instance subsystem (bare test worlds): run reset and direct travel.
	if (URRRunSubsystem* Run = URRRunSubsystem::Get(World))
	{
		Run->ResetRun();
	}
	UE_LOG(LogRogueRigs, Warning, TEXT("URRRunLostSubsystem: Retry: no run transition subsystem; opening %s (level 1) without a loading screen."), *MapName);
	UGameplayStatics::OpenLevel(World, FName(*MapName));
	bLeaving = true;
	return true;
}

bool URRRunLostSubsystem::ReturnToMainMenu()
{
	UWorld* World = GetWorld();
	URRRunFlowSubsystem* RunFlow = World ? World->GetSubsystem<URRRunFlowSubsystem>() : nullptr;
	if (!RunFlow || !bRunLostShown || bLeaving)
	{
		UE_LOG(LogRogueRigs, Log, TEXT("URRRunLostSubsystem: Main Menu refused (%s)."),
			!RunFlow ? TEXT("no run flow") : (bLeaving ? TEXT("already leaving") : TEXT("the run-lost screen is not shown")));
		return false;
	}
	RestoreTimeDilation();
	UE_LOG(LogRogueRigs, Log, TEXT("URRRunLostSubsystem: Main Menu: run lost, back to the main menu."));
	bLeaving = RunFlow->ReturnToMainMenu();
	return bLeaving;
}

void URRRunLostSubsystem::PauseForScreen()
{
	APlayerController* Controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!Controller)
	{
		return;
	}
	if (!Controller->IsPaused())
	{
		Controller->SetPause(true);
	}
	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	Controller->SetInputMode(InputMode);
	Controller->SetShowMouseCursor(true);
}

void URRRunLostSubsystem::RestoreTimeDilation()
{
	if (!bTimeDilated)
	{
		return;
	}
	bTimeDilated = false;
	UWorld* World = GetWorld();
	if (AWorldSettings* WorldSettings = World ? World->GetWorldSettings() : nullptr)
	{
		WorldSettings->SetTimeDilation(TimeDilationBefore);
	}
}
