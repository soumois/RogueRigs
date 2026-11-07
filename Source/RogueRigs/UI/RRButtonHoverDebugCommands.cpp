// Rogue Rigs - button hover debug console command (task-485, compiled out of shipping): rr.Debug.Cursor <fx> <fy> moves the real
// mouse cursor to a fraction of the game viewport, so a -game capture can show a button hovered by the mouse (ui-button-hover).

#include "CoreMinimal.h"

#if !UE_BUILD_SHIPPING

#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Framework/Application/SlateApplication.h"
#include "HAL/IConsoleManager.h"
#include "RogueRigs.h"
#include "Widgets/SViewport.h"

namespace RRButtonHoverDebug
{
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
			const FVector2D Absolute = FVector2D(Geometry.LocalToAbsolute(FVector2f(Local)));
			FSlateApplication::Get().SetCursorPos(Absolute);
			UE_LOG(LogRogueRigs, Log, TEXT("rr.Debug.Cursor %.3f %.3f: cursor at screen (%.0f, %.0f)."), Fraction.X, Fraction.Y, Absolute.X, Absolute.Y);
		}),
		ECVF_Cheat);
}

#endif
