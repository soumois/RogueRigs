// Rogue Rigs - main menu debug console commands (task-338-01, compiled out of shipping): rr.Menu.NewRun (the New Run button) and
// rr.Menu.Exit (the Exit button). Both call the shared URRMainMenuWidget actions, so they work with or without a menu widget.

#include "CoreMinimal.h"

#if !UE_BUILD_SHIPPING

#include "Debug/RRCheats.h"
#include "HAL/IConsoleManager.h"
#include "Menu/RRMainMenuWidget.h"
#include "RogueRigs.h"

namespace RRMainMenuDebug
{
	/** rr.Menu.NewRun: the main menu New Run button (run reset, URRRunSettings::NewRunMapName opened at level 1). */
	FAutoConsoleCommand NewRunCommand(
		TEXT("rr.Menu.NewRun"),
		TEXT("rr.Menu.NewRun: presses the main menu New Run button (run reset, the arena map opens at level 1)."),
		FConsoleCommandDelegate::CreateLambda([]()
		{
			const bool bAccepted = URRMainMenuWidget::StartNewRunInWorld(RRCheats::FindCheatWorld());
			UE_LOG(LogRogueRigs, Log, TEXT("rr.Menu.NewRun: %s."), bAccepted ? TEXT("accepted") : TEXT("refused"));
		}),
		ECVF_Cheat);

	/** rr.Menu.Exit: the main menu Exit button (quits the game). */
	FAutoConsoleCommand ExitCommand(
		TEXT("rr.Menu.Exit"),
		TEXT("rr.Menu.Exit: presses the main menu Exit button (quits the game)."),
		FConsoleCommandDelegate::CreateLambda([]()
		{
			UE_LOG(LogRogueRigs, Log, TEXT("rr.Menu.Exit: quitting."));
			URRMainMenuWidget::QuitGameInWorld(RRCheats::FindCheatWorld());
		}),
		ECVF_Cheat);
}

#endif
