// Rogue Rigs - pause menu debug console command (task-424-03, compiled out of shipping): rr.Debug.PauseMenu open|resume|exit
// calls the same ARRPlayerController functions as the pause key and the Resume / Exit buttons.

#include "CoreMinimal.h"

#if !UE_BUILD_SHIPPING

#include "Debug/RRCheats.h"
#include "Engine/World.h"
#include "HAL/IConsoleManager.h"
#include "Kismet/GameplayStatics.h"
#include "Player/RRPlayerController.h"
#include "RogueRigs.h"
#include "UI/RRPauseMenuWidget.h"

namespace RRPauseMenuDebug
{
	/** rr.Debug.PauseMenu open|resume|exit: the pause key (open), the Resume button, the Exit button, on player 0's run controller. */
	FAutoConsoleCommand PauseMenuCommand(
		TEXT("rr.Debug.PauseMenu"),
		TEXT("rr.Debug.PauseMenu open|resume|exit|options|key|focuslost: opens the pause menu (as the pause key), presses Resume, presses Exit (run reset, main menu), ")
		TEXT("presses Options, sends the pause key (Back when the options screen is open), or simulates the window losing focus."),
		FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
		{
			const FString Action = Args.Num() > 0 ? Args[0].ToLower() : FString();
			if (Action != TEXT("open") && Action != TEXT("resume") && Action != TEXT("exit") && Action != TEXT("options")
				&& Action != TEXT("key") && Action != TEXT("focuslost"))
			{
				UE_LOG(LogRogueRigs, Warning, TEXT("rr.Debug.PauseMenu: usage rr.Debug.PauseMenu open|resume|exit|options|key|focuslost (got '%s')."), *Action);
				return;
			}
			UWorld* World = RRCheats::FindCheatWorld();
			ARRPlayerController* Controller = World ? Cast<ARRPlayerController>(UGameplayStatics::GetPlayerController(World, 0)) : nullptr;
			if (!Controller)
			{
				UE_LOG(LogRogueRigs, Log, TEXT("rr.Debug.PauseMenu %s: refused (no run player controller: main menu or no game world)."), *Action);
				return;
			}
			const TCHAR* Source = TEXT("rr.Debug.PauseMenu");
			URRPauseMenuWidget* Menu = Controller->GetPauseMenuWidget();
			const bool bAccepted = Action == TEXT("open") ? Controller->OpenPauseMenu(Source)
				: Action == TEXT("resume") ? Controller->ClosePauseMenu(Source)
				: Action == TEXT("options") ? (Menu && Menu->OpenOptions())
				: Action == TEXT("key") ? Controller->PressPauseKey()
				: Action == TEXT("focuslost") ? Controller->HandleWindowFocusLost()
				: Controller->ExitPauseMenuToMainMenu(Source);
			UE_LOG(LogRogueRigs, Log, TEXT("rr.Debug.PauseMenu %s: %s (menu open %d, options open %d, paused %d)."), *Action,
				bAccepted ? TEXT("accepted") : TEXT("refused"), Controller->IsPauseMenuOpen() ? 1 : 0,
				Controller->GetPauseMenuWidget() && Controller->GetPauseMenuWidget()->IsOptionsOpen() ? 1 : 0, Controller->IsPaused() ? 1 : 0);
		}),
		ECVF_Cheat);
}

#endif
