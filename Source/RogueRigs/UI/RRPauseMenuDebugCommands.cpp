// Rogue Rigs - pause menu debug console command (task-424-03, compiled out of shipping): rr.Debug.PauseMenu open|resume|exit, the
// pause key and the Resume and Exit buttons through the same functions, for the capture checks.

#include "CoreMinimal.h"

#if !UE_BUILD_SHIPPING

#include "Debug/RRCheats.h"
#include "HAL/IConsoleManager.h"
#include "Kismet/GameplayStatics.h"
#include "Player/RRPlayerController.h"
#include "RogueRigs.h"
#include "UI/RRPauseMenuWidget.h"

namespace RRPauseMenuDebug
{
	/** rr.Debug.PauseMenu open|resume|exit: acts on player 0's ARRPlayerController in RRCheats::FindCheatWorld. */
	static void Run(const TArray<FString>& Args)
	{
		const FString Action = Args.Num() > 0 ? Args[0].ToLower() : FString();
		if (Action != TEXT("open") && Action != TEXT("resume") && Action != TEXT("exit"))
		{
			UE_LOG(LogRogueRigs, Warning, TEXT("rr.Debug.PauseMenu: usage rr.Debug.PauseMenu open|resume|exit."));
			return;
		}
		UWorld* World = RRCheats::FindCheatWorld();
		ARRPlayerController* Controller = World ? Cast<ARRPlayerController>(UGameplayStatics::GetPlayerController(World, 0)) : nullptr;
		if (!Controller)
		{
			UE_LOG(LogRogueRigs, Warning, TEXT("rr.Debug.PauseMenu %s: refused (no run player controller)."), *Action);
			return;
		}

		if (Action == TEXT("open"))
		{
			// The pause key path (opens only; the key toggles).
			const bool bOpened = Controller->OpenPauseMenu();
			UE_LOG(LogRogueRigs, Log, TEXT("rr.Debug.PauseMenu open: %s (widget %s)."), bOpened ? TEXT("accepted") : TEXT("refused"),
				Controller->GetPauseMenuWidget() ? *GetNameSafe(Controller->GetPauseMenuWidget()->GetClass()) : TEXT("none"));
			if (bOpened)
			{
				const URRPauseMenuWidget* Widget = Controller->GetPauseMenuWidget();
				URRPauseMenuWidget::LogStats(Widget ? Widget->GetStatsSnapshot() : URRPauseMenuWidget::BuildStatsSnapshot(RRCheats::FindPlayerMiner(World)),
					TEXT("rr.Debug.PauseMenu open"));
			}
		}
		else if (Action == TEXT("resume"))
		{
			// The Resume button (URRPauseMenuWidget::Resume) when the widget exists, else the same controller call.
			URRPauseMenuWidget* Widget = Controller->GetPauseMenuWidget();
			const bool bResumed = Widget ? Widget->Resume() : Controller->ClosePauseMenu();
			UE_LOG(LogRogueRigs, Log, TEXT("rr.Debug.PauseMenu resume: %s."), bResumed ? TEXT("accepted") : TEXT("refused (menu not open)"));
		}
		else
		{
			// The Exit button (URRPauseMenuWidget::ExitToMainMenu) when the widget exists, else the same shared static.
			URRPauseMenuWidget* Widget = Controller->GetPauseMenuWidget();
			const bool bExited = Widget ? Widget->ExitToMainMenu() : URRPauseMenuWidget::ExitToMainMenuInWorld(World);
			UE_LOG(LogRogueRigs, Log, TEXT("rr.Debug.PauseMenu exit: %s."), bExited ? TEXT("accepted") : TEXT("refused"));
		}
	}

	FAutoConsoleCommand PauseMenuCommand(
		TEXT("rr.Debug.PauseMenu"),
		TEXT("rr.Debug.PauseMenu open|resume|exit: opens the pause menu (pause key), presses Resume, or presses Exit (run reset, main menu)."),
		FConsoleCommandWithArgsDelegate::CreateStatic(&Run),
		ECVF_Cheat);
}

#endif
