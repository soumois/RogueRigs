// Rogue Rigs - audio debug console commands (task-511, compiled out of shipping): play an event, log the counters of the event
// table, and press the left mouse button where the cursor is (a real Slate click, for -game log checks of the button sounds).

#include "CoreMinimal.h"

#if !UE_BUILD_SHIPPING

#include "Audio/RRAudioSubsystem.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/GameViewportClient.h"
#include "Engine/World.h"
#include "Framework/Application/SlateApplication.h"
#include "HAL/IConsoleManager.h"
#include "RogueRigs.h"

namespace RRAudioDebug
{
	static URRAudioSubsystem* FindSubsystem()
	{
		const UWorld* World = GEngine && GEngine->GameViewport ? GEngine->GameViewport->GetWorld() : nullptr;
		const UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
		return GameInstance ? GameInstance->GetSubsystem<URRAudioSubsystem>() : nullptr;
	}

	/** rr.Audio.Play <Event>: plays the event centred. */
	FAutoConsoleCommand PlayCommand(
		TEXT("rr.Audio.Play"),
		TEXT("rr.Audio.Play <Event>: plays the sound of a table event, centred (Shot, Impact, EnemyDeath, PlayerHit, XPPickup, LevelUp, UIHover, UIClick)."),
		FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
		{
			URRAudioSubsystem* Subsystem = FindSubsystem();
			if (Args.Num() < 1 || Subsystem == nullptr)
			{
				UE_LOG(LogRogueRigs, Warning, TEXT("rr.Audio.Play: usage rr.Audio.Play <Event> (needs a game)."));
				return;
			}
			Subsystem->Play(GEngine->GameViewport->GetWorld(), FName(*Args[0]), nullptr);
		}));

	/** rr.Audio.Stats: per table row, plays, cap, peak simultaneous plays, cap stops. */
	FAutoConsoleCommand StatsCommand(
		TEXT("rr.Audio.Stats"),
		TEXT("rr.Audio.Stats [reset]: logs per sound event its sound, plays, cap, peak of simultaneous plays and cap stops; reset clears the counters."),
		FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
		{
			if (URRAudioSubsystem* Subsystem = FindSubsystem())
			{
				Subsystem->LogStats();
				if (Args.Num() > 0 && Args[0].Equals(TEXT("reset"), ESearchCase::IgnoreCase))
				{
					Subsystem->ResetStats();
				}
			}
		}));

	/** rr.Debug.Click: a left press and release at the cursor, through Slate as a real mouse click. */
	FAutoConsoleCommand ClickCommand(
		TEXT("rr.Debug.Click"),
		TEXT("rr.Debug.Click: presses and releases the left mouse button where the cursor is (pair with rr.Debug.CursorOn)."),
		FConsoleCommandDelegate::CreateLambda([]()
		{
			if (!FSlateApplication::IsInitialized())
			{
				return;
			}
			FSlateApplication& App = FSlateApplication::Get();
			const TSharedPtr<FGenericWindow> NoWindow;
			App.OnMouseDown(NoWindow, EMouseButtons::Left);
			App.OnMouseUp(EMouseButtons::Left);
			UE_LOG(LogRogueRigs, Log, TEXT("rr.Debug.Click: left click at screen (%.0f, %.0f)."), App.GetCursorPos().X, App.GetCursorPos().Y);
		}));
}

#endif // !UE_BUILD_SHIPPING
