// Rogue Rigs - audio debug console commands (task-511, compiled out of shipping): play an event, log the counters of the event
// table, press the left mouse button where the cursor is (a real Slate click, for -game log checks of the button sounds), and put
// the miner against a rock face so the drone mines (dig loop checks, task-517), record the game's audio output (task-518).

#include "CoreMinimal.h"

#if !UE_BUILD_SHIPPING

#include "Audio/RRAudioSubsystem.h"
#include "AudioDevice.h"
#include "AudioThread.h"
#include "Components/CapsuleComponent.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/MemoryWriter.h"
#include "Engine/Engine.h"
#include "Grid/RRArenaGrid.h"
#include "Grid/RRCellType.h"
#include "Player/RRMiner.h"
#include "World/RRWorld.h"
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

	/** rr.Audio.Play <Event> [DX DY [EdgeRadius]]: plays the event centred, or placed at the player pawn + (DX, DY) uu, fade measured from EdgeRadius. */
	FAutoConsoleCommand PlayCommand(
		TEXT("rr.Audio.Play"),
		TEXT("rr.Audio.Play <Event> [DX DY [EdgeRadius]]: plays the sound of a table event (Shot, Impact, Impact.Wall, EnemyDeath, Explosion, PlayerHit, XPPickup, OrePickup, LevelUp, UIHover, UIClick), centred, or at the player + (DX, DY) uu (placed rows, distance fade; EdgeRadius: fade from the edge of that circle, as an explosion)."),
		FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
		{
			URRAudioSubsystem* Subsystem = FindSubsystem();
			if (Args.Num() < 1 || Subsystem == nullptr)
			{
				UE_LOG(LogRogueRigs, Warning, TEXT("rr.Audio.Play: usage rr.Audio.Play <Event> [DX DY [EdgeRadius]] (needs a game)."));
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
				Subsystem->Play(World, FName(*Args[0]), &Location, Args.Num() >= 4 ? FCString::Atof(*Args[3]) : 0.f);
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

	/**
	 * rr.Audio.Record [stop]: records the main submix of the game's audio device (what the speakers get) and, on stop, writes it as a
	 * 16-bit WAV to Saved/AudioRecord/ (task-518: hear or measure a sound inside the real game mix).
	 */
	FAutoConsoleCommand RecordCommand(
		TEXT("rr.Audio.Record"),
		TEXT("rr.Audio.Record [stop]: starts recording the game's audio output; stop writes it to Saved/AudioRecord/RRAudio_<time>.wav."),
		FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
		{
			UWorld* World = GEngine && GEngine->GameViewport ? GEngine->GameViewport->GetWorld() : nullptr;
			FAudioDevice* Device = World ? World->GetAudioDeviceRaw() : nullptr;
			if (Device == nullptr)
			{
				UE_LOG(LogRogueRigs, Warning, TEXT("rr.Audio.Record: no audio device (needs a game with audio)."));
				return;
			}
			if (Args.Num() == 0 || !Args[0].Equals(TEXT("stop"), ESearchCase::IgnoreCase))
			{
				Device->StartRecording(nullptr, 0.f);
				UE_LOG(LogRogueRigs, Log, TEXT("rr.Audio.Record: recording the main submix."));
				return;
			}
			// Let the audio thread finish its queued commands before the buffer is read.
			FAudioCommandFence Fence;
			Fence.BeginFence();
			Fence.Wait();
			float NumChannels = 0.f;
			float SampleRate = 0.f;
			const Audio::FAlignedFloatBuffer& Samples = Device->StopRecording(nullptr, NumChannels, SampleRate);
			const int32 Channels = FMath::RoundToInt32(NumChannels);
			if (Samples.IsEmpty() || Channels <= 0 || SampleRate <= 0.f)
			{
				UE_LOG(LogRogueRigs, Warning, TEXT("rr.Audio.Record stop: nothing recorded."));
				return;
			}
			const uint32 Rate = FMath::RoundToInt32(SampleRate);
			const uint32 DataBytes = Samples.Num() * sizeof(int16);
			TArray<uint8> Wav;
			FMemoryWriter Writer(Wav);
			auto Put32 = [&Writer](uint32 Value) { Writer << Value; };
			auto Put16 = [&Writer](uint16 Value) { Writer << Value; };
			Writer.Serialize(const_cast<ANSICHAR*>("RIFF"), 4);
			Put32(36 + DataBytes);
			Writer.Serialize(const_cast<ANSICHAR*>("WAVEfmt "), 8);
			Put32(16);
			Put16(1);
			Put16(static_cast<uint16>(Channels));
			Put32(Rate);
			Put32(Rate * Channels * sizeof(int16));
			Put16(static_cast<uint16>(Channels * sizeof(int16)));
			Put16(16);
			Writer.Serialize(const_cast<ANSICHAR*>("data"), 4);
			Put32(DataBytes);
			for (const float Sample : Samples)
			{
				int16 Value = static_cast<int16>(FMath::Clamp(Sample, -1.f, 1.f) * 32767.f);
				Writer << Value;
			}
			const FString Path = FPaths::ProjectSavedDir() / TEXT("AudioRecord") / FString::Printf(TEXT("RRAudio_%s.wav"), *FDateTime::Now().ToString());
			const bool bSaved = FFileHelper::SaveArrayToFile(Wav, *Path);
			UE_LOG(LogRogueRigs, Log, TEXT("rr.Audio.Record stop: %.2f s, %d channels, %u Hz -> %s (%s)."), Samples.Num() / (SampleRate * Channels),
				Channels, Rate, *FPaths::ConvertRelativePathToFull(Path), bSaved ? TEXT("saved") : TEXT("save failed"));
		}));

	/** Miner location before the last rr.Audio.TouchRock, for rr.Audio.TouchRock back. */
	static TOptional<FVector> TouchRockReturn;

	/**
	 * rr.Audio.TouchRock [back]: moves the player miner against the face of the nearest rock block (a floor cell beside it, the miner's
	 * capsule touching the face), so the drone mines it as on a real contact (dig loop checks, task-517); back returns the miner where
	 * it was, which ends the contact.
	 */
	FAutoConsoleCommand TouchRockCommand(
		TEXT("rr.Audio.TouchRock"),
		TEXT("rr.Audio.TouchRock [back]: moves the player miner against the nearest rock face so the drone mines (dig sound checks); back returns it where it was."),
		FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
		{
			UWorld* World = GEngine && GEngine->GameViewport ? GEngine->GameViewport->GetWorld() : nullptr;
			ARRMiner* Miner = World ? Cast<ARRMiner>(UGameplayStatics::GetPlayerPawn(World, 0)) : nullptr;
			const ARRArenaGrid* Grid = World ? RRWorld::FindArenaGrid(World) : nullptr;
			if (Miner == nullptr || Grid == nullptr)
			{
				UE_LOG(LogRogueRigs, Warning, TEXT("rr.Audio.TouchRock: needs a player miner and a grid."));
				return;
			}
			if (Args.Num() > 0 && Args[0].Equals(TEXT("back"), ESearchCase::IgnoreCase))
			{
				if (TouchRockReturn.IsSet())
				{
					Miner->SetActorLocation(TouchRockReturn.GetValue(), false, nullptr, ETeleportType::TeleportPhysics);
					UE_LOG(LogRogueRigs, Log, TEXT("rr.Audio.TouchRock back: miner at %s."), *TouchRockReturn.GetValue().ToCompactString());
					TouchRockReturn.Reset();
				}
				return;
			}
			const FVector Start = Miner->GetActorLocation();
			const FIntPoint Center = Grid->GetCellAtWorld(Start);
			static const FIntPoint Sides[] = { FIntPoint(1, 0), FIntPoint(-1, 0), FIntPoint(0, 1), FIntPoint(0, -1) };
			double BestDistance = TNumericLimits<double>::Max();
			FIntPoint BestFloor;
			FIntPoint BestRock;
			for (int32 Y = Center.Y - 15; Y <= Center.Y + 15; ++Y)
			{
				for (int32 X = Center.X - 15; X <= Center.X + 15; ++X)
				{
					const FIntPoint Rock(X, Y);
					if (Grid->GetCellType(Rock) != ERRCellType::Rock)
					{
						continue;
					}
					for (const FIntPoint& Side : Sides)
					{
						const FIntPoint Floor = Rock + Side;
						const FVector FloorCenter = Grid->GetCellCenter(Floor.X, Floor.Y);
						const double Distance = FVector::Dist2D(FloorCenter, Start);
						if (Grid->GetCellType(Floor) == ERRCellType::Floor && Distance < BestDistance)
						{
							BestDistance = Distance;
							BestFloor = Floor;
							BestRock = Rock;
						}
					}
				}
			}
			if (BestDistance == TNumericLimits<double>::Max())
			{
				UE_LOG(LogRogueRigs, Warning, TEXT("rr.Audio.TouchRock: no rock face within 15 cells."));
				return;
			}
			// Capsule edge on the rock face: centre half a cell minus the capsule radius from the floor cell centre, toward the rock.
			const FIntPoint Toward = BestRock - BestFloor;
			const double Offset = 0.5 * Grid->GetCellSize() - Miner->GetCapsuleComponent()->GetScaledCapsuleRadius();
			FVector Destination = Grid->GetCellCenter(BestFloor.X, BestFloor.Y) + FVector(Toward.X, Toward.Y, 0.0) * FMath::Max(Offset, 0.0);
			Destination.Z = Start.Z;
			TouchRockReturn = Start;
			Miner->SetActorLocation(Destination, false, nullptr, ETeleportType::TeleportPhysics);
			UE_LOG(LogRogueRigs, Log, TEXT("rr.Audio.TouchRock: miner moved from %s to %s, against rock (%d, %d)."), *Start.ToCompactString(),
				*Destination.ToCompactString(), BestRock.X, BestRock.Y);
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
