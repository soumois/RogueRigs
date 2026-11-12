// Rogue Rigs - C++ base of the main menu screen (WBP_MainMenu, task-338-01): New Run, Options (task-505-03), Exit.

#include "Menu/RRMainMenuWidget.h"

#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "RogueRigs.h"
#include "Components/Button.h"
#include "UI/RRButtonHover.h"
#include "UI/RROptionsWidget.h"
#include "Run/RRRunSettings.h"
#include "Run/RRRunSubsystem.h"
#include "Run/RRRunTransitionSubsystem.h"

URRMainMenuWidget::URRMainMenuWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	OptionsWidgetClass = TSoftClassPtr<URROptionsWidget>(FSoftObjectPath(TEXT("/Game/RogueRigs/UI/WBP_Options.WBP_Options_C")));
}

void URRMainMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	SetIsFocusable(true);
	// Plate buttons since task-487: the plate brightens and grows (ui-button-hover); the orange focus outline is the widget's own
	// (RefreshColours). Options is active since task-505-03.
	URRButtonHoverSubsystem::AttachNamed(this, {TEXT("NewRunButton"), TEXT("OptionsButton"), TEXT("ExitButton")});
	if (UButton* OptionsButton = Cast<UButton>(GetWidgetFromName(TEXT("OptionsButton"))))
	{
		OptionsButton->SetIsEnabled(IsOptionsAvailable());
		OptionsButton->OnClicked.AddUniqueDynamic(this, &URRMainMenuWidget::HandleOptionsClicked);
		// Hover gives keyboard focus, as the graph's OnHovered does for New Run and Exit (MCP cannot add that bound event).
		OptionsButton->OnHovered.AddUniqueDynamic(this, &URRMainMenuWidget::HandleOptionsHovered);
	}
}

void URRMainMenuWidget::HandleOptionsHovered()
{
	if (UWidget* OptionsButton = GetWidgetFromName(TEXT("OptionsButton")))
	{
		OptionsButton->SetKeyboardFocus();
	}
}

bool URRMainMenuWidget::OpenOptions()
{
	if (OptionsWidget && OptionsWidget->IsInViewport())
	{
		return false;
	}
	UClass* Class = OptionsWidgetClass.LoadSynchronous();
	APlayerController* Controller = GetOwningPlayer();
	if (!Class || !Controller)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("Main menu Options: refused (class %s, controller %d)."), *OptionsWidgetClass.ToString(), Controller ? 1 : 0);
		return false;
	}
	OptionsWidget = CreateWidget<URROptionsWidget>(Controller, Class);
	if (!OptionsWidget)
	{
		return false;
	}
	OptionsWidget->OnOptionsClosed.AddUniqueDynamic(this, &URRMainMenuWidget::HandleOptionsClosed);
	OptionsWidget->AddToViewport(OptionsZOrder);
	OptionsWidget->FocusFirstTab();
	UE_LOG(LogRogueRigs, Log, TEXT("Main menu Options: options screen opened (%s)."), *Class->GetName());
	return true;
}

void URRMainMenuWidget::HandleOptionsClicked()
{
	OpenOptions();
}

void URRMainMenuWidget::HandleOptionsClosed()
{
	OptionsWidget = nullptr;
	if (UWidget* OptionsButton = GetWidgetFromName(TEXT("OptionsButton")))
	{
		OptionsButton->SetKeyboardFocus();
	}
	UE_LOG(LogRogueRigs, Log, TEXT("Main menu Options: closed, focus on Options."));
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
