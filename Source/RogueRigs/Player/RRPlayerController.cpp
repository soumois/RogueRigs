// Rogue Rigs - player controller: Enhanced Input setup, mouse cursor, routes input to the miner.

#include "Player/RRPlayerController.h"

#include "Engine/LocalPlayer.h"
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

ARRPlayerController::ARRPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bShowMouseCursor = true;

	PlaceTurretKeys = { EKeys::E, EKeys::RightMouseButton, EKeys::Gamepad_FaceButton_Bottom };
}

void ARRPlayerController::BeginPlay()
{
	Super::BeginPlay();

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
	if (ARRMiner* Miner = GetMiner())
	{
		Miner->ApplyMoveInput(Value.Get<FVector2D>());
	}
}

void ARRPlayerController::HandleStickAim(const FInputActionValue& Value)
{
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
	if (ARRMiner* Miner = GetMiner())
	{
		Miner->UseMouseAim();
	}

	SetShowMouseCursor(true);
}

void ARRPlayerController::HandlePlaceTurret(const FInputActionValue& Value)
{
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
