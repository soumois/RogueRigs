// Rogue Rigs - player controller: Enhanced Input setup, mouse cursor, routes input to the miner.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RRPlayerController.generated.h"

class ARRMiner;
class UInputAction;
class UInputMappingContext;
class URRHUDWidget;
struct FInputActionValue;

/**
 * Player controller of a run. Adds DefaultMappingContext to the local player's Enhanced Input subsystem,
 * binds the move and aim actions and forwards them to the possessed ARRMiner. Shows the mouse cursor;
 * hides it while a stick aims, shows it again on mouse movement. Assets are assigned in the Blueprint
 * subclass. Creates the HUD widget (HUDWidgetClass) for a local player. No gameplay logic here.
 */
UCLASS(Abstract)
class ROGUERIGS_API ARRPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	/** Shows the mouse cursor by default. */
	ARRPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** The HUD widget created at BeginPlay; null without a local player or without HUDWidgetClass. */
	URRHUDWidget* GetHUDWidget() const { return HUDWidget; }

protected:
	/** Local player only: creates HUDWidgetClass, adds it to the viewport and binds it to the game mode and the possessed miner. */
	virtual void BeginPlay() override;

	/** Rebinds the HUD widget to the newly possessed miner. */
	virtual void OnPossess(APawn* InPawn) override;

	/** Adds the mapping context (local controller only) and binds the input actions. */
	virtual void SetupInputComponent() override;

	/** Move action handler: forwards the Axis2D value to ARRMiner::ApplyMoveInput. */
	void HandleMove(const FInputActionValue& Value);

	/** Stick aim action handler: forwards the Axis2D value to ARRMiner::ApplyStickAimInput and hides the cursor. */
	void HandleStickAim(const FInputActionValue& Value);

	/** Mouse aim action handler: switches the miner to mouse aim and shows the cursor. */
	void HandleMouseAim(const FInputActionValue& Value);

	/** Returns the possessed pawn as a miner, or nullptr. */
	ARRMiner* GetMiner() const;

	/** Mapping context added at input setup; maps keys to the actions below. */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	/** Priority of DefaultMappingContext in the Enhanced Input subsystem. */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	int32 MappingContextPriority = 0;

	/** Axis2D move action, in screen axes (X = right, Y = up). Bound on Triggered. */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	/** Axis2D stick aim action (right stick), in screen axes (X = right, Y = up). Bound on Triggered. */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> StickAimAction;

	/** Mouse aim action (mouse XY movement); any trigger switches back to mouse aim. Bound on Triggered. */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> MouseAimAction;

	/** If true, the cursor is hidden while a stick aims and shown again when the mouse moves. */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	bool bHideCursorWhileStickAiming = true;

	/** HUD widget class created at BeginPlay (a Blueprint subclass of URRHUDWidget). None = no HUD. */
	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	TSubclassOf<URRHUDWidget> HUDWidgetClass;

	/** The HUD widget created from HUDWidgetClass. */
	UPROPERTY(Transient)
	TObjectPtr<URRHUDWidget> HUDWidget;
};
