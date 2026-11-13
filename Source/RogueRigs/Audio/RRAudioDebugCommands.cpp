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
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "HAL/IConsoleManager.h"
#include "Kismet/GameplayStatics.h"
#include "RogueRigs.h"

namespace RRAudioDebug
{
	static URRAudioSubsystem* FindSubsystem()
	{
		const UWorld* World = GEngine && GEngine->GameViewport ? GEngine->GameViewport->GetWorld() : nullptr;
		const UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
		return GameInstance ? GameInstance->GetSubsystem<URRAudioSubsystem>() : nullptr;
	}

	/** rr.Audio.Play <Event> [DX DY]: plays the event centred, or placed at the player pawn + (DX, DY) uu. */
	FAutoConsoleCommand PlayCommand(
		TEXT("rr.Audio.Play"),
		TEXT("rr.Audio.Play <Event> [DX DY]: plays the sound of a table event (Shot, Impact, Impact.Wall, EnemyDeath, PlayerHit, XPPickup, LevelUp, UIHover, UIClick), centred, or at the player + (DX, DY) uu (placed rows, distance fade)."),
		FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
		{
			URRAudioSubsystem* Subsystem = FindSubsystem();
			if (Args.Num() < 1 || Subsystem == nullptr)
			{
				UE_LOG(LogRogueRigs, Warning, TEXT("rr.Audio.Play: usage rr.Audio.Play <Event> [DX DY] (needs a game)."));
				return;
			}
			UWorld* World = GEngine->GameViewport->GetWorld();
			if (Args.Num() >= 3)
			{
				const APawn* Player = UGameplayStatics::GetPlayerPawn(World, 0);
				if (Player == nullptr)
				{
					UE_LOG(LogRogueRigs, Warning, TEXT("rr.Audio.Play: an offset needs a player pawn."));
					return;
				}
				const FVector Location = Player->GetActorLocation() + FVector(FCString::Atof(*Args[1]), FCString::Atof(*Args[2]), 0.f);
				Subsystem->Play(World, FName(*Args[0]), &Location);
				return;
			}
			Subsystem->Play(World, FName(*Args[0]), nullptr);
		}));

	/**
	 * rr.Audio.ScreenEdge: logs the ground distance (horizontal plane of the player pawn) from the player to the middle of each screen
	 * edge and to the corners, through the player camera: the measure behind a FadeOuter "screen edge" value (task-513).
	 */
	FAutoConsoleCommand ScreenEdgeCommand(
		TEXT("rr.Audio.ScreenEdge"),
		TEXT("rr.Audio.ScreenEdge: logs the distance in uu from the player to the left, right, top and bottom screen edges and corners, on the player's plane."),
		FConsoleCommandDelegate::CreateLambda([]()
		{
			UWorld* World = GEngine && GEngine->GameViewport ? GEngine->GameViewport->GetWorld() : nullptr;
			APlayerController* Controller = World ? UGameplayStatics::GetPlayerController(World, 0) : nullptr;
			const APawn* Player = Controller ? Controller->GetPawn() : nullptr;
			int32 Width = 0;
			int32 Height = 0;
			if (Player)
			{
				Controller->GetViewportSize(Width, Height);
			}
			if (Player == nullptr || Width <= 0 || Height <= 0)
			{
				UE_LOG(LogRogueRigs, Warning, TEXT("rr.Audio.ScreenEdge: needs a player pawn and a viewport."));
				return;
			}
			const FVector Center = Player->GetActorLocation();
			const auto Measure = [&](float X, float Y) -> float
			{
				FVector Origin, Direction;
				if (!Controller->DeprojectScreenPositionToWorld(X, Y, Origin, Direction) || FMath::IsNearlyZero(Direction.Z))
				{
					return -1.f;
				}
				const float T = (Center.Z - Origin.Z) / Direction.Z;
				return T > 0.f ? FVector::Dist2D(Origin + Direction * T, Center) : -1.f;
			};
			const float W = Width;
			const float H = Height;
			UE_LOG(LogRogueRigs, Log, TEXT("rr.Audio.ScreenEdge: viewport %dx%d, player (%.0f, %.0f, %.0f); edges left %.0f, right %.0f, top %.0f, bottom %.0f uu; corners top %.0f, bottom %.0f uu; centre %.0f uu."),
				Width, Height, Center.X, Center.Y, Center.Z, Measure(0.f, H * 0.5f), Measure(W, H * 0.5f), Measure(W * 0.5f, 0.f), Measure(W * 0.5f, H),
				Measure(0.f, 0.f), Measure(0.f, H), Measure(W * 0.5f, H * 0.5f));
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
		TEXT("rr.Debug.Click [down|up]: presses and releases the left mouse button where the cursor is (pair with rr.Debug.CursorOn); down or up sends only that half."),
		FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
		{
			if (!FSlateApplication::IsInitialized())
			{
				return;
			}
			FSlateApplication& App = FSlateApplication::Get();
			const FString Half = Args.Num() > 0 ? Args[0].ToLower() : FString();
			const TSharedPtr<FGenericWindow> NoWindow;
			if (Half != TEXT("up"))
			{
				App.OnMouseDown(NoWindow, EMouseButtons::Left);
			}
			if (Half != TEXT("down"))
			{
				App.OnMouseUp(EMouseButtons::Left);
			}
			UE_LOG(LogRogueRigs, Log, TEXT("rr.Debug.Click %s: left button at screen (%.0f, %.0f)."), Half.IsEmpty() ? TEXT("click") : *Half,
				App.GetCursorPos().X, App.GetCursorPos().Y);
		}));
}

#endif // !UE_BUILD_SHIPPING
