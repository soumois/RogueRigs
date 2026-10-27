// Rogue Rigs - aim indicator (gdd aim-indicator): a decal projected on the ground in front of the miner drawing a row of white-gold chevrons along the aim.

#include "AimIndicator/RRAimIndicatorComponent.h"

#include "Components/CapsuleComponent.h"
#include "Grid/RRArenaGrid.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Misc/PackageName.h"
#include "Player/RRMiner.h"
#include "RogueRigs.h"
#include "Telegraph/RRDecalWarmUp.h"

const FName URRAimIndicatorComponent::ChevronCountParameterName(TEXT("ChevronCount"));
const FName URRAimIndicatorComponent::FirstOffsetParameterName(TEXT("FirstOffset"));
const FName URRAimIndicatorComponent::SpacingParameterName(TEXT("Spacing"));
const FName URRAimIndicatorComponent::OpeningDegParameterName(TEXT("OpeningDeg"));
const FName URRAimIndicatorComponent::ArmLengthParameterName(TEXT("ArmLength"));
const FName URRAimIndicatorComponent::LineWidthParameterName(TEXT("LineWidth"));
const FName URRAimIndicatorComponent::DecayParameterName(TEXT("Decay"));
const FName URRAimIndicatorComponent::ColorParameterName(TEXT("Color"));
const FName URRAimIndicatorComponent::OpacityParameterName(TEXT("Opacity"));
const FName URRAimIndicatorComponent::HalfLengthParameterName(TEXT("HalfLength"));
const FName URRAimIndicatorComponent::HalfWidthParameterName(TEXT("HalfWidth"));
const FName URRAimIndicatorComponent::VisibleLengthParameterName(TEXT("VisibleLength"));

namespace RRAimIndicator
{
	/** Default material resolved once per run (the path it was resolved for, the result, whether the warning was logged). */
	FSoftObjectPath ResolvedPath;
	TWeakObjectPtr<UMaterialInterface> ResolvedMaterial;
	bool bResolved = false;
	bool bWarned = false;
}

URRAimIndicatorComponent::URRAimIndicatorComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	// After the miner's tick (aim and yaw, TG_PrePhysics) and its movement: placed on this frame's aim and location.
	PrimaryComponentTick.TickGroup = TG_PostPhysics;
	bCanEverAffectNavigation = false;
	// Hidden until the first refresh in play (no stray box in the Blueprint preview or before the material exists).
	SetVisibleFlag(false);
	// Placed in world space every tick: the owner's rotation and the capsule height never leak into the box.
	SetUsingAbsoluteLocation(true);
	SetUsingAbsoluteRotation(true);
	// Projects along local X: straight down by default (yaw set each tick).
	SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));
	// Top-down camera: never fade out with screen size.
	FadeScreenSize = 0.f;
	DecalSize = FVector(ProjectionHalfDepth, HalfWidth, HalfLength);
	VisibleLength = GetFixedLength();
}

float URRAimIndicatorComponent::GetFixedLength() const
{
	return FirstOffset + FMath::Max(ChevronCount - 1, 0) * Spacing;
}

void URRAimIndicatorComponent::BeginPlay()
{
	Super::BeginPlay();

	DecalSize = FVector(ProjectionHalfDepth, HalfWidth, HalfLength);
	VisibleLength = GetFixedLength();

	UMaterialInterface* Material = DecalMaterial;
	if (Material && Material->IsA<UMaterialInstanceDynamic>())
	{
		Material = Cast<UMaterialInstanceDynamic>(Material)->Parent;
	}
	if (!Material)
	{
		Material = ResolveDefaultMaterial();
	}

	if (!Material)
	{
		// Missing asset: nothing to draw (warned once in ResolveDefaultMaterial).
		IndicatorMaterialInstance = nullptr;
		SetDecalMaterial(nullptr);
		HideIndicator();
		SetComponentTickEnabled(false);
		return;
	}

	IndicatorMaterialInstance = UMaterialInstanceDynamic::Create(Material, this);
	SetDecalMaterial(IndicatorMaterialInstance);
	ApplyParameters();
	// Draw the material once out of sight (once per world): its first shown frame is not the engine default decal (task-201-01).
	ARRDecalWarmUp::WarmUp(GetWorld(), Material);

	RefreshIndicator();
}

UMaterialInterface* URRAimIndicatorComponent::ResolveDefaultMaterial() const
{
	using namespace RRAimIndicator;
	if (!bResolved || ResolvedPath != DefaultMaterialPath)
	{
		bResolved = true;
		ResolvedPath = DefaultMaterialPath;
		ResolvedMaterial = nullptr;
		// Check the package first: loading a missing asset makes the engine log its own warning on every try.
		const FString PackageName = DefaultMaterialPath.GetLongPackageName();
		if (!PackageName.IsEmpty() && FPackageName::DoesPackageExist(PackageName))
		{
			ResolvedMaterial = Cast<UMaterialInterface>(DefaultMaterialPath.TryLoad());
		}
		if (!ResolvedMaterial.IsValid() && !bWarned)
		{
			bWarned = true;
			UE_LOG(LogRogueRigs, Warning, TEXT("Aim indicator: material %s not found; the aim chevrons are not drawn (task-328-02)."),
				*DefaultMaterialPath.ToString());
		}
	}
	return ResolvedMaterial.Get();
}

void URRAimIndicatorComponent::ApplyParameters()
{
	if (!IndicatorMaterialInstance)
	{
		return;
	}
	IndicatorMaterialInstance->SetScalarParameterValue(ChevronCountParameterName, static_cast<float>(ChevronCount));
	IndicatorMaterialInstance->SetScalarParameterValue(FirstOffsetParameterName, FirstOffset);
	IndicatorMaterialInstance->SetScalarParameterValue(SpacingParameterName, Spacing);
	IndicatorMaterialInstance->SetScalarParameterValue(OpeningDegParameterName, OpeningDeg);
	IndicatorMaterialInstance->SetScalarParameterValue(ArmLengthParameterName, ArmLength);
	IndicatorMaterialInstance->SetScalarParameterValue(LineWidthParameterName, LineWidth);
	IndicatorMaterialInstance->SetScalarParameterValue(DecayParameterName, Decay);
	IndicatorMaterialInstance->SetVectorParameterValue(ColorParameterName, Color);
	IndicatorMaterialInstance->SetScalarParameterValue(OpacityParameterName, Opacity);
	// The material maps UV to uu with the box half sizes: keep them equal to DecalSize.
	IndicatorMaterialInstance->SetScalarParameterValue(HalfLengthParameterName, HalfLength);
	IndicatorMaterialInstance->SetScalarParameterValue(HalfWidthParameterName, HalfWidth);
	IndicatorMaterialInstance->SetScalarParameterValue(VisibleLengthParameterName, VisibleLength);
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

void URRAimIndicatorComponent::RefreshIndicator()
{
	const ARRMiner* Miner = GetOwnerMiner();
	if (!IndicatorMaterialInstance || !IsValid(Miner) || Miner->IsActorBeingDestroyed() || Miner->IsDead())
	{
		HideIndicator();
		return;
	}

	const FVector Forward = Miner->GetAimDirection().GetSafeNormal2D(UE_SMALL_NUMBER, FVector::ForwardVector);
	const FVector MinerLocation = Miner->GetActorLocation();
	const UCapsuleComponent* Capsule = Miner->GetCapsuleComponent();
	const FVector Feet = MinerLocation - FVector(0.f, 0.f, Capsule ? Capsule->GetScaledCapsuleHalfHeight() : 0.f);

	// Local X (projection) straight down, local +Z (material forward, UV.x) along the aim, local Y to its right
	// (DeferredDecal.usf: UV = OSPosition.zy * 0.5 + 0.5, OSPosition = local position / DecalSize). Built from axes, not
	// from a pitch -90 rotator, to stay clear of the gimbal ambiguity at that pitch.
	const FQuat Rotation = FRotationMatrix::MakeFromXZ(FVector::DownVector, Forward).ToQuat();
	// UV.x = 0 at the miner: the box centre is HalfLength ahead of the feet.
	SetWorldLocationAndRotation(Feet + Forward * HalfLength, Rotation);

	// Fixed length, cut at the first block cell on the aim line (miner cannot shoot through rock); no grid: fixed length.
	float NewVisibleLength = GetFixedLength();
	if (const ARRArenaGrid* Grid = Miner->GetArenaGrid(); IsValid(Grid))
	{
		NewVisibleLength = FMath::Min(NewVisibleLength, Grid->GetClearDistance(MinerLocation, Forward, NewVisibleLength));
	}
	if (NewVisibleLength != VisibleLength)
	{
		VisibleLength = NewVisibleLength;
		IndicatorMaterialInstance->SetScalarParameterValue(VisibleLengthParameterName, VisibleLength);
	}

	SetVisibility(true);
}

void URRAimIndicatorComponent::HideIndicator()
{
	SetVisibility(false);
}
