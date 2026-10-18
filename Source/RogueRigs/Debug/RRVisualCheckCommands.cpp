// Rogue Rigs - visual check console commands for unattended -game checks (debug only, compiled out of shipping): rr.Debug.Shots (timed screenshots + damage number state), rr.Debug.CritChance (weapon crit chance override), rr.Debug.Delayed (runs a command later), rr.Debug.DamageMiner (direct hit on the player miner, logs the vignette state for 6 frames), rr.Debug.SpawnNear (actors around the miner), rr.Debug.Objective (miner moved to the console or shuttle), rr.Debug.ScreenText (logs a screen text item through the text subsystem), rr.Debug.BreakShields (drains the shield of living shield chasers, health kept), rr.Debug.DamageCell (block brought to a display damage), rr.Debug.Camera (camera boom pitch, length, yaw).

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Containers/Ticker.h"
#include "Debug/RRStressSubsystem.h"
#include "GameFramework/Character.h"
#include "Grid/RRArenaGrid.h"
#include "Objectives/RRConsole.h"
#include "Objectives/RRRunFlowSubsystem.h"
#include "Objectives/RRShuttle.h"
#include "Damage/RRDamageNumberSubsystem.h"
#include "Damage/RRGE_DamageExplosion.h"
#include "Debug/RRCheats.h"
#include "Enemies/RREnemy.h"
#include "Enemies/RRShieldChaser.h"
#include "Engine/Engine.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "GAS/RRGameplayTags.h"
#include "GAS/RRHealthAttributeSet.h"
#include "Player/RRMiner.h"
#include "Engine/World.h"
#include "HAL/IConsoleManager.h"
#include "Misc/Paths.h"
#include "RogueRigs.h"
#include "Text/RRTextContentSubsystem.h"
#include "EngineUtils.h"
#include "UnrealClient.h"
#include "Misc/App.h"
#include "Components/Widget.h"
#include "UI/RRHUDWidget.h"
#include "UObject/UObjectIterator.h"
#include "Weapons/RRWeapon.h"
#include "Weapons/RRWeaponAttributeSet.h"
#include "World/RRWorld.h"

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

	/** Logs, for each HUD widget of a game world, the damage vignette's render opacity and the Blueprint HitFlash (vignette checks). */
	void LogVignetteState(const TCHAR* Context)
	{
		for (TObjectIterator<URRHUDWidget> It; It; ++It)
		{
			URRHUDWidget* HUD = *It;
			const UWorld* World = HUD->GetWorld();
			if (!World || !World->IsGameWorld() || HUD->IsTemplate())
			{
				continue;
			}
			const UWidget* Vignette = HUD->FindHUDWidget(TEXT("DamageVignette"));
			// HitFlash lives in the panel that draws the vignette (WBP_HealthStatus), not in WBP_HUD itself.
			double HitFlash = -1.0;
			const UUserWidget* FlashOwner = URRHUDWidget::FindUserWidgetWithProperty(HUD, TEXT("HitFlash"));
			if (const FDoubleProperty* Double = FlashOwner ? FindFProperty<FDoubleProperty>(FlashOwner->GetClass(), TEXT("HitFlash")) : nullptr)
			{
				HitFlash = Double->GetPropertyValue_InContainer(FlashOwner);
			}
			else if (const FFloatProperty* Float = FlashOwner ? FindFProperty<FFloatProperty>(FlashOwner->GetClass(), TEXT("HitFlash")) : nullptr)
			{
				HitFlash = Float->GetPropertyValue_InContainer(FlashOwner);
			}
			UE_LOG(LogRogueRigs, Log, TEXT("%s: frame %llu, delta %.3f s, vignette opacity %.3f (%s), HitFlash %.3f, in viewport %s"), Context,
				GFrameCounter, FApp::GetDeltaTime(), Vignette ? Vignette->GetRenderOpacity() : -1.f, Vignette ? Vignette->GetVisibility() == ESlateVisibility::Collapsed ? TEXT("collapsed") : TEXT("shown") : TEXT("missing"),
				HitFlash, HUD->IsInViewport() ? TEXT("yes") : TEXT("no"));
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
		LogVignetteState(TEXT("rr.Debug.Shots"));
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
			// The vignette state on the next frames (the flash decays fast: shows whether it was ever drawn).
			TSharedRef<int32> FramesLeft = MakeShared<int32>(6);
			FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([FramesLeft](float)
			{
				LogVignetteState(TEXT("rr.Debug.DamageMiner"));
				return --(*FramesLeft) > 0;
			}));
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

	/** Feet location (capsule bottom) of the player miner. */
	FVector MinerFeet(const ARRMiner& Miner)
	{
		return Miner.GetActorLocation() - FVector(0.f, 0.f, Miner.GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	}

	/** First arena grid of World, or null. */
	ARRArenaGrid* FindGrid(UWorld* World)
	{
		for (TActorIterator<ARRArenaGrid> It(World); It; ++It)
		{
			return *It;
		}
		return nullptr;
	}

	/**
	 * rr.Debug.SpawnNear <Class> <Distance> [YawDegrees] [Count]: spawns Count actors (default 1) of Class Distance uu from
	 * the player miner, the first at YawDegrees (default 0 = +X), the others spread evenly around it. Class: a /Game class
	 * path (e.g. /Game/RogueRigs/Blueprints/Pickups/BP_PickupHeal.BP_PickupHeal_C), else a robot name resolved like
	 * rr.Stress.Spawn. Characters stand on the miner's floor height, other actors are placed at it (a kamikaze within its
	 * arm range flares at once: visual check of the flare).
	 */
	FAutoConsoleCommandWithWorldAndArgs SpawnNearCommand(
		TEXT("rr.Debug.SpawnNear"),
		TEXT("rr.Debug.SpawnNear <Class> <Distance> [YawDegrees] [Count]: spawns actors of Class (/Game class path or robot name) Distance uu from the player miner (visual checks)."),
		FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
		{
			World = World ? World : RRCheats::FindCheatWorld();
			ARRMiner* Miner = RRCheats::FindPlayerMiner(World);
			if (!Miner || Args.Num() < 2)
			{
				UE_LOG(LogRogueRigs, Warning, TEXT("rr.Debug.SpawnNear: needs a class, a distance and a player miner."));
				return;
			}
			UClass* Class = nullptr;
			if (Args[0].StartsWith(TEXT("/")))
			{
				Class = LoadClass<AActor>(nullptr, *Args[0]);
			}
			else if (const URRStressSubsystem* Stress = World->GetSubsystem<URRStressSubsystem>())
			{
				Class = Stress->ResolveEnemyClass(Args[0]);
			}
			if (!Class)
			{
				UE_LOG(LogRogueRigs, Warning, TEXT("rr.Debug.SpawnNear: no class matches '%s'."), *Args[0]);
				return;
			}
			const float Distance = FCString::Atof(*Args[1]);
			const float Yaw = Args.Num() > 2 ? FCString::Atof(*Args[2]) : 0.f;
			const int32 Count = FMath::Clamp(Args.Num() > 3 ? FCString::Atoi(*Args[3]) : 1, 1, 64);
			const ACharacter* CharacterDefault = Cast<ACharacter>(Class->GetDefaultObject());
			const float Lift = CharacterDefault ? CharacterDefault->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 2.f : 0.f;
			FActorSpawnParameters Params;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			int32 Spawned = 0;
			for (int32 Index = 0; Index < Count; ++Index)
			{
				const float AngleDegrees = Yaw + 360.f * static_cast<float>(Index) / static_cast<float>(Count);
				const FVector Direction = FRotator(0.f, AngleDegrees, 0.f).Vector();
				const FVector Location = MinerFeet(*Miner) + Direction * Distance + FVector(0.f, 0.f, Lift);
				Spawned += World->SpawnActor<AActor>(Class, Location, FRotator(0.f, AngleDegrees + 180.f, 0.f), Params) ? 1 : 0;
			}
			UE_LOG(LogRogueRigs, Log, TEXT("rr.Debug.SpawnNear: %d %s at %.0f uu from the miner %s (yaw %.0f)."), Spawned, *Class->GetName(), Distance,
				*Miner->GetActorLocation().ToCompactString(), Yaw);
		}),
		ECVF_Cheat);

	/**
	 * rr.Debug.Objective <Console|Shuttle> [Distance]: moves the player miner Distance uu (2D) from the run's console (default
	 * 150, inside the zone; its marked rocks are destroyed first so the drone hacks) or shuttle (default 900, so the shuttle
	 * and the chevrons share the screen), on the side it came from (the first floor cell found turning by 30 degrees).
	 * Logs the miner and target positions and the bearing from the miner to the target.
	 */
	FAutoConsoleCommandWithWorldAndArgs ObjectiveCommand(
		TEXT("rr.Debug.Objective"),
		TEXT("rr.Debug.Objective <Console|Shuttle> [Distance]: moves the player miner next to the run's console (marked rocks destroyed) or shuttle (visual checks)."),
		FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
		{
			World = World ? World : RRCheats::FindCheatWorld();
			ARRMiner* Miner = RRCheats::FindPlayerMiner(World);
			const URRRunFlowSubsystem* RunFlow = World ? World->GetSubsystem<URRRunFlowSubsystem>() : nullptr;
			ARRArenaGrid* Grid = World ? FindGrid(World) : nullptr;
			if (!Miner || !RunFlow || !Grid || Args.Num() < 1)
			{
				UE_LOG(LogRogueRigs, Warning, TEXT("rr.Debug.Objective <Console|Shuttle> [Distance]: needs a run flow, a grid and a player miner."));
				return;
			}
			const bool bConsole = Args[0].Equals(TEXT("Console"), ESearchCase::IgnoreCase);
			AActor* Target = bConsole ? static_cast<AActor*>(RunFlow->GetConsole()) : static_cast<AActor*>(RunFlow->GetShuttle());
			if (!Target)
			{
				UE_LOG(LogRogueRigs, Warning, TEXT("rr.Debug.Objective: no %s in the run yet."), *Args[0]);
				return;
			}
			if (ARRConsole* Console = bConsole ? RunFlow->GetConsole() : nullptr)
			{
				for (const FIntPoint& Rock : Console->GetMarkedRocks())
				{
					Grid->DestroyCell(Rock, ERRTerrainDamageSource::Drone);
				}
			}
			const float Distance = Args.Num() > 1 ? FCString::Atof(*Args[1]) : (bConsole ? 150.f : 900.f);
			const FVector TargetLocation = Target->GetActorLocation();
			const FVector Away = (Miner->GetActorLocation() - TargetLocation).GetSafeNormal2D(UE_SMALL_NUMBER, FVector::XAxisVector);
			FVector Destination = Miner->GetActorLocation();
			for (int32 Turn = 0; Turn < 12; ++Turn)
			{
				const FVector Candidate = TargetLocation + Away.RotateAngleAxis(30.f * static_cast<float>(Turn), FVector::ZAxisVector) * Distance;
				if (!Grid->IsBlockCell(Grid->GetCellAtWorld(Candidate)))
				{
					Destination = FVector(Candidate.X, Candidate.Y, Miner->GetActorLocation().Z);
					break;
				}
			}
			Miner->SetActorLocation(Destination, false, nullptr, ETeleportType::TeleportPhysics);
			const FVector ToTarget = TargetLocation - Destination;
			UE_LOG(LogRogueRigs, Log, TEXT("rr.Debug.Objective: miner moved to %s, %s at %s, %.0f uu away, bearing yaw %.0f (screen: +X up, +Y right)."),
				*Destination.ToCompactString(), *Target->GetName(), *TargetLocation.ToCompactString(), ToTarget.Size2D(), ToTarget.Rotation().Yaw);
		}),
		ECVF_Cheat);

	/** rr.Debug.ScreenText <Id>: logs the title, subtitle, lines and labels URRTextContentSubsystem::GetScreenText returns for Id. */
	FAutoConsoleCommandWithWorldAndArgs ScreenTextCommand(
		TEXT("rr.Debug.ScreenText"),
		TEXT("rr.Debug.ScreenText <Id>: logs the screen texts (screen_texts.json) of item Id as the widgets get them."),
		FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
		{
			World = World ? World : RRCheats::FindCheatWorld();
			URRTextContentSubsystem* TextContent = URRTextContentSubsystem::Get(World);
			if (!TextContent || Args.Num() < 1)
			{
				UE_LOG(LogRogueRigs, Warning, TEXT("rr.Debug.ScreenText <Id>: needs a game world with the text subsystem."));
				return;
			}
			const FName Id(*Args[0]);
			FRRScreenText Screen;
			const bool bFound = TextContent->GetScreenText(Id, Screen);
			const auto Join = [](const TArray<FText>& Texts)
			{
				TArray<FString> Strings;
				for (const FText& Text : Texts)
				{
					Strings.Add(Text.ToString());
				}
				return FString::Join(Strings, TEXT(" | "));
			};
			UE_LOG(LogRogueRigs, Log, TEXT("rr.Debug.ScreenText: %s found=%d title='%s' subtitle='%s' lines='%s' labels='%s' (GetScreenTitle='%s', GetScreenSubtitle='%s')"),
				*Args[0], bFound ? 1 : 0, *Screen.Title.ToString(), *Screen.Subtitle.ToString(), *Join(Screen.Lines), *Join(Screen.Labels),
				*TextContent->GetScreenTitle(Id).ToString(), *TextContent->GetScreenSubtitle(Id).ToString());
		}),
		ECVF_Cheat);

	/**
	 * rr.Debug.BreakShields [RadiusUU]: breaks the shield of every living ARRShieldChaser (within RadiusUU, 2D, of the first living
	 * miner when given), health kept. Each one gets a URRGE_DamageExplosion of exactly its Shield value, applied to itself: the health
	 * set takes it all from Shield (nothing left for Health) and broadcasts OnShieldBroken the normal way, so the Blueprint swaps its
	 * materials and plays the burst as on a hit. Before the drain each chaser records a hit direction from the first living miner
	 * toward it, as if the miner shot it (task-224-01: GetShieldDebrisDirection); without a living miner none is recorded.
	 * Side effects of a hit: one damage number, one run log damage line.
	 */
	FAutoConsoleCommandWithWorldAndArgs BreakShieldsCommand(
		TEXT("rr.Debug.BreakShields"),
		TEXT("rr.Debug.BreakShields [RadiusUU]: drains the shield of every living shield chaser (within RadiusUU of the first living miner when given), health kept; fires OnShieldBroken."),
		FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
		{
			World = World ? World : RRCheats::FindCheatWorld();
			if (!World)
			{
				UE_LOG(LogRogueRigs, Warning, TEXT("rr.Debug.BreakShields: no game world."));
				return;
			}
			const bool bUseRadius = Args.Num() > 0;
			const float Radius = bUseRadius ? FMath::Max(0.f, FCString::Atof(*Args[0])) : 0.f;
			const ARRMiner* Miner = RRWorld::FindMiner(World, /*bLivingOnly*/ true);
			if (bUseRadius && !Miner)
			{
				UE_LOG(LogRogueRigs, Warning, TEXT("rr.Debug.BreakShields: a radius needs a living miner."));
				return;
			}
			int32 Broken = 0;
			int32 Living = 0;
			for (TActorIterator<ARRShieldChaser> It(World); It; ++It)
			{
				ARRShieldChaser* Chaser = *It;
				UAbilitySystemComponent* ASC = !Chaser->IsDead() ? Chaser->GetAbilitySystemComponent() : nullptr;
				if (!ASC || (bUseRadius && FVector::Dist2D(Chaser->GetActorLocation(), Miner->GetActorLocation()) > Radius))
				{
					continue;
				}
				++Living;
				const float Shield = Chaser->GetShield();
				if (Shield <= 0.f)
				{
					continue;
				}
				// Same path as an explosion hit (URRStressSubsystem::KillEnemy): damage equal to the shield, all absorbed by it.
				FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(URRGE_DamageExplosion::StaticClass(), 1.f, ASC->MakeEffectContext());
				if (!Spec.IsValid())
				{
					continue;
				}
				// As if the miner shot it: the break direction (shield debris) is the miner-to-chaser travel direction.
				if (Miner)
				{
					Chaser->RecordHitDirection(Chaser->GetActorLocation() - Miner->GetActorLocation());
				}
				Spec.Data->SetSetByCallerMagnitude(RRGameplayTags::Data_Damage, Shield);
				ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data);
				Broken += Chaser->HasShield() ? 0 : 1;
			}
			UE_LOG(LogRogueRigs, Log, TEXT("rr.Debug.BreakShields: %d shields broken of %d living shield chasers%s."), Broken, Living,
				bUseRadius ? *FString::Printf(TEXT(" within %.0f uu of %s"), Radius, *Miner->GetName()) : TEXT(""));
		}),
		ECVF_Cheat);

	/**
	 * rr.Debug.DamageCell <X> <Y> <Fraction>: brings the block in cell (X, Y) to Fraction display damage (0..1, below 1 so the
	 * block stays) through ARRArenaGrid::ApplyTerrainDamage (drone source), so the broken piece swap and the piece data follow
	 * as on a dig. Max health is read back from the display damage (d = 1 - health / max). Never heals.
	 */
	FAutoConsoleCommandWithWorldAndArgs DamageCellCommand(
		TEXT("rr.Debug.DamageCell"),
		TEXT("rr.Debug.DamageCell <X> <Y> <Fraction>: damages the block in cell (X, Y) up to Fraction of its max health (0..1, not destroyed), as a dig hit (visual checks of rock damage)."),
		FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
		{
			World = World ? World : RRCheats::FindCheatWorld();
			ARRArenaGrid* Grid = World ? FindGrid(World) : nullptr;
			if (!Grid || Args.Num() < 3)
			{
				UE_LOG(LogRogueRigs, Warning, TEXT("rr.Debug.DamageCell <X> <Y> <Fraction>: needs a grid and 3 arguments."));
				return;
			}
			const FIntPoint Cell(FCString::Atoi(*Args[0]), FCString::Atoi(*Args[1]));
			const float Target = FMath::Clamp(FCString::Atof(*Args[2]), 0.f, 0.99f);
			const float Health = Grid->GetCellHealth(Cell);
			const float Damage = Grid->GetCellDisplayDamage(Cell);
			if (Health <= 0.f || Damage >= 1.f)
			{
				UE_LOG(LogRogueRigs, Warning, TEXT("rr.Debug.DamageCell: no block in (%d, %d)."), Cell.X, Cell.Y);
				return;
			}
			const float MaxHealth = Health / (1.f - Damage);
			const float Amount = Health - (1.f - Target) * MaxHealth;
			if (Amount > 0.f)
			{
				Grid->ApplyTerrainDamage(Cell, Amount, ERRTerrainDamageSource::Drone);
			}
			UE_LOG(LogRogueRigs, Log, TEXT("rr.Debug.DamageCell: (%d, %d) health %.3f -> %.3f of max %.3f, display damage %.2f -> %.2f."),
				Cell.X, Cell.Y, Health, Grid->GetCellHealth(Cell), MaxHealth, Damage, Grid->GetCellDisplayDamage(Cell));
		}),
		ECVF_Cheat);

	/**
	 * rr.Debug.Camera <Pitch> <ArmLength> [Yaw]: sets the player miner's camera boom pitch, length and yaw (world degrees,
	 * absolute rotation), for close or three-quarter visual check views. Yaw also turns the screen axes of move and aim.
	 */
	FAutoConsoleCommand CameraCommand(
		TEXT("rr.Debug.Camera"),
		TEXT("rr.Debug.Camera <Pitch> <ArmLength> [Yaw]: sets the player miner's camera boom pitch (negative looks down), length (uu) and yaw (visual checks)."),
		FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
		{
			ARRMiner* Miner = RRCheats::FindPlayerMiner(RRCheats::FindCheatWorld());
			USpringArmComponent* Arm = Miner ? Miner->FindComponentByClass<USpringArmComponent>() : nullptr;
			if (!Arm || Args.Num() < 2)
			{
				UE_LOG(LogRogueRigs, Warning, TEXT("rr.Debug.Camera <Pitch> <ArmLength> [Yaw]: needs a player miner and 2 arguments."));
				return;
			}
			const float Yaw = Args.Num() >= 3 ? FCString::Atof(*Args[2]) : Arm->GetRelativeRotation().Yaw;
			Arm->SetRelativeRotation(FRotator(FCString::Atof(*Args[0]), Yaw, 0.f));
			Arm->TargetArmLength = FMath::Max(0.f, FCString::Atof(*Args[1]));
			UE_LOG(LogRogueRigs, Log, TEXT("rr.Debug.Camera: pitch %s, arm %.0f, yaw %.0f."), *Args[0], Arm->TargetArmLength, Yaw);
		}),
		ECVF_Cheat);
}

#endif // !UE_BUILD_SHIPPING
