// Rogue Rigs - aim indicator (gdd aim-indicator): a thin lime-green laser from the shown rifle Muzzle along the miner's aim, stopped at the first robot or block.

#include "AimIndicator/RRAimIndicatorComponent.h"

#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/CollisionProfile.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Enemies/RREnemy.h"
#include "Grid/RRArenaGrid.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Misc/PackageName.h"
#include "Player/RRMiner.h"
#include "RogueRigs.h"
#include "Settings/RRGameUserSettings.h"
#include "Weapons/RRWeapon.h"

const FName URRAimIndicatorComponent::ColorParameterName(TEXT("Color"));
const FName URRAimIndicatorComponent::EmissiveGainParameterName(TEXT("EmissiveGain"));
const FName URRAimIndicatorComponent::OpacityParameterName(TEXT("Opacity"));
const FName URRAimIndicatorComponent::CoreFracParameterName(TEXT("CoreFrac"));
const FName URRAimIndicatorComponent::GlowOpacityParameterName(TEXT("GlowOpacity"));
const FName URRAimIndicatorComponent::TipFadeFracParameterName(TEXT("TipFadeFrac"));

namespace RRAimIndicator
{
	/** Default mesh and material resolved once per run (the path each was resolved for, the result, whether the warning was logged). */
	FSoftObjectPath MeshPath;
	TWeakObjectPtr<UObject> Mesh;
	bool bMeshWarned = false;
	FSoftObjectPath MaterialPath;
	TWeakObjectPtr<UObject> Material;
	bool bMaterialWarned = false;
}

URRAimIndicatorComponent::URRAimIndicatorComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	// After the miner's tick (aim and yaw, TG_PrePhysics), its movement and the hand pose: placed on this frame's Muzzle and aim.
	PrimaryComponentTick.TickGroup = TG_PostPhysics;
	bCanEverAffectNavigation = false;
	LaserLength = TraceReach;
}

UObject* URRAimIndicatorComponent::ResolveAsset(const FSoftObjectPath& Path, const TCHAR* What, FSoftObjectPath& CachedPath, TWeakObjectPtr<UObject>& Cache, bool& bWarned)
{
	// Resolved again when the path changed or the cached asset was collected (editor, between two plays).
	if (CachedPath != Path || !Cache.IsValid())
	{
		CachedPath = Path;
		Cache = nullptr;
		// Check the package first: loading a missing asset makes the engine log its own warning on every try.
		const FString PackageName = Path.GetLongPackageName();
		if (!PackageName.IsEmpty() && FPackageName::DoesPackageExist(PackageName))
		{
			Cache = Path.TryLoad();
		}
		if (!Cache.IsValid() && !bWarned)
		{
			bWarned = true;
			UE_LOG(LogRogueRigs, Warning, TEXT("Aim indicator: %s %s not found; the aim laser is not drawn (task-415-02)."), What, *Path.ToString());
		}
	}
	return Cache.Get();
}

void URRAimIndicatorComponent::BeginPlay()
{
	Super::BeginPlay();

	LaserLength = TraceReach;

	UStaticMesh* PlaneMesh = nullptr;
	UMaterialInterface* LaserMaterial = nullptr;
	{
		using namespace RRAimIndicator;
		PlaneMesh = Cast<UStaticMesh>(ResolveAsset(DefaultMeshPath, TEXT("mesh"), MeshPath, Mesh, bMeshWarned));
		LaserMaterial = Cast<UMaterialInterface>(ResolveAsset(DefaultMaterialPath, TEXT("material"), MaterialPath, Material, bMaterialWarned));
	}

	AActor* Owner = GetOwner();
	if (!PlaneMesh || !LaserMaterial || !Owner)
	{
		// Missing asset: nothing to draw (warned once in ResolveAsset).
		IndicatorMaterialInstance = nullptr;
		SetComponentTickEnabled(false);
		return;
	}

	LaserMesh = NewObject<UStaticMeshComponent>(Owner, MakeUniqueObjectName(Owner, UStaticMeshComponent::StaticClass(), TEXT("AimLaserMesh")), RF_Transient);
	// Pure visual.
	LaserMesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	LaserMesh->SetGenerateOverlapEvents(false);
	LaserMesh->SetCanEverAffectNavigation(false);
	LaserMesh->CanCharacterStepUpOn = ECB_No;
	LaserMesh->CastShadow = false;
	LaserMesh->bCastDynamicShadow = false;
	LaserMesh->bReceivesDecals = false;
	LaserMesh->PrimaryComponentTick.bCanEverTick = false;
	// Hidden until the first refresh.
	LaserMesh->SetVisibleFlag(false);
	// Placed in world space every tick: the owner's rotation, scale and capsule height never leak into the plane.
	LaserMesh->SetUsingAbsoluteLocation(true);
	LaserMesh->SetUsingAbsoluteRotation(true);
	LaserMesh->SetUsingAbsoluteScale(true);
	LaserMesh->SetStaticMesh(PlaneMesh);
	LaserMesh->SetupAttachment(this);
	LaserMesh->RegisterComponent();

	IndicatorMaterialInstance = UMaterialInstanceDynamic::Create(LaserMaterial, this);
	LaserMesh->SetMaterial(0, IndicatorMaterialInstance);
	ApplyParameters();

	RefreshIndicator();
}

void URRAimIndicatorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (LaserMesh)
	{
		LaserMesh->DestroyComponent();
		LaserMesh = nullptr;
	}
	Super::EndPlay(EndPlayReason);
}

bool URRAimIndicatorComponent::IsIndicatorShown() const
{
	return LaserMesh && LaserMesh->IsVisible();
}

void URRAimIndicatorComponent::ApplyParameters()
{
	if (!IndicatorMaterialInstance)
	{
		return;
	}
	IndicatorMaterialInstance->SetVectorParameterValue(ColorParameterName, Color);
	IndicatorMaterialInstance->SetScalarParameterValue(EmissiveGainParameterName, EmissiveGain);
	IndicatorMaterialInstance->SetScalarParameterValue(OpacityParameterName, Opacity);
	IndicatorMaterialInstance->SetScalarParameterValue(CoreFracParameterName, CoreFrac);
	IndicatorMaterialInstance->SetScalarParameterValue(GlowOpacityParameterName, GlowOpacity);
	// No fade at the tip (task-468 Q2): the line ends sharp on the wall, robot or barrel that stops it. M_AimLaser divides by
	// max(TipFadeFrac, 0.0001), so 0 keeps the whole length opaque.
	IndicatorMaterialInstance->SetScalarParameterValue(TipFadeFracParameterName, 0.f);
}

ARRMiner* URRAimIndicatorComponent::GetOwnerMiner() const
{
	return Cast<ARRMiner>(GetOwner());
}

void URRAimIndicatorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	RefreshIndicator();
}

bool URRAimIndicatorComponent::FindLaserOrigin(const ARRMiner& Miner, FVector& OutOrigin, float& OutVerticalHalfHeight) const
{
	OutVerticalHalfHeight = 0.f;
	const ARRWeapon* FirstWeapon = nullptr;
	for (const ARRWeapon* Weapon : Miner.GetWeapons())
	{
		if (IsValid(Weapon))
		{
			FirstWeapon = Weapon;
			break;
		}
	}
	if (!FirstWeapon)
	{
		return false;
	}
	if (const ARRWeapon* HandWeapon = FirstWeapon->FindHandWeapon(); HandWeapon && HandWeapon->GetMuzzle())
	{
		// The shown rifle Muzzle, where the shots leave (task-412); the shot capsule height gives the robot sweep its vertical reach.
		OutOrigin = HandWeapon->GetMuzzle()->GetComponentLocation();
		OutVerticalHalfHeight = HandWeapon->GetShotPlacement().CapsuleHalfHeight;
		return true;
	}
	OutOrigin = FirstWeapon->GetShotOrigin();
	return true;
}

float URRAimIndicatorComponent::FindRobotDistance(const ARRMiner& Miner, const FVector& Origin, const FVector& Direction, float VerticalHalfHeight, float MaxDistance) const
{
	const UWorld* World = GetWorld();
	if (!World || MaxDistance <= 0.f)
	{
		return MaxDistance;
	}

	const float Radius = FMath::Max(RobotSweepRadius, 0.f);
	const FCollisionShape Shape = FCollisionShape::MakeCapsule(Radius, FMath::Max(VerticalHalfHeight, Radius));
	// Robots are found by their hit capsule (object type Pawn, the shape the shots hit); other pawns are filtered below.
	const FCollisionObjectQueryParams ObjectParams(ECC_Pawn);
	FCollisionQueryParams Params(SCENE_QUERY_STAT(RRAimLaserRobots), /*bTraceComplex*/ false, &Miner);

	TArray<FHitResult> Hits;
	World->SweepMultiByObjectType(Hits, Origin, Origin + Direction * MaxDistance, FQuat::Identity, ObjectParams, Shape, Params);

	float Nearest = MaxDistance;
	for (const FHitResult& Hit : Hits)
	{
		const ARREnemy* Robot = Cast<ARREnemy>(Hit.GetActor());
		if (!IsValid(Robot) || Robot->IsDead() || Robot->IsDeathPending() || Hit.GetComponent() != Robot->GetHitCapsule())
		{
			continue;
		}
		// Initial overlap (robot on the Muzzle): distance 0.
		Nearest = FMath::Min(Nearest, Hit.bStartPenetrating ? 0.f : Hit.Distance);
	}
	return Nearest;
}

void URRAimIndicatorComponent::RefreshIndicator()
{
	const ARRMiner* Miner = GetOwnerMiner();
	// Aim indicator option of the Gameplay tab (options-menu): Off hides the laser, read each refresh so it follows at once.
	if (!IndicatorMaterialInstance || !LaserMesh || !IsValid(Miner) || Miner->IsActorBeingDestroyed() || Miner->IsDead()
		|| !URRGameUserSettings::IsGameplayOptionOn(ERRGameplayOption::AimIndicator))
	{
		HideIndicator();
		return;
	}

	FVector Origin;
	float VerticalHalfHeight = 0.f;
	if (!FindLaserOrigin(*Miner, Origin, VerticalHalfHeight))
	{
		HideIndicator();
		return;
	}
	LaserOrigin = Origin;

	const FVector Forward = Miner->GetAimDirection().GetSafeNormal2D(UE_SMALL_NUMBER, FVector::ForwardVector);

	// No length cap (task-468): the nearer of the first block cell (rock, ore, barrel; the arena border is rock) and the first
	// living robot on the aim line, searched over TraceReach (past the screen edge when nothing is in the way).
	float Length = TraceReach;
	if (const ARRArenaGrid* Grid = Miner->GetArenaGrid(); IsValid(Grid))
	{
		Length = FMath::Min(Length, Grid->GetClearDistance(Origin, Forward, Length));
	}
	Length = FMath::Min(Length, FindRobotDistance(*Miner, Origin, Forward, VerticalHalfHeight, Length));
	LaserLength = FMath::Max(Length, 0.f);

	if (LaserLength <= KINDA_SMALL_NUMBER || LaserMeshSize <= 0.f)
	{
		HideIndicator();
		return;
	}

	// Engine Plane: LaserMeshSize square centred on its pivot in local XY, normal +Z, UV.x = 0 at local -X and 1 at +X (checked
	// on the mesh vertices, task-415-02). Local +X along the aim, flat (horizontal) at the Muzzle height: UV.x = 0 at the Muzzle.
	const FQuat Rotation = FRotationMatrix::MakeFromXZ(Forward, FVector::UpVector).ToQuat();
	LaserMesh->SetWorldLocationAndRotation(Origin + Forward * (LaserLength * 0.5f), Rotation);
	LaserMesh->SetWorldScale3D(FVector(LaserLength / LaserMeshSize, Width / LaserMeshSize, 1.f));
	LaserMesh->SetVisibility(true);
}

void URRAimIndicatorComponent::HideIndicator()
{
	if (LaserMesh)
	{
		LaserMesh->SetVisibility(false);
	}
}
