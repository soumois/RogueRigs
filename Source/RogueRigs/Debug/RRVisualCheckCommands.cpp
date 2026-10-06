// Rogue Rigs - visual check console commands for unattended -game checks (debug only, compiled out of shipping): rr.Debug.Shots (timed screenshots + damage number state), rr.Debug.CritChance (weapon crit chance override), rr.Debug.Delayed (runs a command later), rr.Debug.DamageMiner (direct hit on the player miner).

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Containers/Ticker.h"
#include "Damage/RRDamageNumberSubsystem.h"
#include "Debug/RRCheats.h"
#include "Engine/Engine.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "GAS/RRHealthAttributeSet.h"
#include "Player/RRMiner.h"
#include "Engine/World.h"
#include "HAL/IConsoleManager.h"
#include "Misc/Paths.h"
#include "RogueRigs.h"
#include "EngineUtils.h"
#include "UnrealClient.h"
#include "Weapons/RRWeapon.h"
#include "Weapons/RRWeaponAttributeSet.h"

#if !UE_BUILD_SHIPPING

namespace RRScreenshotCommands
{
	/** State of one timed screenshot series (real time, unaffected by slomo or pause). */
	struct FShotSeries
	{
		/** Seconds left before the next shot. */
		double TimeToNext = 0.0;
		/** Seconds between shots. */
		double Interval = 0.0;
		/** Shots still to take. */
		int32 Remaining = 0;
		/** Index of the next shot, used in the file name. */
		int32 Index = 0;
		/** File name prefix. */
		FString Prefix;
		/** True: the shot includes the UI (HUD widgets), captured from the window. */
		bool bShowUI = false;
	};

	/** Logs the damage number state of the first game world (pushed total, pending, live and spawned particles) next to a shot. */
	void LogDamageNumberState()
	{
		if (!GEngine)
		{
			return;
		}
		for (const FWorldContext& Context : GEngine->GetWorldContexts())
		{
			const UWorld* World = Context.World();
			const URRDamageNumberSubsystem* Numbers = World && World->IsGameWorld() ? World->GetSubsystem<URRDamageNumberSubsystem>() : nullptr;
			if (Numbers)
			{
				int32 Live = 0;
				int32 Spawned = 0;
				const bool bInstance = Numbers->GetParticleStats(Live, Spawned);
				UE_LOG(LogRogueRigs, Log, TEXT("rr.Debug.Shots: damage numbers pushed %d, pending %d, component %s, instance %s, particles live %d spawned %d"),
					Numbers->GetTotalPushedCount(), Numbers->GetPendingCount(), Numbers->GetNiagaraComponent() ? TEXT("yes") : TEXT("no"),
					bInstance ? TEXT("yes") : TEXT("no"), Live, Spawned);
				return;
			}
		}
	}

	/** Ticks a series: requests one screenshot of the game viewport (no UI) when due; false once the series is done. */
	bool TickSeries(float DeltaTime, TSharedRef<FShotSeries> Series)
	{
		Series->TimeToNext -= DeltaTime;
		if (Series->TimeToNext > 0.0)
		{
			return true;
		}
		const FString File = FPaths::Combine(FPaths::ScreenShotDir(), FString::Printf(TEXT("%s_%02d.png"), *Series->Prefix, Series->Index));
		FScreenshotRequest::RequestScreenshot(File, Series->bShowUI, /*bAddFilenameSuffix*/ false);
		UE_LOG(LogRogueRigs, Log, TEXT("rr.Debug.Shots: requested %s%s"), *File, Series->bShowUI ? TEXT(" (with UI)") : TEXT(""));
		LogDamageNumberState();
		++Series->Index;
		Series->TimeToNext = Series->Interval;
		return --Series->Remaining > 0;
	}

	/** rr.Debug.Shots <Delay> <Count> <Interval> [Prefix] [ShowUI]: Count screenshots, the first after Delay s, then every Interval s (real time); ShowUI 1 includes the HUD. */
	FAutoConsoleCommand ShotsCommand(
		TEXT("rr.Debug.Shots"),
		TEXT("rr.Debug.Shots <Delay> <Count> <Interval> [Prefix] [ShowUI]: takes Count game viewport screenshots to Saved/Screenshots (ShowUI 1: with the HUD, else none), the first after Delay seconds, then every Interval seconds of real time."),
		FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
		{
			TSharedRef<FShotSeries> Series = MakeShared<FShotSeries>();
			Series->TimeToNext = Args.Num() > 0 ? FCString::Atod(*Args[0]) : 1.0;
			Series->Remaining = FMath::Max(1, Args.Num() > 1 ? FCString::Atoi(*Args[1]) : 1);
			Series->Interval = FMath::Max(0.0, Args.Num() > 2 ? FCString::Atod(*Args[2]) : 0.5);
			Series->Prefix = Args.Num() > 3 ? Args[3] : FString(TEXT("RRShot"));
			Series->bShowUI = Args.Num() > 4 && FCString::Atoi(*Args[4]) != 0;
			FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([Series](float DeltaTime)
			{
				return TickSeries(DeltaTime, Series);
			}));
		}),
		ECVF_Cheat);

	/** Runs Command on the first local player controller of the cheat world (engine, cheat manager and player commands), else on the engine. */
	void RunDelayedCommand(const FString& Command)
	{
		UWorld* World = RRCheats::FindCheatWorld();
		APlayerController* Controller = World ? World->GetFirstPlayerController() : nullptr;
		UE_LOG(LogRogueRigs, Log, TEXT("rr.Debug.Delayed: running '%s'%s"), *Command, Controller ? TEXT("") : TEXT(" (no player controller)"));
		if (Controller)
		{
			Controller->ConsoleCommand(Command, /*bWriteToLog*/ true);
		}
		else if (GEngine)
		{
			GEngine->Exec(World, *Command);
		}
	}

	/** rr.Debug.Delayed <Seconds> <Command...>: runs the command after Seconds of real time (chains timed steps of an -ExecCmds check). */
	FAutoConsoleCommand DelayedCommand(
		TEXT("rr.Debug.Delayed"),
		TEXT("rr.Debug.Delayed <Seconds> <Command...>: runs Command (the rest of the line) after Seconds of real time, through the first player controller."),
		FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
		{
			if (Args.Num() < 2)
			{
				return;
			}
			const float Delay = FMath::Max(0.f, FCString::Atof(*Args[0]));
			const FString Command = FString::Join(TArrayView<const FString>(Args).RightChop(1), TEXT(" "));
			FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([Command](float)
			{
				RunDelayedCommand(Command);
				return false;
			}), Delay);
		}),
		ECVF_Cheat);

	/** rr.Debug.DamageMiner <Amount>: lowers the player miner's Health base by Amount (min 1 left): a hit for the HUD vignette, no other effect. */
	FAutoConsoleCommand DamageMinerCommand(
		TEXT("rr.Debug.DamageMiner"),
		TEXT("rr.Debug.DamageMiner <Amount>: lowers the player miner's Health by Amount, keeping at least 1 (visual checks of the damage vignette)."),
		FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
		{
			ARRMiner* Miner = RRCheats::FindPlayerMiner(RRCheats::FindCheatWorld());
			UAbilitySystemComponent* ASC = Miner ? Miner->GetAbilitySystemComponent() : nullptr;
			if (!ASC || Args.Num() < 1)
			{
				UE_LOG(LogRogueRigs, Warning, TEXT("rr.Debug.DamageMiner: no miner or no amount."));
				return;
			}
			const float Health = ASC->GetNumericAttribute(URRHealthAttributeSet::GetHealthAttribute());
			const float NewHealth = FMath::Max(1.f, Health - FMath::Max(0.f, FCString::Atof(*Args[0])));
			ASC->SetNumericAttributeBase(URRHealthAttributeSet::GetHealthAttribute(), NewHealth);
			UE_LOG(LogRogueRigs, Log, TEXT("rr.Debug.DamageMiner: Health %.1f -> %.1f."), Health, NewHealth);
		}),
		ECVF_Cheat);

	/** rr.Debug.CritChance <0..1>: sets the CritChance base of every weapon of the world (forces crits for a visual check). */
	FAutoConsoleCommandWithWorldAndArgs CritChanceCommand(
		TEXT("rr.Debug.CritChance"),
		TEXT("rr.Debug.CritChance <0..1>: sets the CritChance base attribute of every weapon in the world (visual checks of critical damage numbers)."),
		FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
		{
			if (!World || Args.Num() < 1)
			{
				return;
			}
			const float Chance = FMath::Clamp(FCString::Atof(*Args[0]), 0.f, 1.f);
			int32 Count = 0;
			for (TActorIterator<ARRWeapon> It(World); It; ++It)
			{
				if (UAbilitySystemComponent* ASC = It->GetAbilitySystemComponent())
				{
					ASC->SetNumericAttributeBase(URRWeaponAttributeSet::GetCritChanceAttribute(), Chance);
					++Count;
				}
			}
			UE_LOG(LogRogueRigs, Log, TEXT("rr.Debug.CritChance: %.2f on %d weapons."), Chance, Count);
		}),
		ECVF_Cheat);
}

#endif // !UE_BUILD_SHIPPING
