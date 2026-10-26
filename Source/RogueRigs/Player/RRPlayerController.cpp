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
#include "UI/RRLoadingScreenWidget.h"
#include "UI/RRShopScreenWidget.h"
#include "Weapons/RRWeapon.h"

ARRPlayerController::ARRPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bShowMouseCursor = true;

	PlaceTurretKeys = { EKeys::E, EKeys::RightMouseButton, EKeys::Gamepad_FaceButton_Bottom };
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
			// A widget does not survive OpenLevel: the world opened by a transition shows the loading screen again until OnLevelReady
			// (higher Z-order than the HUD created below).
			if (Transition->IsLevelLoading())
			{
				ShowLoadingScreen(Transition->GetLoadingLevelIndex(), Transition->GetIntermissionRecap().NextLayoutName);
			}
		}
	}

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
	}
	BoundTransition.Reset();
	Super::EndPlay(EndPlayReason);
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
	SetHUDHiddenForTransition(false, TEXT("level ready"));
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
	UE_LOG(LogRogueRigs, Log, TEXT("%s: loading screen shown for level %d."), *GetName(), LevelIndex);
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
