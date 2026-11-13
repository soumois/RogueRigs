// Rogue Rigs - C++ base of the main menu screen (WBP_MainMenu, task-338-01): New Run, Options (disabled), Exit.

#include "Menu/RRMainMenuWidget.h"

#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "RogueRigs.h"
#include "UI/RRButtonHover.h"
#include "Run/RRRunSettings.h"
#include "Run/RRRunSubsystem.h"
#include "Run/RRRunTransitionSubsystem.h"
#include "Components/Button.h"
#include "UI/RROptionsWidget.h"

void URRMainMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	SetIsFocusable(true);
	// Plate buttons since task-487: the plate brightens and grows (ui-button-hover); no focus look since task-540.
	// Options active since task-500-04.
	if (OptionsButton)
	{
		OptionsButton->SetIsEnabled(IsOptionsAvailable());
		OptionsButton->OnClicked.AddUniqueDynamic(this, &URRMainMenuWidget::HandleOptionsClicked);
	}
	URRButtonHoverSubsystem::AttachNamed(this, {TEXT("NewRunButton"), TEXT("OptionsButton"), TEXT("ExitButton")});
}

bool URRMainMenuWidget::StartNewRun()
{
	if (bNewRunRequested)
	{
		return false;
	}
	bNewRunRequested = StartNewRunInWorld(GetWorld());
	return bNewRunRequested;
}

bool URRMainMenuWidget::IsOptionsAvailable() const
{
	return URROptionsWidget::IsOptionsAvailable();
}

bool URRMainMenuWidget::OpenOptions()
{
	return URROptionsWidget::OpenOptions(this) != nullptr;
}

void URRMainMenuWidget::HandleOptionsClicked()
{
	OpenOptions();
}

void URRMainMenuWidget::QuitGame()
{
	QuitGameInWorld(GetWorld());
}

bool URRMainMenuWidget::StartNewRunInWorld(UWorld* World)
{
	if (!World)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("Main menu New Run: refused (no world)."));
		return false;
	}
	const FString& MapName = GetDefault<URRRunSettings>()->NewRunMapName;
	if (MapName.IsEmpty())
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("Main menu New Run: refused (URRRunSettings::NewRunMapName is empty)."));
		return false;
	}
	// Loading screen path (task-379-01): run reset, loading phase for level 1, loading screen painted, then the travel.
	if (URRRunTransitionSubsystem* Transition = URRRunTransitionSubsystem::Get(World))
	{
		UE_LOG(LogRogueRigs, Log, TEXT("Main menu New Run: run reset and loading of %s (level 1) through the run transition."), *MapName);
		return Transition->StartNewRun(World, MapName);
	}
	// No game instance subsystem (should not happen in a game): direct travel as before.
	if (URRRunSubsystem* Run = URRRunSubsystem::Get(World))
	{
		Run->ResetRun();
	}
	UE_LOG(LogRogueRigs, Warning, TEXT("Main menu New Run: no run transition subsystem; run reset, opening %s (level 1) without a loading screen."), *MapName);
	UGameplayStatics::OpenLevel(World, FName(*MapName));
	return true;
}

void URRMainMenuWidget::QuitGameInWorld(UWorld* World)
{
	if (!World)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("Main menu Exit: refused (no world)."));
		return;
	}
	UE_LOG(LogRogueRigs, Log, TEXT("Main menu Exit: quitting the game."));
	UKismetSystemLibrary::QuitGame(World, UGameplayStatics::GetPlayerController(World, 0), EQuitPreference::Quit, false);
}
