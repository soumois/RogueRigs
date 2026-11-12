// Rogue Rigs - player controller: Enhanced Input setup, mouse cursor, routes input to the miner.

#include "Player/RRPlayerController.h"

#include "EngineUtils.h"
#include "Engine/LocalPlayer.h"
#include "Enemies/RREnemySpawner.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "Objectives/RRRunFlowSubsystem.h"
#include "Player/RRGameMode.h"
#include "Player/RRMiner.h"
#include "RogueRigs.h"
#include "UI/RRHUDWidget.h"
#include "UI/RRIntermissionWidget.h"
#include "Engine/GameViewportClient.h"
#include "UI/RRLoadingScreenWidget.h"
#include "UI/RRPauseMenuWidget.h"
#include "UI/RROptionsWidget.h"
#include "UI/RRRunLostWidget.h"
#include "UI/RRScreenFadeWidget.h"
#include "UI/RRShopScreenWidget.h"
#include "Weapons/RRWeapon.h"

ARRPlayerController::ARRPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bShowMouseCursor = true;

	PlaceTurretKeys = { EKeys::E, EKeys::RightMouseButton, EKeys::Gamepad_FaceButton_Bottom };
	// pause-menu gdd (task-424 Q2): Escape, P (Escape stops a Play In Editor session), gamepad Start.
	PauseMenuKeys = { EKeys::Escape, EKeys::P, EKeys::Gamepad_Special_Right };
}

void ARRPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Level transition screens (task-206-03): bound for a local player even without a HUD class (headless runs create no widget).
	if (IsLocalPlayerController() && GetLocalPlayer())
	{
		if (URRRunTransitionSubsystem* Transition = URRRunTransitionSubsystem::Get(this))
		{
			BoundTransition = Transition;
			Transition->OnIntermissionStarted.AddUniqueDynamic(this, &ARRPlayerController::HandleIntermissionStarted);
			Transition->OnShopScreenStarted.AddUniqueDynamic(this, &ARRPlayerController::HandleShopScreenStarted);
			Transition->OnRunWonScreenStarted.AddUniqueDynamic(this, &ARRPlayerController::HandleRunWonScreenStarted);
			Transition->OnLoadingStarted.AddUniqueDynamic(this, &ARRPlayerController::HandleLoadingStarted);
			Transition->OnLevelReady.AddUniqueDynamic(this, &ARRPlayerController::HandleLevelReady);
			Transition->OnNextLevelReady.AddUniqueDynamic(this, &ARRPlayerController::HandleNextLevelReady);
			Transition->OnScreenFadeStarted.AddUniqueDynamic(this, &ARRPlayerController::HandleScreenFadeStarted);
			Transition->OnScreenFadeFinished.AddUniqueDynamic(this, &ARRPlayerController::HandleScreenFadeFinished);
			// A widget does not survive OpenLevel: the world opened by a transition shows the loading screen again until OnLevelReady
			// (higher Z-order than the HUD created below).
			if (Transition->IsLevelLoading())
			{
				// GetLoadingLayoutName: the next layout after Continue, level 1's layout after the main menu New Run (task-379-01).
				ShowLoadingScreen(Transition->GetLoadingLevelIndex(), Transition->GetLoadingLayoutName());
			}
		}
	}

	// Run-lost screen (task-484): death after its sequence, or missed shuttle.
	if (IsLocalPlayerController() && GetLocalPlayer())
	{
		if (URRRunLostSubsystem* RunLost = URRRunLostSubsystem::Get(this))
		{
			BoundRunLost = RunLost;
			RunLost->OnRunLost.AddUniqueDynamic(this, &ARRPlayerController::HandleRunLost);
		}
	}

	RestoreGameInputMode();
	CreateHUDWidget();

	// The world opened by a transition starts behind the loading screen: no HUD until OnLevelReady (task-207-01).
	const URRRunTransitionSubsystem* Transition = BoundTransition.Get();
	if (Transition && Transition->GetPhase() != ERRRunTransitionPhase::None)
	{
		SetHUDHiddenForTransition(true, Transition->IsLevelLoading() ? TEXT("loading") : TEXT("intermission"));
	}
}

void ARRPlayerController::CreateHUDWidget()
{
	// Headless runs and non-local controllers have no viewport: no HUD.
	if (!HUDWidgetClass || !IsLocalPlayerController() || !GetLocalPlayer())
	{
		return;
	}

	HUDWidget = CreateWidget<URRHUDWidget>(this, HUDWidgetClass);
	if (!HUDWidget)
	{
		UE_LOG(LogRogueRigs, Error, TEXT("%s: could not create the HUD widget %s."), *GetName(), *GetNameSafe(HUDWidgetClass));
		return;
	}
	HUDWidget->AddToViewport();
	HUDWidget->BindToGameMode(GetWorld()->GetAuthGameMode<ARRGameMode>());
	HUDWidget->BindToMiner(GetMiner());
	HUDWidget->BindToRunFlow(GetWorld()->GetSubsystem<URRRunFlowSubsystem>());
	// Horde banner and alert-bar horde icons: the level's spawner (one per level).
	TActorIterator<ARREnemySpawner> SpawnerIt(GetWorld());
	HUDWidget->BindToSpawner(SpawnerIt ? *SpawnerIt : nullptr);
}

void ARRPlayerController::HandleNextLevelReady(int32 LevelIndex, const FString& LayoutName)
{
	if (!HUDWidget)
	{
		return;
	}
	// The old HUD's visibility before the transition is kept for the new one (restored on OnLevelReady).
	const bool bWasHidden = bHUDHiddenForTransition;
	HUDWidget->RemoveFromParent();
	HUDWidget = nullptr;
	CreateHUDWidget();
	if (HUDWidget && bWasHidden)
	{
		HUDWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
	UE_LOG(LogRogueRigs, Log, TEXT("%s: HUD recreated for level %d prepared in this world (%s)."), *GetName(), LevelIndex,
		HUDWidget ? (bWasHidden ? TEXT("collapsed until level ready") : TEXT("visible")) : TEXT("no HUD"));
}

void ARRPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (URRRunTransitionSubsystem* Transition = BoundTransition.Get())
	{
		Transition->OnIntermissionStarted.RemoveDynamic(this, &ARRPlayerController::HandleIntermissionStarted);
		Transition->OnShopScreenStarted.RemoveDynamic(this, &ARRPlayerController::HandleShopScreenStarted);
		Transition->OnRunWonScreenStarted.RemoveDynamic(this, &ARRPlayerController::HandleRunWonScreenStarted);
		Transition->OnLoadingStarted.RemoveDynamic(this, &ARRPlayerController::HandleLoadingStarted);
		Transition->OnLevelReady.RemoveDynamic(this, &ARRPlayerController::HandleLevelReady);
		Transition->OnNextLevelReady.RemoveDynamic(this, &ARRPlayerController::HandleNextLevelReady);
		Transition->OnScreenFadeStarted.RemoveDynamic(this, &ARRPlayerController::HandleScreenFadeStarted);
		Transition->OnScreenFadeFinished.RemoveDynamic(this, &ARRPlayerController::HandleScreenFadeFinished);
	}
	BoundTransition.Reset();
	if (URRRunLostSubsystem* RunLost = BoundRunLost.Get())
	{
		RunLost->OnRunLost.RemoveDynamic(this, &ARRPlayerController::HandleRunLost);
	}
	BoundRunLost.Reset();
	RemoveScreenFadeOverlay();
	Super::EndPlay(EndPlayReason);
}

void ARRPlayerController::RestoreGameInputMode()
{
	if (!IsLocalPlayerController())
	{
		return;
	}
	FInputModeGameOnly InputMode;
	InputMode.SetConsumeCaptureMouseDown(false);
	SetInputMode(InputMode);
	SetShowMouseCursor(GetClass()->GetDefaultObject<APlayerController>()->bShowMouseCursor);
}

bool ARRPlayerController::IsGameplayInputHeld() const
{
	const URRRunTransitionSubsystem* Transition = BoundTransition.Get();
	return Transition && Transition->IsLevelLoading();
}

void ARRPlayerController::HandleIntermissionStarted(const FRRIntermissionRecap& Recap)
{
	SetHUDHiddenForTransition(true, TEXT("intermission"));
	RemoveShopScreenWidget();
	if (!IntermissionWidgetClass)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("%s: stats screen after level %d but IntermissionWidgetClass is not set; run rr.Run.Stats.Next to go on."),
			*GetName(), Recap.FinishedLevelIndex);
		return;
	}
	RemoveIntermissionWidget();
	IntermissionWidget = CreateWidget<URRIntermissionWidget>(this, IntermissionWidgetClass);
	if (!IntermissionWidget)
	{
		UE_LOG(LogRogueRigs, Error, TEXT("%s: could not create the intermission widget %s."), *GetName(), *GetNameSafe(IntermissionWidgetClass));
		return;
	}
	IntermissionWidget->AddToViewport(TransitionWidgetZOrder);
	IntermissionWidget->ShowRecap(Recap);
}

void ARRPlayerController::HandleLoadingStarted(int32 LevelIndex, const FString& LayoutName)
{
	SetHUDHiddenForTransition(true, TEXT("loading"));
	RemoveIntermissionWidget();
	RemoveShopScreenWidget();
	RemoveRunLostWidget();
	ShowLoadingScreen(LevelIndex, LayoutName);
}

void ARRPlayerController::HandleRunLost(const FRRRunLostRecap& Recap)
{
	// HUD hidden behind the screen (death-screen gdd, Buttons); the old end screen panels are inside it.
	SetHUDHiddenForTransition(true, TEXT("run lost"));
	if (!RunLostWidgetClass || !GetLocalPlayer())
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("%s: run lost but RunLostWidgetClass is not set; run rr.Run.Lost.Retry or rr.Run.Lost.MainMenu to go on."), *GetName());
		return;
	}
	RemoveRunLostWidget();
	RunLostWidget = CreateWidget<URRRunLostWidget>(this, RunLostWidgetClass);
	if (!RunLostWidget)
	{
		UE_LOG(LogRogueRigs, Error, TEXT("%s: could not create the run-lost widget %s."), *GetName(), *GetNameSafe(RunLostWidgetClass));
		return;
	}
	RunLostWidget->AddToViewport(RunLostWidgetZOrder);
	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetWidgetToFocus(RunLostWidget->TakeWidget());
	SetInputMode(InputMode);
	SetShowMouseCursor(true);
	RunLostWidget->ShowRunLost(Recap);
	UE_LOG(LogRogueRigs, Log, TEXT("%s: run-lost screen shown (%s)."), *GetName(), Recap.bMinerDied ? TEXT("miner died") : TEXT("shuttle missed"));
}

void ARRPlayerController::RemoveRunLostWidget()
{
	if (RunLostWidget)
	{
		RunLostWidget->RemoveFromParent();
		RunLostWidget = nullptr;
	}
}

void ARRPlayerController::HandleShopScreenStarted()
{
	RemoveIntermissionWidget();
	RemoveShopScreenWidget();
	if (!ShopScreenWidgetClass)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("%s: shop screen but ShopScreenWidgetClass is not set; run rr.Run.Continue to go on."), *GetName());
		return;
	}
	ShopScreenWidget = CreateWidget<URRShopScreenWidget>(this, ShopScreenWidgetClass);
	if (!ShopScreenWidget)
	{
		UE_LOG(LogRogueRigs, Error, TEXT("%s: could not create the shop screen widget %s."), *GetName(), *GetNameSafe(ShopScreenWidgetClass));
		return;
	}
	ShopScreenWidget->AddToViewport(TransitionWidgetZOrder);
	UE_LOG(LogRogueRigs, Log, TEXT("%s: shop screen shown."), *GetName());
}

void ARRPlayerController::HandleRunWonScreenStarted()
{
	RemoveIntermissionWidget();
	RemoveShopScreenWidget();
	// The run won end screen is part of the HUD widget (URRHUDWidget, OnRunEnded).
	SetHUDHiddenForTransition(false, TEXT("run won end screen"));
}

void ARRPlayerController::RemoveShopScreenWidget()
{
	if (ShopScreenWidget)
	{
		ShopScreenWidget->RemoveFromParent();
		ShopScreenWidget = nullptr;
	}
}

void ARRPlayerController::HandleLevelReady(int32 LevelIndex, const FString& LayoutName)
{
	RemoveLoadingScreenWidget();
	RestoreGameInputMode();
	// Fade path (task-318-02): no OnLoadingStarted, the cards screen stayed under the black until this switch.
	RemoveIntermissionWidget();
	RemoveShopScreenWidget();
	SetHUDHiddenForTransition(false, TEXT("level ready"));
}

void ARRPlayerController::HandleScreenFadeStarted(ERRTransitionFade Fade, float Duration)
{
	AddScreenFadeOverlay();
	UE_LOG(LogRogueRigs, Log, TEXT("%s: screen fade %s over %.2f s (%s)."), *GetName(), Fade == ERRTransitionFade::FadingOut ? TEXT("out") : TEXT("in"),
		Duration, ScreenFadeWidget ? TEXT("overlay in the viewport") : TEXT("no viewport, nothing drawn"));
}

void ARRPlayerController::HandleScreenFadeFinished(ERRTransitionFade Fade)
{
	// After the fade out the overlay stays at full black until the fade in that follows in the same frame.
	if (Fade == ERRTransitionFade::FadingIn)
	{
		RemoveScreenFadeOverlay();
	}
}

void ARRPlayerController::AddScreenFadeOverlay()
{
	const URRRunTransitionSubsystem* Transition = BoundTransition.Get();
	if (!IsLocalPlayerController() || !GetLocalPlayer() || !GetWorld() || !GetWorld()->GetGameViewport())
	{
		return;
	}
	if (!ScreenFadeWidget)
	{
		ScreenFadeWidget = CreateWidget<URRScreenFadeWidget>(this, URRScreenFadeWidget::StaticClass());
		if (!ScreenFadeWidget)
		{
			return;
		}
		ScreenFadeWidget->AddToViewport(ScreenFadeZOrder);
	}
	ScreenFadeWidget->SetFadeAlpha(Transition ? Transition->GetScreenFadeAlpha() : 0.f);
	if (!ScreenFadeTickHandle.IsValid())
	{
		// The world is paused during the fade out: the core ticker still runs every frame.
		TWeakObjectPtr<ARRPlayerController> WeakThis(this);
		ScreenFadeTickHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([WeakThis](float)
		{
			ARRPlayerController* Self = WeakThis.Get();
			if (!Self || !Self->ScreenFadeWidget)
			{
				return false;
			}
			const URRRunTransitionSubsystem* Transition = Self->BoundTransition.Get();
			Self->ScreenFadeWidget->SetFadeAlpha(Transition ? Transition->GetScreenFadeAlpha() : 0.f);
			return true;
		}));
	}
}

void ARRPlayerController::RemoveScreenFadeOverlay()
{
	if (ScreenFadeTickHandle.IsValid())
	{
		FTSTicker::GetCoreTicker().RemoveTicker(ScreenFadeTickHandle);
		ScreenFadeTickHandle.Reset();
	}
	if (ScreenFadeWidget)
	{
		ScreenFadeWidget->RemoveFromParent();
		ScreenFadeWidget = nullptr;
	}
}

void ARRPlayerController::SetHUDHiddenForTransition(bool bHideHUD, const TCHAR* Reason)
{
	if (!HUDWidget || bHUDHiddenForTransition == bHideHUD)
	{
		return;
	}
	bHUDHiddenForTransition = bHideHUD;
	if (bHideHUD)
	{
		HUDVisibilityBeforeTransition = HUDWidget->GetVisibility();
		HUDWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		HUDWidget->SetVisibility(HUDVisibilityBeforeTransition);
	}
	UE_LOG(LogRogueRigs, Log, TEXT("%s: HUD %s (%s)."), *GetName(), bHideHUD ? TEXT("hidden") : TEXT("restored"), Reason);
}

void ARRPlayerController::ShowLoadingScreen(int32 LevelIndex, const FString& LayoutName)
{
	if (!LoadingScreenWidgetClass)
	{
		UE_LOG(LogRogueRigs, Log, TEXT("%s: loading level %d without a loading screen (LoadingScreenWidgetClass not set)."), *GetName(), LevelIndex);
		return;
	}
	if (!LoadingScreenWidget)
	{
		LoadingScreenWidget = CreateWidget<URRLoadingScreenWidget>(this, LoadingScreenWidgetClass);
		if (!LoadingScreenWidget)
		{
			UE_LOG(LogRogueRigs, Error, TEXT("%s: could not create the loading screen widget %s."), *GetName(), *GetNameSafe(LoadingScreenWidgetClass));
			return;
		}
		LoadingScreenWidget->AddToViewport(TransitionWidgetZOrder + 1);
	}
	LoadingScreenWidget->ShowLoading(LevelIndex, LayoutName);
	UE_LOG(LogRogueRigs, Log, TEXT("%s: loading screen shown for level %d (layout %s, progress %s)."), *GetName(), LevelIndex, *LayoutName,
		LoadingScreenWidget->IsLoadingProgressMeasured() ? TEXT("measured") : TEXT("not measured"));
}

void ARRPlayerController::RemoveIntermissionWidget()
{
	if (IntermissionWidget)
	{
		IntermissionWidget->RemoveFromParent();
		IntermissionWidget = nullptr;
	}
}

void ARRPlayerController::RemoveLoadingScreenWidget()
{
	if (LoadingScreenWidget)
	{
		LoadingScreenWidget->RemoveFromParent();
		LoadingScreenWidget = nullptr;
		UE_LOG(LogRogueRigs, Log, TEXT("%s: loading screen hidden."), *GetName());
	}
}

void ARRPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (HUDWidget)
	{
		HUDWidget->BindToMiner(Cast<ARRMiner>(InPawn));
	}
}

void ARRPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputLocalPlayerSubsystem* Subsystem = nullptr;
	if (IsLocalPlayerController())
	{
		Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
		if (Subsystem)
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, MappingContextPriority);
			}
			else
			{
				UE_LOG(LogRogueRigs, Warning, TEXT("%s: DefaultMappingContext is not set."), *GetName());
			}
		}
	}

	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent);
	if (!EnhancedInput)
	{
		UE_LOG(LogRogueRigs, Error, TEXT("%s: input component is not a UEnhancedInputComponent."), *GetName());
		return;
	}

	if (MoveAction)
	{
		EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARRPlayerController::HandleMove);
	}
	if (StickAimAction)
	{
		EnhancedInput->BindAction(StickAimAction, ETriggerEvent::Triggered, this, &ARRPlayerController::HandleStickAim);
	}
	if (MouseAimAction)
	{
		EnhancedInput->BindAction(MouseAimAction, ETriggerEvent::Triggered, this, &ARRPlayerController::HandleMouseAim);
	}
	if (UInputAction* PlaceTurret = ResolvePlaceTurretAction(Subsystem))
	{
		EnhancedInput->BindAction(PlaceTurret, ETriggerEvent::Started, this, &ARRPlayerController::HandlePlaceTurret);
	}
	if (UInputAction* PauseMenu = ResolvePauseMenuAction(Subsystem))
	{
		EnhancedInput->BindAction(PauseMenu, ETriggerEvent::Started, this, &ARRPlayerController::HandlePauseMenu);
	}
	if (!MoveAction || !StickAimAction || !MouseAimAction)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("%s: one or more input actions are not set (Move, StickAim, MouseAim)."), *GetName());
	}
}

ARRMiner* ARRPlayerController::GetMiner() const
{
	return GetPawn<ARRMiner>();
}

void ARRPlayerController::HandleMove(const FInputActionValue& Value)
{
	if (IsGameplayInputHeld())
	{
		return;
	}
	if (ARRMiner* Miner = GetMiner())
	{
		Miner->ApplyMoveInput(Value.Get<FVector2D>());
	}
}

void ARRPlayerController::HandleStickAim(const FInputActionValue& Value)
{
	if (IsGameplayInputHeld())
	{
		return;
	}
	ARRMiner* Miner = GetMiner();
	if (!Miner)
	{
		return;
	}

	Miner->ApplyStickAimInput(Value.Get<FVector2D>());

	if (bHideCursorWhileStickAiming && !Miner->IsUsingMouseAim())
	{
		SetShowMouseCursor(false);
	}
}

void ARRPlayerController::HandleMouseAim(const FInputActionValue& Value)
{
	if (IsGameplayInputHeld())
	{
		return;
	}
	if (ARRMiner* Miner = GetMiner())
	{
		Miner->UseMouseAim();
	}

	SetShowMouseCursor(true);
}

void ARRPlayerController::HandlePlaceTurret(const FInputActionValue& Value)
{
	if (IsGameplayInputHeld())
	{
		return;
	}
	if (ARRMiner* Miner = GetMiner())
	{
		Miner->PlaceTurret();
	}
}

UInputAction* ARRPlayerController::ResolvePlaceTurretAction(UEnhancedInputLocalPlayerSubsystem* Subsystem)
{
	if (PlaceTurretAction)
	{
		return PlaceTurretAction;
	}

	// No asset yet: a transient Boolean action and its own mapping context, so E / right mouse / gamepad work out of the box.
	if (!RuntimePlaceTurretAction)
	{
		RuntimePlaceTurretAction = NewObject<UInputAction>(this, TEXT("RuntimePlaceTurretAction"), RF_Transient);
	}
	if (Subsystem && PlaceTurretKeys.Num() > 0)
	{
		if (!RuntimePlaceTurretContext)
		{
			RuntimePlaceTurretContext = NewObject<UInputMappingContext>(this, TEXT("RuntimePlaceTurretContext"), RF_Transient);
			for (const FKey& Key : PlaceTurretKeys)
			{
				if (Key.IsValid())
				{
					RuntimePlaceTurretContext->MapKey(RuntimePlaceTurretAction, Key);
				}
			}
		}
		Subsystem->AddMappingContext(RuntimePlaceTurretContext, MappingContextPriority + 1);
	}
	return RuntimePlaceTurretAction;
}

void ARRPlayerController::HandlePauseMenu(const FInputActionValue& Value)
{
	TogglePauseMenu(TEXT("pause key"));
}

UInputAction* ARRPlayerController::ResolvePauseMenuAction(UEnhancedInputLocalPlayerSubsystem* Subsystem)
{
	if (PauseMenuAction)
	{
		if (!PauseMenuAction->bTriggerWhenPaused)
		{
			UE_LOG(LogRogueRigs, Warning, TEXT("%s: PauseMenuAction %s does not trigger when paused; the pause key will not close the pause menu."),
				*GetName(), *GetNameSafe(PauseMenuAction));
		}
		return PauseMenuAction;
	}

	// No asset (IMC_Miner is untouched): a transient Boolean action that still triggers while the game is paused, so the same key closes the menu.
	if (!RuntimePauseMenuAction)
	{
		RuntimePauseMenuAction = NewObject<UInputAction>(this, TEXT("RuntimePauseMenuAction"), RF_Transient);
		RuntimePauseMenuAction->bTriggerWhenPaused = true;
	}
	if (Subsystem && PauseMenuKeys.Num() > 0)
	{
		if (!RuntimePauseMenuContext)
		{
			RuntimePauseMenuContext = NewObject<UInputMappingContext>(this, TEXT("RuntimePauseMenuContext"), RF_Transient);
			for (const FKey& Key : PauseMenuKeys)
			{
				if (Key.IsValid())
				{
					RuntimePauseMenuContext->MapKey(RuntimePauseMenuAction, Key);
				}
			}
		}
		Subsystem->AddMappingContext(RuntimePauseMenuContext, MappingContextPriority + 2);
	}
	return RuntimePauseMenuAction;
}

bool ARRPlayerController::CanOpenPauseMenu(FString* OutReason) const
{
	auto Refuse = [OutReason](const TCHAR* Reason)
	{
		if (OutReason)
		{
			*OutReason = Reason;
		}
		return false;
	};

	const UWorld* World = GetWorld();
	if (!World || !World->IsGameWorld() || !IsLocalPlayerController())
	{
		return Refuse(TEXT("no local player in a game world"));
	}
	if (bPauseMenuOpen)
	{
		return Refuse(TEXT("already open"));
	}
	if (bPauseMenuExitRequested)
	{
		return Refuse(TEXT("leaving for the main menu"));
	}
	if (IsPaused())
	{
		return Refuse(TEXT("the game is already paused by another screen"));
	}
	if (const URRRunTransitionSubsystem* Transition = BoundTransition.Get())
	{
		if (Transition->GetPhase() != ERRRunTransitionPhase::None || Transition->IsLevelLoading())
		{
			return Refuse(TEXT("a level transition screen is up"));
		}
	}
	if (const URRRunFlowSubsystem* RunFlow = World->GetSubsystem<URRRunFlowSubsystem>())
	{
		if (RunFlow->IsRunEnded())
		{
			return Refuse(TEXT("the run has ended"));
		}
	}
	if (const ARRGameMode* GameMode = World->GetAuthGameMode<ARRGameMode>())
	{
		if (GameMode->IsGameOver())
		{
			return Refuse(TEXT("game over"));
		}
	}
	if (const ARRMiner* Miner = GetMiner())
	{
		if (Miner->IsDead())
		{
			return Refuse(TEXT("the miner is dead"));
		}
	}
	return true;
}

bool ARRPlayerController::TogglePauseMenu(const TCHAR* Source)
{
	// Options screen open over the pause menu (task-500-04): the pause key acts as its Escape (Back), the game stays paused.
	if (URROptionsWidget* Options = URROptionsWidget::FindOpen(GetWorld()); Options && bPauseMenuOpen)
	{
		Options->HandleEscape();
		return true;
	}
	return bPauseMenuOpen ? ClosePauseMenu(Source) : OpenPauseMenu(Source);
}

bool ARRPlayerController::OpenPauseMenu(const TCHAR* Source)
{
	FString Reason;
	if (!CanOpenPauseMenu(&Reason))
	{
		UE_LOG(LogRogueRigs, Log, TEXT("%s: pause menu open refused (%s): %s."), *GetName(), Source, *Reason);
		return false;
	}
	if (!SetPause(true))
	{
		UE_LOG(LogRogueRigs, Log, TEXT("%s: pause menu open refused (%s): the game mode does not allow pausing."), *GetName(), Source);
		return false;
	}
	bPauseMenuOpen = true;

	const FRRPauseMenuStats Stats = URRPauseMenuWidget::BuildStats(GetMiner());
	if (PauseMenuWidgetClass && GetLocalPlayer())
	{
		PauseMenuWidget = CreateWidget<URRPauseMenuWidget>(this, PauseMenuWidgetClass);
		if (PauseMenuWidget)
		{
			PauseMenuWidget->AddToViewport(PauseMenuWidgetZOrder);
			PauseMenuWidget->RefreshStats();
		}
		else
		{
			UE_LOG(LogRogueRigs, Error, TEXT("%s: could not create the pause menu widget %s."), *GetName(), *GetNameSafe(PauseMenuWidgetClass));
		}
	}

	// Same input setup as the level-up cards: UI clicks and keys, cursor shown, focus on Resume (pause-menu gdd, Opening).
	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	if (PauseMenuWidget)
	{
		InputMode.SetWidgetToFocus(PauseMenuWidget->GetResumeFocusWidget()->TakeWidget());
	}
	SetInputMode(InputMode);
	SetShowMouseCursor(true);
	if (PauseMenuWidget)
	{
		PauseMenuWidget->FocusResume();
	}

	UE_LOG(LogRogueRigs, Log, TEXT("%s: pause menu opened (%s), game paused, %s."), *GetName(), Source,
		PauseMenuWidget ? *FString::Printf(TEXT("widget %s"), *PauseMenuWidget->GetClass()->GetName())
			: (PauseMenuWidgetClass ? TEXT("widget creation failed") : TEXT("no widget (PauseMenuWidgetClass not set)")));
	UE_LOG(LogRogueRigs, Log, TEXT("%s: pause menu stats: %s."), *GetName(), *URRPauseMenuWidget::DescribeStats(Stats));
	return true;
}

bool ARRPlayerController::ClosePauseMenu(const TCHAR* Source)
{
	if (!bPauseMenuOpen || bPauseMenuExitRequested)
	{
		UE_LOG(LogRogueRigs, Log, TEXT("%s: pause menu Resume refused (%s): %s."), *GetName(), Source,
			bPauseMenuExitRequested ? TEXT("leaving for the main menu") : TEXT("the pause menu is not open"));
		return false;
	}
	bPauseMenuOpen = false;
	if (PauseMenuWidget)
	{
		PauseMenuWidget->RemoveFromParent();
		PauseMenuWidget = nullptr;
	}
	SetPause(false);
	RestoreGameInputMode();
	UE_LOG(LogRogueRigs, Log, TEXT("%s: pause menu Resume (%s): menu closed, game resumed."), *GetName(), Source);
	return true;
}

bool ARRPlayerController::ExitPauseMenuToMainMenu(const TCHAR* Source)
{
	if (!bPauseMenuOpen || bPauseMenuExitRequested)
	{
		UE_LOG(LogRogueRigs, Log, TEXT("%s: pause menu Exit refused (%s): %s."), *GetName(), Source,
			bPauseMenuExitRequested ? TEXT("already leaving for the main menu") : TEXT("the pause menu is not open"));
		return false;
	}
	URRRunFlowSubsystem* RunFlow = GetWorld() ? GetWorld()->GetSubsystem<URRRunFlowSubsystem>() : nullptr;
	UE_LOG(LogRogueRigs, Log, TEXT("%s: pause menu Exit (%s): ending the run, back to the main menu."), *GetName(), Source);
	// The run won "Main Menu" path (main-menu gdd, task-424 Q1): run reset, ore back to 0, menu map opened directly.
	if (!RunFlow || !RunFlow->ReturnToMainMenu())
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("%s: pause menu Exit refused (%s): the run flow could not open the main menu; the menu stays open."),
			*GetName(), Source);
		return false;
	}
	bPauseMenuExitRequested = true;
	return true;
}
