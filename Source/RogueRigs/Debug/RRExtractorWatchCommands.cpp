// Rogue Rigs - extractor watch console command for unattended -game checks (debug only, compiled out of shipping): rr.Debug.WatchExtractors
// logs every ARRShuttle (exit and arrival) with its place, hidden state, takeoff state, player view state and the run transition state,
// at a fixed real-time interval and on every state change, also while the world is paused (task-397-01).

#include "CoreMinimal.h"
#include "Containers/Ticker.h"
#include "Debug/RRCheats.h"
#include "Engine/GameViewportClient.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "HAL/IConsoleManager.h"
#include "Objectives/RRRunFlowSubsystem.h"
#include "Objectives/RRShuttle.h"
#include "RogueRigs.h"
#include "Run/RRRunTransitionSubsystem.h"

#if !UE_BUILD_SHIPPING

namespace RRExtractorWatchCommands
{
	/** State of one watch (real time, unaffected by pause). */
	struct FExtractorWatch
	{
		/** Seconds left before the watch stops. */
		double Remaining = 0.0;
		/** Seconds between two periodic lines. */
		double Interval = 1.0;
		/** Seconds left before the next periodic line. */
		double TimeToNext = 0.0;
		/** State signature of the last logged step (a change logs at once). */
		FString LastSignature;
	};

	/** Short name of an enum value (no type prefix). */
	template <typename TEnum>
	FString EnumName(TEnum Value)
	{
		const FString Full = UEnum::GetValueAsString(Value);
		int32 Index = INDEX_NONE;
		return Full.FindLastChar(TEXT(':'), Index) ? Full.RightChop(Index + 1) : Full;
	}

	/** "in view", "out of view" or "no viewport": any of the 8 bounds corners projects inside the player 0 viewport. */
	FString GetViewState(const AActor& Actor)
	{
		APlayerController* Controller = Actor.GetWorld() ? Actor.GetWorld()->GetFirstPlayerController() : nullptr;
		const ULocalPlayer* LocalPlayer = Controller ? Controller->GetLocalPlayer() : nullptr;
		FVector2D ViewportSize = FVector2D::ZeroVector;
		if (LocalPlayer && LocalPlayer->ViewportClient)
		{
			LocalPlayer->ViewportClient->GetViewportSize(ViewportSize);
		}
		if (ViewportSize.X <= 1.f || ViewportSize.Y <= 1.f)
		{
			return TEXT("no viewport");
		}
		FVector Origin, Extent;
		Actor.GetActorBounds(true, Origin, Extent);
		for (int32 Corner = 0; Corner < 8; ++Corner)
		{
			const FVector Point = Origin + FVector((Corner & 1) ? Extent.X : -Extent.X, (Corner & 2) ? Extent.Y : -Extent.Y, (Corner & 4) ? Extent.Z : -Extent.Z);
			FVector2D Screen;
			if (Controller->ProjectWorldLocationToScreen(Point, Screen, false)
				&& Screen.X >= 0.f && Screen.X <= ViewportSize.X && Screen.Y >= 0.f && Screen.Y <= ViewportSize.Y)
			{
				return TEXT("in view");
			}
		}
		return TEXT("out of view");
	}

	/** Logs one line for the transition state and one per extractor; Reason says why (periodic or change). */
	void LogExtractors(UWorld& World, const TCHAR* Reason)
	{
		const URRRunTransitionSubsystem* Transition = URRRunTransitionSubsystem::Get(&World);
		const URRRunFlowSubsystem* RunFlow = World.GetSubsystem<URRRunFlowSubsystem>();
		const FString TransitionState = Transition
			? FString::Printf(TEXT("phase %s, prep %s, fade %s"), *EnumName(Transition->GetPhase()), *EnumName(Transition->GetNextLevelPrepStep()), *EnumName(Transition->GetScreenFade()))
			: FString(TEXT("no transition"));
		TArray<FString> Lines;
		for (TActorIterator<ARRShuttle> It(&World); It; ++It)
		{
			const ARRShuttle* Shuttle = *It;
			if (!IsValid(Shuttle) || Shuttle->IsActorBeingDestroyed())
			{
				continue;
			}
			const TCHAR* Role = Shuttle->IsArrival() ? TEXT("arrival") : TEXT("exit");
			const FString View = GetViewState(*Shuttle);
			const bool bVisible = !Shuttle->IsHidden() && View == TEXT("in view");
			Lines.Add(FString::Printf(TEXT("rr.Debug.WatchExtractors:   %s (%s): location %s, rotation %s, hidden %s, taking off %s (%.2f s), %s -> %s."),
				*Shuttle->GetName(), Role, *Shuttle->GetActorLocation().ToCompactString(), *Shuttle->GetActorRotation().ToCompactString(),
				Shuttle->IsHidden() ? TEXT("yes") : TEXT("no"), Shuttle->IsTakingOff() ? TEXT("yes") : TEXT("no"), Shuttle->GetTakeoffSeconds(), *View,
				bVisible ? TEXT("VISIBLE") : TEXT("not visible")));
		}
		UE_LOG(LogRogueRigs, Log, TEXT("rr.Debug.WatchExtractors: %s, frame %llu, world %s, %s, %d extractors (run flow exit %s, arrival %s)."), Reason, GFrameCounter,
			World.IsPaused() ? TEXT("paused") : TEXT("playing"), *TransitionState, Lines.Num(),
			RunFlow && RunFlow->GetShuttle() ? *RunFlow->GetShuttle()->GetName() : TEXT("none"),
			RunFlow && RunFlow->GetArrivalShuttle() ? *RunFlow->GetArrivalShuttle()->GetName() : TEXT("none"));
		for (const FString& Line : Lines)
		{
			UE_LOG(LogRogueRigs, Log, TEXT("%s"), *Line);
		}
	}

	/** Ticks a watch: logs on a state change or when the interval is due; false once the watch is over. */
	bool TickWatch(float DeltaTime, TSharedRef<FExtractorWatch> Watch)
	{
		Watch->Remaining -= DeltaTime;
		Watch->TimeToNext -= DeltaTime;
		UWorld* World = RRCheats::FindCheatWorld();
		if (!World)
		{
			return Watch->Remaining > 0.0;
		}
		// The signature is built by a silent pass first: a change logs at once, else the periodic line logs when due.
		FString Signature;
		{
			const URRRunTransitionSubsystem* Transition = URRRunTransitionSubsystem::Get(World);
			Signature = Transition ? FString::Printf(TEXT("%s %s %s"), *EnumName(Transition->GetPhase()), *EnumName(Transition->GetNextLevelPrepStep()), *EnumName(Transition->GetScreenFade()))
				: FString(TEXT("none"));
			for (TActorIterator<ARRShuttle> It(World); It; ++It)
			{
				if (IsValid(*It) && !It->IsActorBeingDestroyed())
				{
					Signature += FString::Printf(TEXT("|%s %d %d %s"), *It->GetName(), It->IsHidden() ? 1 : 0, It->IsTakingOff() ? 1 : 0, *GetViewState(**It));
				}
			}
		}
		const bool bChanged = Signature != Watch->LastSignature;
		if (bChanged || Watch->TimeToNext <= 0.0)
		{
			LogExtractors(*World, bChanged ? TEXT("change") : TEXT("tick"));
			Watch->LastSignature = Signature;
			Watch->TimeToNext = Watch->Interval;
		}
		if (Watch->Remaining <= 0.0)
		{
			UE_LOG(LogRogueRigs, Log, TEXT("rr.Debug.WatchExtractors: watch over."));
			return false;
		}
		return true;
	}

	/** rr.Debug.WatchExtractors <Seconds> [Interval]: logs every extractor each Interval s (default 1) and on every change, for Seconds of real time. */
	FAutoConsoleCommand WatchExtractorsCommand(
		TEXT("rr.Debug.WatchExtractors"),
		TEXT("rr.Debug.WatchExtractors <Seconds> [Interval]: logs every ARRShuttle (exit and arrival: location, rotation, hidden, takeoff, player view) and the run transition state every Interval seconds (default 1) and on each change, for Seconds of real time, also while paused."),
		FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
		{
			TSharedRef<FExtractorWatch> Watch = MakeShared<FExtractorWatch>();
			Watch->Remaining = FMath::Max(0.1, Args.Num() > 0 ? FCString::Atod(*Args[0]) : 60.0);
			Watch->Interval = FMath::Max(0.05, Args.Num() > 1 ? FCString::Atod(*Args[1]) : 1.0);
			UE_LOG(LogRogueRigs, Log, TEXT("rr.Debug.WatchExtractors: watching for %.1f s, every %.2f s and on each change."), Watch->Remaining, Watch->Interval);
			FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([Watch](float DeltaTime)
			{
				return TickWatch(DeltaTime, Watch);
			}));
		}),
		ECVF_Cheat);
}

#endif // !UE_BUILD_SHIPPING
