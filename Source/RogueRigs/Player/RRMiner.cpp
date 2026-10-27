// Rogue Rigs - the miner: player character owning the player ASC, top-down camera, world-space move and aim.

#include "Player/RRMiner.h"

#include "AimIndicator/RRAimIndicatorComponent.h"
#include "Camera/CameraComponent.h"
#include "Damage/RRGE_IFrames.h"
#include "Drone/RRDrone.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "GAS/RRAbilitySystemComponent.h"
#include "GAS/RRBaseStatsLoader.h"
#include "GAS/RRGameplayTags.h"
#include "GAS/RRHealthAttributeSet.h"
#include "GAS/RRMovementAttributeSet.h"
#include "GAS/RRPlayerAttributeSet.h"
#include "Grid/RRArenaGrid.h"
#include "Objectives/RRRunFlowSubsystem.h"
#include "Player/RRGameMode.h"
#include "Progression/RRLevelUpComponent.h"
#include "Progression/RRProgressionComponent.h"
#include "Projectiles/RRCollisionChannels.h"
#include "Upgrades/RRUpgradeComponent.h"
#include "RogueRigs.h"
#include "Telegraph/RRGroundTelegraphComponent.h"
#include "Run/RRRunSubsystem.h"
#include "RunLog/RRRunLogSubsystem.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRShotgun.h"
#include "Weapons/RRTurretLauncher.h"
#include "Weapons/RRWeapon.h"
#include "World/RRWorld.h"
#include "World/RRBlobShadowComponent.h"
#include "World/RRPlayerLightMaskComponent.h"
#include "World/RRShadows.h"

ARRMiner::ARRMiner(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Weapon catalogue (upgrades gdd): the miner starts with the burst rifle only; these native defaults let new-weapon cards
	// work without Blueprints. BP_Miner replaces them with the Blueprint classes (rifle, shotgun, turret launcher).
	// Descriptions are provisional working texts.
	WeaponCatalogue.Add({ ARRShotgun::StaticClass(),
		NSLOCTEXT("RRMiner", "Catalogue_Shotgun", "A short-range spray of pellets that knocks robots back.") });
	WeaponCatalogue.Add({ ARRTurretLauncher::StaticClass(),
		NSLOCTEXT("RRMiner", "Catalogue_TurretLauncher", "Places turrets that fire at nearby robots on their own.") });

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
	Progression = CreateDefaultSubobject<URRProgressionComponent>(TEXT("Progression"));
	Upgrades = CreateDefaultSubobject<URRUpgradeComponent>(TEXT("Upgrades"));
	LevelUp = CreateDefaultSubobject<URRLevelUpComponent>(TEXT("LevelUp"));
	IFramesEffectClass = URRGE_IFrames::StaticClass();

	// Camera boom with a fixed world rotation.
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetUsingAbsoluteRotation(true);
	SpringArm->SetRelativeRotation(FRotator(CameraPitch, CameraYaw, 0.f));
	SpringArm->TargetArmLength = CameraArmLength;
	SpringArm->bDoCollisionTest = false;
	SpringArm->bUsePawnControlRotation = false;
	SpringArm->bEnableCameraLag = bCameraLag;
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

	// task-264-02: fake round shadow, placed and sized from the capsule in PostInitializeComponents.
	BlobShadow = CreateDefaultSubobject<URRBlobShadowComponent>(TEXT("BlobShadow"));
	BlobShadow->SetupAttachment(GetCapsuleComponent());

	// task-272-07: feeds MPC_PlayerLight (post-process light mask around the miner).
	LightMask = CreateDefaultSubobject<URRPlayerLightMaskComponent>(TEXT("LightMask"));

	// task-328-03: ground chevrons along the aim (absolute transform, placed by the component each tick).
	AimIndicator = CreateDefaultSubobject<URRAimIndicatorComponent>(TEXT("AimIndicator"));
	AimIndicator->SetupAttachment(GetCapsuleComponent());
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
	SpringArm->bEnableCameraLag = bCameraLag;
	Camera->SetFieldOfView(CameraFieldOfView);
}

void ARRMiner::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	// task-263-01: only the grid rocks cast shadows (from the player light); no mesh of this actor or its Blueprint does.
	RRShadows::DisableCastShadows(this);

	// Unscaled sizes: the disc is a child of the capsule, so the actor scale applies to both alike.
	const UCapsuleComponent* Capsule = GetCapsuleComponent();
	BlobShadow->SetFootprintDiameter(Capsule->GetUnscaledCapsuleRadius() * BlobShadowDiameterScale);
	BlobShadow->PlaceUnderCapsule(Capsule->GetUnscaledCapsuleHalfHeight());
}

void ARRMiner::BeginPlay()
{
	Super::BeginPlay();
	// Ground telegraphs (sys-ground-telegraph) never tint the miner.
	URRGroundTelegraphComponent::DisableDecalsOn(this);

	AimDirection = GetActorForwardVector().GetSafeNormal2D(UE_SMALL_NUMBER, FVector::ForwardVector);

	// Init order (sys-gas-core): actor info, then base stats once the three sets are on the ASC.
	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	// HUD health events (sys-hud): bound before the base stats so their application sends the first OnHealthChanged.
	HealthChangedHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(URRHealthAttributeSet::GetHealthAttribute())
		.AddUObject(this, &ARRMiner::HandleHealthAttributeChanged);
	MaxHealthChangedHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(URRHealthAttributeSet::GetMaxHealthAttribute())
		.AddUObject(this, &ARRMiner::HandleHealthAttributeChanged);
	LastWeaponSlots.Reset();
	LastWeaponSlots.SetNum(FMath::Max(MaxWeapons, 0));

	MoveSpeedChangedHandle = AbilitySystemComponent
		->GetGameplayAttributeValueChangeDelegate(URRMovementAttributeSet::GetMoveSpeedAttribute())
		.AddUObject(this, &ARRMiner::HandleMoveSpeedChanged);

	if (!FRRBaseStatsLoader::LoadAndApplyPlayerBaseStats(*AbilitySystemComponent, BaseStatsRowName))
	{
		UE_LOG(LogRogueRigs, Error, TEXT("%s: base stats row '%s' could not be applied."), *GetName(), *BaseStatsRowName.ToString());
	}

	// Push the value even if no change was broadcast (e.g. same value as before).
	ApplyMoveSpeed(MovementSet->GetMoveSpeed());

	// Damage reactions (sys-damage): i-frames from the hit callback (same call stack as the damage), single death.
	DamageReceivedHandle = HealthSet->OnDamageReceived.AddUObject(this, &ARRMiner::HandleDamageReceived);
	DeathHandle = HealthSet->OnDeath.AddUObject(this, &ARRMiner::HandleDeath);

	// Game over: the run game mode listens to OnDeath (no game mode in the test worlds).
	if (ARRGameMode* GameMode = GetWorld()->GetAuthGameMode<ARRGameMode>())
	{
		GameMode->RegisterMiner(this);
	}

	SpawnDrone();
	FindArenaGrid();
	SpawnStartingWeapons();

	// Registry for late HUD binding (sys-hud): after the base stats, so a listener reads real health values.
	if (URRRunFlowSubsystem* RunFlow = GetWorld()->GetSubsystem<URRRunFlowSubsystem>())
	{
		RunFlow->RegisterMiner(this);
	}

	// Run carry-over (sys-run): begins level 1 on a directly played map, or re-applies the previous level snapshot.
	if (URRRunSubsystem* Run = URRRunSubsystem::Get(this))
	{
		Run->HandleMinerBeginPlay(this);
	}

	// Run log (sys-run-log): per-level counters reset and bound after the snapshot replay, so replayed cards are not counted.
	if (URRRunLogSubsystem* RunLog = URRRunLogSubsystem::Get(this))
	{
		RunLog->HandleMinerBeginPlay(this);
	}
}

void ARRMiner::FindArenaGrid()
{
	ArenaGrid = RRWorld::FindArenaGrid(GetWorld());
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
	// StartingWeapons in order, then each guaranteed class that no StartingWeapons entry already provides (itself or a subclass).
	TArray<TSubclassOf<ARRWeapon>> Loadout = StartingWeapons;
	for (const TSubclassOf<ARRWeapon>& Guaranteed : GuaranteedWeaponClasses)
	{
		const bool bProvided = !Guaranteed || StartingWeapons.ContainsByPredicate([&Guaranteed](const TSubclassOf<ARRWeapon>& Entry)
		{
			return Entry && Entry->IsChildOf(Guaranteed);
		});
		if (!bProvided)
		{
			Loadout.Add(Guaranteed);
		}
	}

	for (const TSubclassOf<ARRWeapon>& WeaponClass : Loadout)
	{
		if (!WeaponClass)
		{
			UE_LOG(LogRogueRigs, Warning, TEXT("%s: empty entry in StartingWeapons skipped."), *GetName());
			continue;
		}
		AddWeapon(WeaponClass);
	}
}

bool ARRMiner::OwnsWeaponClass(TSubclassOf<ARRWeapon> WeaponClass) const
{
	return WeaponClass && Weapons.ContainsByPredicate([&WeaponClass](const TObjectPtr<ARRWeapon>& Weapon)
	{
		return IsValid(Weapon) && Weapon->IsA(WeaponClass);
	});
}

int32 ARRMiner::GetOwnedWeaponCount() const
{
	int32 Count = 0;
	for (const TObjectPtr<ARRWeapon>& Weapon : Weapons)
	{
		Count += IsValid(Weapon) ? 1 : 0;
	}
	return Count;
}

bool ARRMiner::CanAcquireWeapon(TSubclassOf<ARRWeapon> WeaponClass) const
{
	return WeaponClass && GetOwnedWeaponCount() < MaxWeapons && !OwnsWeaponClass(WeaponClass);
}

TArray<FRRWeaponCatalogueEntry> ARRMiner::GetAcquirableWeapons() const
{
	TArray<FRRWeaponCatalogueEntry> Result;
	for (const FRRWeaponCatalogueEntry& Entry : WeaponCatalogue)
	{
		const bool bDuplicate = Result.ContainsByPredicate([&Entry](const FRRWeaponCatalogueEntry& Other) { return Other.WeaponClass == Entry.WeaponClass; });
		if (!bDuplicate && CanAcquireWeapon(Entry.WeaponClass))
		{
			Result.Add(Entry);
		}
	}
	return Result;
}

ARRWeapon* ARRMiner::AddWeapon(TSubclassOf<ARRWeapon> WeaponClass)
{
	UWorld* World = GetWorld();
	if (!WeaponClass || !World)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("%s: AddWeapon without a weapon class or a world; ignored."), *GetName());
		return nullptr;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	SpawnParameters.Instigator = this;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// Acquisition order (sys-weapons): spawn and attach, then Acquire (actor info, CSV base values, auto-fire).
	ARRWeapon* Weapon = World->SpawnActor<ARRWeapon>(WeaponClass, GetActorTransform(), SpawnParameters);
	if (!Weapon)
	{
		UE_LOG(LogRogueRigs, Error, TEXT("%s: weapon of class %s could not be spawned."), *GetName(), *WeaponClass->GetName());
		return nullptr;
	}

	// No socket: the capsule, which turns with the aim yaw, so the weapon points forward.
	if (WeaponAttachSocketName.IsNone() || !GetMesh())
	{
		Weapon->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		Weapon->SetActorRelativeLocation(WeaponAttachOffset);
	}
	else
	{
		Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
	}
	// The weapon is carried inside the capsule: never let it block the miner's own moves.
	GetCapsuleComponent()->IgnoreActorWhenMoving(Weapon, true);
	// Carried weapons are part of the miner look: ground telegraphs do not tint them either.
	URRGroundTelegraphComponent::DisableDecalsOn(Weapon);
	Weapon->Acquire(this);
	Weapons.Add(Weapon);
	BindWeaponSlotEvents(Weapon);
	RefreshWeaponSlots();
	OnWeaponAdded.Broadcast(Weapon);
	return Weapon;
}

ARRTurretLauncher* ARRMiner::FindTurretLauncher() const
{
	for (ARRWeapon* Weapon : Weapons)
	{
		if (ARRTurretLauncher* Launcher = Cast<ARRTurretLauncher>(Weapon))
		{
			return Launcher;
		}
	}
	return nullptr;
}

ARRTurret* ARRMiner::PlaceTurret()
{
	if (IsDead())
	{
		return nullptr;
	}
	ARRTurretLauncher* Launcher = FindTurretLauncher();
	return Launcher ? Launcher->TryPlaceTurret() : nullptr;
}

bool ARRMiner::GetMouseAimPoint(FVector& OutPoint) const
{
	if (!bUsingMouseAim || !bHasMouseAimPoint)
	{
		return false;
	}
	OutPoint = MouseAimPoint;
	return true;
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

	if (HealthSet)
	{
		HealthSet->OnDamageReceived.Remove(DamageReceivedHandle);
		HealthSet->OnDeath.Remove(DeathHandle);
	}
	DamageReceivedHandle.Reset();
	DeathHandle.Reset();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(URRHealthAttributeSet::GetHealthAttribute()).Remove(HealthChangedHandle);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(URRHealthAttributeSet::GetMaxHealthAttribute()).Remove(MaxHealthChangedHandle);
	}
	HealthChangedHandle.Reset();
	MaxHealthChangedHandle.Reset();

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

void ARRMiner::NotifyPickupCollected(ERRPickupType Type, const FVector& Location)
{
	OnPickupCollected.Broadcast(Type);
	OnPickupCollectedAt.Broadcast(Type, Location);
}

bool ARRMiner::IsDead() const
{
	return AbilitySystemComponent && AbilitySystemComponent->HasMatchingGameplayTag(RRGameplayTags::State_Dead);
}

void ARRMiner::HandleDamageReceived(float Damage, bool bIsCritical, float HealthLeft)
{
	// A lethal hit gets no i-frames: the death handler takes over.
	if (HealthLeft > 0.f && !IsDead())
	{
		ApplyIFrames();
	}
}

void ARRMiner::ApplyIFrames()
{
	if (IFramesDuration <= 0.f || !IFramesEffectClass)
	{
		return;
	}

	// Applying a GE from the post-execute call stack is safe (gas-api-notes); the next hit of the frame is refused.
	FGameplayEffectSpecHandle Spec = AbilitySystemComponent->MakeOutgoingSpec(IFramesEffectClass, 1.f, AbilitySystemComponent->MakeEffectContext());
	if (!Spec.IsValid())
	{
		UE_LOG(LogRogueRigs, Error, TEXT("%s: i-frames spec of class %s could not be made."), *GetName(), *IFramesEffectClass->GetName());
		return;
	}
	Spec.Data->SetSetByCallerMagnitude(RRGameplayTags::Data_Duration, IFramesDuration);
	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec.Data);

	if (BlinkRate > 0.f)
	{
		const double Now = GetWorld()->GetTimeSeconds();
		BlinkStartTime = Now;
		BlinkEndTime = Now + IFramesDuration;
		bBlinking = true;
		SetMeshVisible(false);
	}
}

void ARRMiner::UpdateBlink()
{
	if (!bBlinking)
	{
		return;
	}

	const double Now = GetWorld()->GetTimeSeconds();
	if (Now >= BlinkEndTime)
	{
		bBlinking = false;
		SetMeshVisible(true);
		return;
	}

	// Hidden during the first half of each cycle, so the blink starts on the hit frame.
	const double Phase = FMath::Frac((Now - BlinkStartTime) * BlinkRate);
	SetMeshVisible(Phase >= 0.5);
}

void ARRMiner::SetMeshVisible(bool bVisible)
{
	if (USkeletalMeshComponent* MeshComponent = GetMesh())
	{
		MeshComponent->SetVisibility(bVisible, /*bPropagateToChildren*/ true);
	}
}

void ARRMiner::HandleDeath(AActor* DeathInstigator)
{
	URRGroundTelegraphComponent::HideAllOn(this);
	// Aim chevrons are hidden at death (aim-indicator); the component's tick keeps them hidden while IsDead.
	if (AimIndicator)
	{
		AimIndicator->HideIndicator();
	}
	// Controls cut (miner gdd): no more input-driven movement or aim, weapons stop firing.
	PendingMoveInput = FVector::ZeroVector;
	ConsumeMovementInputVector();
	GetCharacterMovement()->StopMovementImmediately();

	for (ARRWeapon* Weapon : Weapons)
	{
		if (IsValid(Weapon) && Weapon->GetAutoFireComponent())
		{
			Weapon->GetAutoFireComponent()->StopFiring();
		}
	}

	bBlinking = false;
	SetMeshVisible(true);

	UE_LOG(LogRogueRigs, Log, TEXT("%s died (instigator: %s)."), *GetName(), *GetNameSafe(DeathInstigator));
	OnDeath.Broadcast(DeathInstigator);
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
	if (IsDead())
	{
		return;
	}

	const FVector WorldInput = ScreenInputToWorld(Input).GetClampedToMaxSize(1.f);
	PendingMoveInput = WorldInput;
	AddMovementInput(WorldInput);
}

void ARRMiner::ApplyStickAimInput(const FVector2D& Input)
{
	if (Input.Size() <= StickAimMinInput || IsDead())
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
	MouseAimPoint = AimPoint;
	bHasMouseAimPoint = true;
	const FVector ToAim = AimPoint - GetActorLocation();
	AimDirection = ToAim.GetSafeNormal2D(UE_SMALL_NUMBER, AimDirection);
}

void ARRMiner::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateBlink();

	// Dead: no aim, no turning, no dig (controls are cut).
	if (IsDead())
	{
		PendingMoveInput = FVector::ZeroVector;
		bTouchingRock = false;
		if (IsValid(Drone))
		{
			Drone->ReleaseDig();
		}
		return;
	}

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
	UpdateDigTrigger(MoveInput);
}

void ARRMiner::UpdateDigTrigger(const FVector& MoveInput)
{
	bTouchingRock = false;
	if (!IsValid(ArenaGrid) || !IsValid(Drone))
	{
		return;
	}

	// Contact: block cells whose square comes within the capsule radius + margin of the miner (2D, unrotated grid).
	const FVector Location = GetActorLocation();
	const double CellSize = ArenaGrid->GetCellSize();
	const double HalfCell = 0.5 * CellSize;
	const double ContactDistance = GetCapsuleComponent()->GetScaledCapsuleRadius() + DigContactMargin;
	const int32 Span = FMath::CeilToInt32(ContactDistance / FMath::Max(CellSize, 1.0));
	const FIntPoint MinerCell = ArenaGrid->GetCellAtWorld(Location);
	FVector NormalSum = FVector::ZeroVector;
	TArray<FIntPoint> TouchingCells;
	for (int32 Y = MinerCell.Y - Span; Y <= MinerCell.Y + Span; ++Y)
	{
		for (int32 X = MinerCell.X - Span; X <= MinerCell.X + Span; ++X)
		{
			if (!ArenaGrid->IsBlockCell(FIntPoint(X, Y)))
			{
				continue;
			}
			const FVector Center = ArenaGrid->GetCellCenter(X, Y);
			const FVector Nearest(FMath::Clamp(Location.X, Center.X - HalfCell, Center.X + HalfCell),
				FMath::Clamp(Location.Y, Center.Y - HalfCell, Center.Y + HalfCell), Location.Z);
			const FVector Offset = Location - Nearest;
			if (Offset.SizeSquared2D() > FMath::Square(ContactDistance))
			{
				continue;
			}
			bTouchingRock = true;
			TouchingCells.Add(FIntPoint(X, Y));
			// Inside the square (overlap): push the normal away from the block center instead.
			const FVector Normal = Offset.SizeSquared2D() > UE_KINDA_SMALL_NUMBER ? Offset.GetSafeNormal2D() : (Location - Center).GetSafeNormal2D();
			NormalSum += Normal;
		}
	}

	if (!bTouchingRock)
	{
		Drone->ReleaseDig();
		return;
	}

	// Toward the wall; a symmetric contact (normals cancel out) falls back to the facing direction.
	FVector WallDirection = (-NormalSum).GetSafeNormal2D();
	if (WallDirection.IsZero())
	{
		WallDirection = GetActorForwardVector().GetSafeNormal2D();
	}

	// The move input steers the arc while it pushes into the wall.
	FVector ArcDirection = WallDirection;
	const FVector MoveDirection = MoveInput.GetSafeNormal2D();
	if (MoveInput.Size2D() > DigMinMoveInput && FVector::DotProduct(MoveDirection, WallDirection) > 0.0)
	{
		ArcDirection = MoveDirection;
	}

	// The touching cells let the drone dig the nearest one when the steered arc misses them all (task-065-01).
	Drone->RequestArcDig(ArenaGrid, ArcDirection, TouchingCells);
}

float ARRMiner::GetCurrentHealth() const
{
	return AbilitySystemComponent ? AbilitySystemComponent->GetNumericAttribute(URRHealthAttributeSet::GetHealthAttribute()) : 0.f;
}

float ARRMiner::GetMaxHealth() const
{
	return AbilitySystemComponent ? AbilitySystemComponent->GetNumericAttribute(URRHealthAttributeSet::GetMaxHealthAttribute()) : 0.f;
}

void ARRMiner::HandleHealthAttributeChanged(const FOnAttributeChangeData& ChangeData)
{
	const float Current = GetCurrentHealth();
	const float Max = GetMaxHealth();
	if (Max > 0.f && (Current != LastBroadcastHealth || Max != LastBroadcastMaxHealth))
	{
		LastBroadcastHealth = Current;
		LastBroadcastMaxHealth = Max;
		OnHealthChanged.Broadcast(Current, Max);
	}

	// Damage: Health went down. A MaxHealth decrease clamps Health to the new maximum (OldValue above it): not a hit.
	if (ChangeData.Attribute == URRHealthAttributeSet::GetHealthAttribute() && ChangeData.NewValue < ChangeData.OldValue
		&& Max > 0.f && ChangeData.OldValue <= Max)
	{
		OnDamaged.Broadcast(ChangeData.OldValue - ChangeData.NewValue, FMath::Clamp(ChangeData.NewValue / Max, 0.f, 1.f));
	}
}

FRRWeaponSlotHUD ARRMiner::GetWeaponSlotHUD(int32 SlotIndex) const
{
	if (SlotIndex < 0 || SlotIndex >= MaxWeapons)
	{
		return FRRWeaponSlotHUD();
	}
	// Slots follow acquisition order over the valid weapons, so a destroyed entry never leaves a hole.
	int32 ValidIndex = 0;
	for (const TObjectPtr<ARRWeapon>& Weapon : Weapons)
	{
		if (IsValid(Weapon))
		{
			if (ValidIndex == SlotIndex)
			{
				return FRRWeaponSlotHUD::Make(Weapon);
			}
			++ValidIndex;
		}
	}
	return FRRWeaponSlotHUD();
}

void ARRMiner::BindWeaponSlotEvents(ARRWeapon* Weapon)
{
	if (!IsValid(Weapon))
	{
		return;
	}
	Weapon->OnAmmoChanged.AddUniqueDynamic(this, &ARRMiner::HandleWeaponAmmoChanged);
	if (URRAutoFireComponent* WeaponAutoFire = Weapon->GetAutoFireComponent())
	{
		WeaponAutoFire->OnReloadStarted.AddUniqueDynamic(this, &ARRMiner::HandleWeaponReloadStarted);
		WeaponAutoFire->OnReloadFinished.AddUniqueDynamic(this, &ARRMiner::HandleWeaponReloadFinished);
	}
	if (ARRTurretLauncher* Launcher = Cast<ARRTurretLauncher>(Weapon))
	{
		Launcher->OnTurretChargesChanged.AddUniqueDynamic(this, &ARRMiner::HandleTurretChargesChanged);
	}
}

void ARRMiner::RefreshWeaponSlots()
{
	if (LastWeaponSlots.Num() != FMath::Max(MaxWeapons, 0))
	{
		LastWeaponSlots.SetNum(FMath::Max(MaxWeapons, 0));
	}
	for (int32 SlotIndex = 0; SlotIndex < LastWeaponSlots.Num(); ++SlotIndex)
	{
		const FRRWeaponSlotHUD Slot = GetWeaponSlotHUD(SlotIndex);
		if (!Slot.HasSameState(LastWeaponSlots[SlotIndex]))
		{
			LastWeaponSlots[SlotIndex] = Slot;
			OnWeaponSlotChanged.Broadcast(SlotIndex, Slot);
		}
	}
}

void ARRMiner::HandleWeaponAmmoChanged(ARRWeapon* Weapon, int32 Rounds, int32 MagazineSize)
{
	RefreshWeaponSlots();
}

void ARRMiner::HandleWeaponReloadStarted(float ReloadDuration)
{
	RefreshWeaponSlots();
}

void ARRMiner::HandleWeaponReloadFinished()
{
	RefreshWeaponSlots();
}

void ARRMiner::HandleTurretChargesChanged(int32 Charges, int32 MaxCharges, float RechargeDuration)
{
	RefreshWeaponSlots();
}
