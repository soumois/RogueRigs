// Rogue Rigs - debug command of the shared screen entrance (task-547): a simulated key press through the player input, to check the
// skip by a key in a -game capture.

#include "CoreMinimal.h"
#include "Containers/Ticker.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "HAL/IConsoleManager.h"
#include "InputCoreTypes.h"
#include "InputKeyEventArgs.h"
#include "RogueRigs.h"
#include "Debug/RRCheats.h"

namespace RRScreenEntranceDebug
{
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
