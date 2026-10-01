// Rogue Rigs - player controller: Enhanced Input setup, mouse cursor, routes input to the miner.

#include "Player/RRPlayerController.h"

#include "Engine/LocalPlayer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "Player/RRMiner.h"
#include "RogueRigs.h"

ARRPlayerController::ARRPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bShowMouseCursor = true;
}

void ARRPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (IsLocalPlayerController())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
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
