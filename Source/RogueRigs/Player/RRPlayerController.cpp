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
#include "Misc/PackageName.h"
#include "Progression/RRLevelUpComponent.h"
#include "UI/RRLoadingScreenWidget.h"
#include "UI/RRPauseMenuWidget.h"
#include "UI/RRScreenFadeWidget.h"
#include "UI/RRShopScreenWidget.h"
#include "Weapons/RRWeapon.h"

ARRPlayerController::ARRPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bShowMouseCursor = true;

	PlaceTurretKeys = { EKeys::E, EKeys::RightMouseButton, EKeys::Gamepad_FaceButton_Bottom };

	// Pause menu (pause-menu gdd, task-424 Q2): Escape, P, gamepad Start; widget loaded by path on the first opening.
	PauseKeys = { EKeys::Escape, EKeys::P, EKeys::Gamepad_Special_Right };
	PauseMenuWidgetClass = TSoftClassPtr<URRPauseMenuWidget>(FSoftObjectPath(TEXT("/Game/RogueRigs/UI/WBP_PauseMenu.WBP_PauseMenu_C")));
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
	RemoveScreenFadeOverlay();
	if (PauseMenuWidget)
	{
		PauseMenuWidget->RemoveFromParent();
		PauseMenuWidget = nullptr;
	}
	bPauseMenuOpen = false;
	bPausedByPauseMenu = false;
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
	ShowLoadingScreen(LevelIndex, LayoutName);
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
	if (UInputAction* Pause = ResolvePauseAction(Subsystem))
	{
		EnhancedInput->BindAction(Pause, ETriggerEvent::Started, this, &ARRPlayerController::HandlePause);
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

void ARRPlayerController::HandlePause(const FInputActionValue& Value)
{
	TogglePauseMenu();
}

UInputAction* ARRPlayerController::ResolvePauseAction(UEnhancedInputLocalPlayerSubsystem* Subsystem)
{
	// No input asset (task-424-03): a transient Boolean action that also triggers while paused, so the same key closes the menu.
	if (!RuntimePauseAction)
	{
		RuntimePauseAction = NewObject<UInputAction>(this, TEXT("RuntimePauseAction"), RF_Transient);
		RuntimePauseAction->bTriggerWhenPaused = true;
	}
	if (Subsystem && PauseKeys.Num() > 0)
	{
		if (!RuntimePauseContext)
		{
			RuntimePauseContext = NewObject<UInputMappingContext>(this, TEXT("RuntimePauseContext"), RF_Transient);
			for (const FKey& Key : PauseKeys)
			{
				if (Key.IsValid())
				{
					RuntimePauseContext->MapKey(RuntimePauseAction, Key);
				}
			}
		}
		Subsystem->AddMappingContext(RuntimePauseContext, MappingContextPriority + 2);
	}
	return RuntimePauseAction;
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

	UWorld* World = GetWorld();
	if (!World || !IsLocalPlayerController())
	{
		return Refuse(TEXT("no local player"));
	}
	if (bPauseMenuOpen)
	{
		return Refuse(TEXT("already open"));
	}
	const ARRMiner* Miner = GetMiner();
	if (!Miner || Miner->IsDead())
	{
		return Refuse(Miner ? TEXT("miner dead") : TEXT("no miner"));
	}
	if (const ARRGameMode* GameMode = World->GetAuthGameMode<ARRGameMode>(); GameMode && GameMode->IsGameOver())
	{
		return Refuse(TEXT("game over"));
	}
	if (const URRRunTransitionSubsystem* Transition = BoundTransition.Get())
	{
		if (Transition->GetPhase() != ERRRunTransitionPhase::None || Transition->GetScreenFade() != ERRTransitionFade::None)
		{
			return Refuse(TEXT("level transition screen"));
		}
	}
	if (const URRRunFlowSubsystem* RunFlow = World->GetSubsystem<URRRunFlowSubsystem>(); RunFlow && RunFlow->IsRunEnded())
	{
		return Refuse(TEXT("run ended"));
	}
	if (const URRLevelUpComponent* LevelUp = Miner->GetLevelUp(); LevelUp && (LevelUp->IsAwaitingChoice() || LevelUp->IsChoiceDelayActive()))
	{
		return Refuse(TEXT("level-up cards"));
	}
	// Any other screen that paused the game first.
	if (IsPaused())
	{
		return Refuse(TEXT("game already paused by another screen"));
	}
	return true;
}

TSubclassOf<URRPauseMenuWidget> ARRPlayerController::LoadPauseMenuWidgetClass() const
{
	if (PauseMenuWidgetClass.IsNull())
	{
		return nullptr;
	}
	if (UClass* Loaded = PauseMenuWidgetClass.Get())
	{
		return Loaded;
	}
	// Check the package first: loading a missing asset would log a load error.
	const FString PackageName = PauseMenuWidgetClass.ToSoftObjectPath().GetLongPackageName();
	if (!FPackageName::DoesPackageExist(PackageName))
	{
		return nullptr;
	}
	return PauseMenuWidgetClass.LoadSynchronous();
}

bool ARRPlayerController::OpenPauseMenu()
{
	FString Reason;
	if (!CanOpenPauseMenu(&Reason))
	{
		UE_LOG(LogRogueRigs, Log, TEXT("%s: pause menu refused (%s)."), *GetName(), *Reason);
		return false;
	}

	bPausedByPauseMenu = SetPause(true);
	if (!bPausedByPauseMenu)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("%s: pause menu refused (the game mode refused the pause)."), *GetName());
		return false;
	}
	bPauseMenuOpen = true;

	const FRRPauseMenuStats Stats = URRPauseMenuWidget::BuildStatsSnapshot(GetMiner());
	UWidget* FocusTarget = nullptr;
	if (GetLocalPlayer() && GetWorld()->GetGameViewport())
	{
		const TSubclassOf<URRPauseMenuWidget> WidgetClass = LoadPauseMenuWidgetClass();
		if (!WidgetClass)
		{
			UE_LOG(LogRogueRigs, Warning, TEXT("%s: pause menu widget class %s not found; game paused without a menu (press the pause key or rr.Debug.PauseMenu resume)."),
				*GetName(), *PauseMenuWidgetClass.ToString());
		}
		else
		{
			PauseMenuWidget = CreateWidget<URRPauseMenuWidget>(this, WidgetClass);
			if (!PauseMenuWidget)
			{
				UE_LOG(LogRogueRigs, Error, TEXT("%s: could not create the pause menu widget %s."), *GetName(), *GetNameSafe(WidgetClass.Get()));
			}
			else
			{
				PauseMenuWidget->AddToViewport(PauseMenuZOrder);
				PauseMenuWidget->ShowPauseMenu(Stats);
				FocusTarget = PauseMenuWidget->GetWidgetFromName(PauseMenuWidget->GetResumeButtonName());
				if (!FocusTarget)
				{
					FocusTarget = PauseMenuWidget;
				}
			}
		}
	}

	// Game and UI: the pause key still reaches the game input (Slate buttons do not handle Escape, P or Start).
	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	if (FocusTarget)
	{
		InputMode.SetWidgetToFocus(FocusTarget->TakeWidget());
	}
	SetInputMode(InputMode);
	SetShowMouseCursor(true);

	UE_LOG(LogRogueRigs, Log, TEXT("%s: pause menu opened (%s), %d weapon block(s)."), *GetName(),
		PauseMenuWidget ? *GetNameSafe(PauseMenuWidget->GetClass()) : TEXT("no widget"), Stats.Weapons.Num());
	return true;
}

bool ARRPlayerController::ClosePauseMenu()
{
	if (!bPauseMenuOpen)
	{
		return false;
	}
	bPauseMenuOpen = false;
	if (PauseMenuWidget)
	{
		PauseMenuWidget->RemoveFromParent();
		PauseMenuWidget = nullptr;
	}
	if (bPausedByPauseMenu)
	{
		SetPause(false);
	}
	bPausedByPauseMenu = false;
	RestoreGameInputMode();
	UE_LOG(LogRogueRigs, Log, TEXT("%s: pause menu closed, game resumed."), *GetName());
	return true;
}

bool ARRPlayerController::TogglePauseMenu()
{
	return bPauseMenuOpen ? ClosePauseMenu() : OpenPauseMenu();
}
