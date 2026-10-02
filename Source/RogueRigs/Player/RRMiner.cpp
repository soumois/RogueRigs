// Rogue Rigs - the miner: player character owning the player ASC, top-down camera, world-space move and aim.

#include "Player/RRMiner.h"

#include "Camera/CameraComponent.h"
#include "Drone/RRDrone.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "GAS/RRAbilitySystemComponent.h"
#include "GAS/RRBaseStatsLoader.h"
#include "GAS/RRHealthAttributeSet.h"
#include "GAS/RRMovementAttributeSet.h"
#include "GAS/RRPlayerAttributeSet.h"
#include "Grid/RRArenaGrid.h"
#include "Projectiles/RRCollisionChannels.h"
#include "RogueRigs.h"
#include "Weapons/RRWeapon.h"

ARRMiner::ARRMiner(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	// The pawn faces the aim, not the controller or the movement direction.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Ability system: attribute sets created as default subobjects of the miner are found and
	// registered by UAbilitySystemComponent::InitializeComponent.
	AbilitySystemComponent = CreateDefaultSubobject<URRAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	HealthSet = CreateDefaultSubobject<URRHealthAttributeSet>(TEXT("HealthSet"));
	MovementSet = CreateDefaultSubobject<URRMovementAttributeSet>(TEXT("MovementSet"));
	PlayerSet = CreateDefaultSubobject<URRPlayerAttributeSet>(TEXT("PlayerSet"));

	// Camera boom with a fixed world rotation.
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetUsingAbsoluteRotation(true);
	SpringArm->SetRelativeRotation(FRotator(CameraPitch, CameraYaw, 0.f));
	SpringArm->TargetArmLength = CameraArmLength;
	SpringArm->bDoCollisionTest = false;
	SpringArm->bUsePawnControlRotation = false;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 0.5f;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;
	Camera->SetFieldOfView(CameraFieldOfView);

	// Movement defaults (editable on the component). MaxWalkSpeed is driven by the MoveSpeed attribute.
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	Movement->bOrientRotationToMovement = false;
	Movement->GravityScale = 1.5f;
	Movement->MaxAcceleration = 1000.f;
	Movement->BrakingFrictionFactor = 1.f;
	Movement->bCanWalkOffLedges = false;

	// Projectiles (own or enemy) never block the capsule; Overlap keeps their overlap events (hits by overlap).
	GetCapsuleComponent()->SetCollisionResponseToChannel(RRCollisionChannels::Projectile, ECR_Overlap);
}

UAbilitySystemComponent* ARRMiner::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ARRMiner::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	SpringArm->SetRelativeRotation(FRotator(CameraPitch, CameraYaw, 0.f));
	SpringArm->TargetArmLength = CameraArmLength;
	Camera->SetFieldOfView(CameraFieldOfView);
}

void ARRMiner::BeginPlay()
{
	Super::BeginPlay();

	AimDirection = GetActorForwardVector().GetSafeNormal2D(UE_SMALL_NUMBER, FVector::ForwardVector);

	// Init order (sys-gas-core): actor info, then base stats once the three sets are on the ASC.
	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	MoveSpeedChangedHandle = AbilitySystemComponent
		->GetGameplayAttributeValueChangeDelegate(URRMovementAttributeSet::GetMoveSpeedAttribute())
		.AddUObject(this, &ARRMiner::HandleMoveSpeedChanged);

	if (!FRRBaseStatsLoader::LoadAndApplyPlayerBaseStats(*AbilitySystemComponent, BaseStatsRowName))
	{
		UE_LOG(LogRogueRigs, Error, TEXT("%s: base stats row '%s' could not be applied."), *GetName(), *BaseStatsRowName.ToString());
	}

	// Push the value even if no change was broadcast (e.g. same value as before).
	ApplyMoveSpeed(MovementSet->GetMoveSpeed());

	SpawnDrone();
	FindArenaGrid();
	SpawnStartingWeapons();
}

void ARRMiner::FindArenaGrid()
{
	TActorIterator<ARRArenaGrid> It(GetWorld());
	ArenaGrid = It ? *It : nullptr;
	if (!ArenaGrid)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("%s: no ARRArenaGrid in the world; digging is disabled."), *GetName());
	}
}

void ARRMiner::SpawnDrone()
{
	if (!DroneClass)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("%s: DroneClass is not set; no drone spawned."), *GetName());
		return;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	SpawnParameters.Instigator = this;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	Drone = GetWorld()->SpawnActor<ARRDrone>(DroneClass, GetActorTransform(), SpawnParameters);
	if (!Drone)
	{
		UE_LOG(LogRogueRigs, Error, TEXT("%s: drone of class %s could not be spawned."), *GetName(), *DroneClass->GetName());
		return;
	}

	Drone->SetOwnerMiner(this);
}

void ARRMiner::SpawnStartingWeapons()
{
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	SpawnParameters.Instigator = this;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// No socket: the capsule, which turns with the aim yaw, so the weapon points forward.
	const bool bAttachToCapsule = WeaponAttachSocketName.IsNone() || !GetMesh();
	USceneComponent* AttachParent = bAttachToCapsule ? GetRootComponent() : static_cast<USceneComponent*>(GetMesh());

	for (const TSubclassOf<ARRWeapon>& WeaponClass : StartingWeapons)
	{
		if (!WeaponClass)
		{
			UE_LOG(LogRogueRigs, Warning, TEXT("%s: empty entry in StartingWeapons skipped."), *GetName());
			continue;
		}

		// Acquisition order (sys-weapons): spawn and attach, then Acquire (actor info, CSV base values, auto-fire).
		ARRWeapon* Weapon = GetWorld()->SpawnActor<ARRWeapon>(WeaponClass, GetActorTransform(), SpawnParameters);
		if (!Weapon)
		{
			UE_LOG(LogRogueRigs, Error, TEXT("%s: weapon of class %s could not be spawned."), *GetName(), *WeaponClass->GetName());
			continue;
		}

		if (bAttachToCapsule)
		{
			Weapon->AttachToComponent(AttachParent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			Weapon->SetActorRelativeLocation(WeaponAttachOffset);
		}
		else
		{
			Weapon->AttachToComponent(AttachParent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
		}
		// The weapon is carried inside the capsule: never let it block the miner's own moves.
		GetCapsuleComponent()->IgnoreActorWhenMoving(Weapon, true);
		Weapon->Acquire(this);
		Weapons.Add(Weapon);
	}
}

void ARRMiner::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AbilitySystemComponent->RefreshAbilityActorInfo();
}

void ARRMiner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (MoveSpeedChangedHandle.IsValid())
	{
		AbilitySystemComponent
			->GetGameplayAttributeValueChangeDelegate(URRMovementAttributeSet::GetMoveSpeedAttribute())
			.Remove(MoveSpeedChangedHandle);
		MoveSpeedChangedHandle.Reset();
	}

	if (IsValid(Drone))
	{
		Drone->Destroy();
	}
	Drone = nullptr;

	for (ARRWeapon* Weapon : Weapons)
	{
		if (IsValid(Weapon))
		{
			Weapon->Destroy();
		}
	}
	Weapons.Reset();

	Super::EndPlay(EndPlayReason);
}

void ARRMiner::ApplyMoveSpeed(float NewMoveSpeed)
{
	GetCharacterMovement()->MaxWalkSpeed = NewMoveSpeed;
}

void ARRMiner::HandleMoveSpeedChanged(const FOnAttributeChangeData& ChangeData)
{
	ApplyMoveSpeed(ChangeData.NewValue);
}

float ARRMiner::GetPickupRadius() const
{
	if (!AbilitySystemComponent || !AbilitySystemComponent->HasAttributeSetForAttribute(URRPlayerAttributeSet::GetPickupRadiusAttribute()))
	{
		return 0.f;
	}
	return AbilitySystemComponent->GetNumericAttribute(URRPlayerAttributeSet::GetPickupRadiusAttribute());
}

void ARRMiner::NotifyPickupCollected(ERRPickupType Type)
{
	OnPickupCollected.Broadcast(Type);
}

FVector ARRMiner::ScreenInputToWorld(const FVector2D& Input) const
{
	// Screen up = camera forward flattened, screen right = camera right flattened.
	const FRotator CameraYawRotation(0.f, SpringArm->GetComponentRotation().Yaw, 0.f);
	const FVector ScreenUp = CameraYawRotation.RotateVector(FVector::ForwardVector);
	const FVector ScreenRight = CameraYawRotation.RotateVector(FVector::RightVector);
	return ScreenUp * Input.Y + ScreenRight * Input.X;
}

void ARRMiner::ApplyMoveInput(const FVector2D& Input)
{
	const FVector WorldInput = ScreenInputToWorld(Input).GetClampedToMaxSize(1.f);
	PendingMoveInput = WorldInput;
	AddMovementInput(WorldInput);
}

void ARRMiner::ApplyStickAimInput(const FVector2D& Input)
{
	if (Input.Size() <= StickAimMinInput)
	{
		return;
	}

	bUsingMouseAim = false;
	AimDirection = ScreenInputToWorld(Input).GetSafeNormal2D(UE_SMALL_NUMBER, AimDirection);
}

void ARRMiner::UseMouseAim()
{
	bUsingMouseAim = true;
}

void ARRMiner::UpdateMouseAim()
{
	const APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!PlayerController)
	{
		return;
	}

	FVector RayOrigin;
	FVector RayDirection;
	if (!PlayerController->DeprojectMousePositionToWorld(RayOrigin, RayDirection))
	{
		return;
	}

	// Horizontal plane at the miner's feet (+ offset).
	const FVector Feet = GetActorLocation() - FVector(0.f, 0.f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	const float PlaneZ = Feet.Z + MouseAimPlaneHeight;

	// A ray parallel to or pointing away from the plane never reaches it.
	if (FMath::IsNearlyZero(RayDirection.Z) || (PlaneZ - RayOrigin.Z) / RayDirection.Z <= 0.f)
	{
		return;
	}

	const FVector AimPoint = FMath::RayPlaneIntersection(RayOrigin, RayDirection, FPlane(FVector(0.f, 0.f, PlaneZ), FVector::UpVector));
	const FVector ToAim = AimPoint - GetActorLocation();
	AimDirection = ToAim.GetSafeNormal2D(UE_SMALL_NUMBER, AimDirection);
}

void ARRMiner::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bUsingMouseAim)
	{
		UpdateMouseAim();
	}

	const FRotator CurrentRotation = GetActorRotation();
	const FRotator TargetRotation(CurrentRotation.Pitch, AimDirection.Rotation().Yaw, CurrentRotation.Roll);
	SetActorRotation(FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaSeconds, AimRotationInterpSpeed));

	// Move input arrives before the pawn ticks (the controller ticks first); consume the value of this frame.
	const FVector MoveInput = PendingMoveInput;
	PendingMoveInput = FVector::ZeroVector;
	if (MoveInput.Size2D() > DigMinMoveInput)
	{
		UpdateDigTrigger(MoveInput);
	}
}

void ARRMiner::UpdateDigTrigger(const FVector& MoveDirection)
{
	if (!IsValid(ArenaGrid) || !IsValid(Drone))
	{
		return;
	}

	// Dominant axis of the move direction in grid space: a diagonal push picks one neighbour.
	const FVector GridDirection = ArenaGrid->GetActorTransform().InverseTransformVectorNoScale(MoveDirection);
	FIntPoint Step(0, 0);
	if (FMath::Abs(GridDirection.X) >= FMath::Abs(GridDirection.Y))
	{
		Step.X = GridDirection.X >= 0.0 ? 1 : -1;
	}
	else
	{
		Step.Y = GridDirection.Y >= 0.0 ? 1 : -1;
	}

	const FIntPoint MinerCell = ArenaGrid->GetCellAtWorld(GetActorLocation());
	const FIntPoint TargetCell = MinerCell + Step;
	if (!ArenaGrid->IsBlockCell(TargetCell))
	{
		return;
	}

	// Distance from the miner to the block face that looks back at it, measured along the push axis.
	const FIntPoint MinerSide(-Step.X, -Step.Y);
	const FVector FaceCenter = ArenaGrid->GetCellFaceCenter(TargetCell, MinerSide);
	const FVector BlockCenter = ArenaGrid->GetCellCenter(TargetCell.X, TargetCell.Y);
	const FVector TowardBlock = (BlockCenter - FaceCenter).GetSafeNormal2D();
	const double DistanceToFace = FVector::DotProduct(FaceCenter - GetActorLocation(), TowardBlock);
	const double Reach = GetCapsuleComponent()->GetScaledCapsuleRadius() + DigReachMargin;
	if (DistanceToFace > Reach)
	{
		return;
	}

	Drone->RequestDig(ArenaGrid, TargetCell, MinerSide);
}
