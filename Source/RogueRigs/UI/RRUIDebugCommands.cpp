// Rogue Rigs - UI input debug console commands for -game captures (compiled out of shipping): rr.Debug.Cursor and
// rr.Debug.CursorOn move the real mouse cursor so a capture shows a button hovered by the mouse, rr.Debug.ButtonHover logs the
// hover state of every attached button (task-485, ui-button-hover), rr.Debug.PressKey simulates a key press on the player input
// (task-547, moved here from RRScreenEntranceDebugCommands.cpp by task-549: one file for the UI input of every screen).

#include "CoreMinimal.h"

#if !UE_BUILD_SHIPPING

#include "Components/Button.h"
#include "Containers/Ticker.h"
#include "Debug/RRCheats.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/GameViewportClient.h"
#include "Engine/World.h"
#include "Framework/Application/SlateApplication.h"
#include "GameFramework/PlayerController.h"
#include "HAL/IConsoleManager.h"
#include "InputCoreTypes.h"
#include "InputKeyEventArgs.h"
#include "RogueRigs.h"
#include "UI/RRButtonHover.h"
#include "UObject/UObjectIterator.h"
#include "Widgets/SViewport.h"

namespace RRUIDebug
{
	static void MoveCursor(const FVector2D& Absolute, const FString& What)
	{
		FSlateApplication::Get().SetCursorPos(Absolute);
		UE_LOG(LogRogueRigs, Log, TEXT("rr.Debug.Cursor %s: cursor at screen (%.0f, %.0f)."), *What, Absolute.X, Absolute.Y);
	}

	/** rr.Debug.Cursor <fx> <fy>: cursor at (fx, fy) of the game viewport, 0..1 from the top-left corner. */
	FAutoConsoleCommand CursorCommand(
		TEXT("rr.Debug.Cursor"),
		TEXT("rr.Debug.Cursor <fx> <fy>: moves the mouse cursor to the fraction (fx, fy) of the game viewport (0..1 from the top left), as a player would."),
		FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
		{
			if (Args.Num() < 2 || !FSlateApplication::IsInitialized() || !GEngine || !GEngine->GameViewport)
			{
				UE_LOG(LogRogueRigs, Warning, TEXT("rr.Debug.Cursor: usage rr.Debug.Cursor <fx> <fy> (needs a game viewport)."));
				return;
			}
			const TSharedPtr<SViewport> ViewportWidget = GEngine->GameViewport->GetGameViewportWidget();
			if (!ViewportWidget.IsValid())
			{
				UE_LOG(LogRogueRigs, Warning, TEXT("rr.Debug.Cursor: no game viewport widget."));
				return;
			}
			const FGeometry& Geometry = ViewportWidget->GetTickSpaceGeometry();
			const FVector2D Fraction(FCString::Atof(*Args[0]), FCString::Atof(*Args[1]));
			const FVector2D Local = Fraction * FVector2D(Geometry.GetLocalSize());
			MoveCursor(FVector2D(Geometry.LocalToAbsolute(FVector2f(Local))), FString::Printf(TEXT("%.3f %.3f"), Fraction.X, Fraction.Y));
		}),
		ECVF_Cheat);

	/** rr.Debug.CursorOn <ButtonName> [Index]: cursor at the centre of the visible game button of that widget name (Index-th from the left). */
	FAutoConsoleCommand CursorOnCommand(
		TEXT("rr.Debug.CursorOn"),
		TEXT("rr.Debug.CursorOn <ButtonName> [Index=0]: moves the mouse cursor to the centre of the on-screen button with that widget name (several: Index-th from the left, e.g. BuyButton 2)."),
		FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
		{
			if (Args.Num() < 1 || !FSlateApplication::IsInitialized())
			{
				UE_LOG(LogRogueRigs, Warning, TEXT("rr.Debug.CursorOn: usage rr.Debug.CursorOn <ButtonName> [Index]."));
				return;
			}
			const FName Name(*Args[0]);
			const int32 Index = Args.Num() > 1 ? FCString::Atoi(*Args[1]) : 0;
			TArray<TPair<FVector2D, FString>> Found;
			for (TObjectIterator<UButton> It; It; ++It)
			{
				UButton* Button = *It;
				const UWorld* World = Button->GetWorld();
				if (Button->GetFName() != Name || !World || !World->IsGameWorld() || !Button->GetCachedWidget().IsValid() || !Button->IsVisible())
				{
					continue;
				}
				const FGeometry& Geometry = Button->GetCachedGeometry();
				if (Geometry.GetLocalSize().IsNearlyZero())
				{
					continue;
				}
				Found.Emplace(FVector2D(Geometry.GetAbsolutePositionAtCoordinates(FVector2f(0.5f, 0.5f))), Button->GetPathName());
			}
			Found.Sort([](const TPair<FVector2D, FString>& A, const TPair<FVector2D, FString>& B) { return A.Key.X < B.Key.X; });
			if (!Found.IsValidIndex(Index))
			{
				UE_LOG(LogRogueRigs, Warning, TEXT("rr.Debug.CursorOn %s %d: %d visible button(s) of that name."), *Name.ToString(), Index, Found.Num());
				return;
			}
			MoveCursor(Found[Index].Key, FString::Printf(TEXT("on %s %d of %d"), *Name.ToString(), Index, Found.Num()));
		}),
		ECVF_Cheat);

	/** rr.Debug.ButtonHover: logs the hover state of every attached button. */
	FAutoConsoleCommand ButtonHoverCommand(
		TEXT("rr.Debug.ButtonHover"),
		TEXT("rr.Debug.ButtonHover: logs every button with the hover of ui-button-hover (owner, name, hovered, enabled, eased amount, plate mode)."),
		FConsoleCommandDelegate::CreateLambda([]()
		{
			for (const FWorldContext& Context : GEngine->GetWorldContexts())
			{
				UGameInstance* GameInstance = Context.OwningGameInstance;
				if (URRButtonHoverSubsystem* Subsystem = GameInstance ? GameInstance->GetSubsystem<URRButtonHoverSubsystem>() : nullptr)
				{
					Subsystem->LogEntries();
				}
			}
		}),
		ECVF_Cheat);

	/** rr.Debug.PressKey <Key>: the first player controller receives Key pressed, then released 0.1 s later (real time). */
	FAutoConsoleCommand PressKeyCommand(
		TEXT("rr.Debug.PressKey"),
		TEXT("rr.Debug.PressKey <Key>: simulates a press of Key (e.g. SpaceBar, Enter) on the first player controller, released 0.1 s later."),
		FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
		{
			UWorld* World = RRCheats::FindCheatWorld();
			APlayerController* Controller = World ? World->GetFirstPlayerController() : nullptr;
			const FKey Key(Args.Num() > 0 ? FName(*Args[0]) : FName(TEXT("SpaceBar")));
			if (!Controller || !Key.IsValid())
			{
				UE_LOG(LogRogueRigs, Warning, TEXT("rr.Debug.PressKey: no player controller or unknown key."));
				return;
			}
			Controller->InputKey(FInputKeyEventArgs::CreateSimulated(Key, IE_Pressed, 1.f));
			UE_LOG(LogRogueRigs, Log, TEXT("rr.Debug.PressKey: %s pressed."), *Key.ToString());
			TWeakObjectPtr<APlayerController> WeakController = Controller;
			FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([WeakController, Key](float)
			{
				if (APlayerController* PC = WeakController.Get())
				{
					PC->InputKey(FInputKeyEventArgs::CreateSimulated(Key, IE_Released, 0.f));
				}
				return false;
			}), 0.1f);
		}),
		ECVF_Cheat);
}

#endif
