// Rogue Rigs - ground telegraph (sys-ground-telegraph): a decal projected downward drawing a zone circle and a progress circle growing to its edge.

#include "Telegraph/RRGroundTelegraphComponent.h"

#include "Components/PrimitiveComponent.h"
#include "Enemies/RREnemy.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Misc/PackageName.h"
#include "Player/RRMiner.h"
#include "Pool/RRActorPoolSubsystem.h"
#include "RogueRigs.h"
#include "Telegraph/RRDecalWarmUp.h"

const FName URRGroundTelegraphComponent::RadiusParameterName(TEXT("Radius"));
const FName URRGroundTelegraphComponent::ProgressParameterName(TEXT("Progress"));
const FName URRGroundTelegraphComponent::ZoneColorParameterName(TEXT("ZoneColor"));
const FName URRGroundTelegraphComponent::ProgressColorParameterName(TEXT("ProgressColor"));
const FName URRGroundTelegraphComponent::OpacityParameterName(TEXT("Opacity"));

namespace RRGroundTelegraph
{
	/** Default material resolved once per process, and again once collected (the path it was resolved for, the result, whether it was missing). */
	FSoftObjectPath ResolvedPath;
	TWeakObjectPtr<UMaterialInterface> ResolvedMaterial;
	bool bResolved = false;
	bool bWarned = false;
}

URRGroundTelegraphComponent::URRGroundTelegraphComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	bCanEverAffectNavigation = false;
	SetVisibleFlag(false);
	// Projects along its X axis: pointing straight down whatever the owner's rotation.
	SetUsingAbsoluteRotation(true);
	SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));
	// Top-down camera: never fade out with screen size.
	FadeScreenSize = 0.f;
	DecalSize = FVector(ProjectionHalfDepth, Radius, Radius);
}

void URRGroundTelegraphComponent::BeginPlay()
{
	Super::BeginPlay();

	// The decal box covers its owner (console, chest, heavy): without this the owner's meshes take the zone colour (task-312-01).
	if (bKeepDecalsOffOwner)
	{
		DisableDecalsOn(GetOwner());
	}

	UMaterialInterface* Material = DecalMaterial;
	if (Material && Material->IsA<UMaterialInstanceDynamic>())
	{
		Material = Cast<UMaterialInstanceDynamic>(Material)->Parent;
	}
	UMaterialInterface* const TelegraphMaterial = Material ? Material : ResolveDefaultMaterial();
	SetTelegraphMaterial(TelegraphMaterial);
	// Draw the material once out of sight (once per world): its first shown frame is not the engine default decal (task-201-01).
	ARRDecalWarmUp::WarmUp(GetWorld(), TelegraphMaterial);
}

UMaterialInterface* URRGroundTelegraphComponent::ResolveDefaultMaterial() const
{
	using namespace RRGroundTelegraph;
	// Stale: a package build collects the material with the first world (nothing keeps it between two maps). A null DecalMaterial
	// would draw the engine default decal (the green square) for every later run of the process (task-431-01).
	if (!bResolved || ResolvedPath != DefaultMaterialPath || ResolvedMaterial.IsStale())
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
			UE_LOG(LogRogueRigs, Warning, TEXT("Ground telegraph: material %s not found; telegraphs keep their values but draw nothing until a material exists (task-112-02)."),
				*DefaultMaterialPath.ToString());
		}
	}
	return ResolvedMaterial.Get();
}

void URRGroundTelegraphComponent::SetTelegraphMaterial(UMaterialInterface* Material)
{
	if (!Material)
	{
		TelegraphMaterialInstance = nullptr;
		SetDecalMaterial(nullptr);
		return;
	}
	TelegraphMaterialInstance = UMaterialInstanceDynamic::Create(Material, this);
	SetDecalMaterial(TelegraphMaterialInstance);
	ApplyParameters();
}

void URRGroundTelegraphComponent::SetRadius(float NewRadius)
{
	Radius = FMath::Max(0.f, NewRadius);
	DecalSize = FVector(ProjectionHalfDepth, Radius, Radius);
	MarkRenderStateDirty();
	UpdateBounds();
	ApplyParameters();
}

void URRGroundTelegraphComponent::SetProgress(float NewProgress)
{
	Progress = FMath::Clamp(NewProgress, 0.f, 1.f);
	if (TelegraphMaterialInstance)
	{
		TelegraphMaterialInstance->SetScalarParameterValue(ProgressParameterName, Progress);
	}
}

void URRGroundTelegraphComponent::SetMode(ERRGroundTelegraphMode NewMode)
{
	Mode = NewMode;
	if (TelegraphMaterialInstance)
	{
		TelegraphMaterialInstance->SetVectorParameterValue(ZoneColorParameterName, GetModeColor(Mode));
	}
}

void URRGroundTelegraphComponent::SetOpacity(float NewOpacity)
{
	Opacity = FMath::Clamp(NewOpacity, 0.f, 1.f);
	if (TelegraphMaterialInstance)
	{
		TelegraphMaterialInstance->SetScalarParameterValue(OpacityParameterName, Opacity);
	}
}

FLinearColor URRGroundTelegraphComponent::GetModeColor(ERRGroundTelegraphMode InMode) const
{
	switch (InMode)
	{
	case ERRGroundTelegraphMode::ZoneBlocked:
		return ZoneBlockedColor;
	case ERRGroundTelegraphMode::ZoneActive:
		return ZoneActiveColor;
	default:
		return DangerColor;
	}
}

void URRGroundTelegraphComponent::ApplyParameters()
{
	if (!TelegraphMaterialInstance)
	{
		return;
	}
	TelegraphMaterialInstance->SetScalarParameterValue(RadiusParameterName, Radius);
	TelegraphMaterialInstance->SetScalarParameterValue(ProgressParameterName, Progress);
	TelegraphMaterialInstance->SetScalarParameterValue(OpacityParameterName, Opacity);
	TelegraphMaterialInstance->SetVectorParameterValue(ZoneColorParameterName, GetModeColor(Mode));
	TelegraphMaterialInstance->SetVectorParameterValue(ProgressColorParameterName, ProgressColor);
}

bool URRGroundTelegraphComponent::Show()
{
	if (IsOwnerGone())
	{
		Hide();
		return false;
	}
	SetVisibility(true);
	SetComponentTickEnabled(true);
	return true;
}

void URRGroundTelegraphComponent::Hide()
{
	SetVisibility(false);
	SetComponentTickEnabled(false);
}

bool URRGroundTelegraphComponent::IsOwnerGone() const
{
	const AActor* Owner = GetOwner();
	if (!IsValid(Owner) || Owner->IsActorBeingDestroyed() || URRActorPoolSubsystem::IsPooledIdle(Owner))
	{
		return true;
	}
	if (const ARREnemy* Enemy = Cast<ARREnemy>(Owner))
	{
		return Enemy->IsDead() || Enemy->IsDeathPending();
	}
	if (const ARRMiner* Miner = Cast<ARRMiner>(Owner))
	{
		return Miner->IsDead();
	}
	return false;
}

void URRGroundTelegraphComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (IsOwnerGone())
	{
		Hide();
	}
}

void URRGroundTelegraphComponent::HideAllOn(const AActor* Actor)
{
	if (!Actor)
	{
		return;
	}
	TInlineComponentArray<URRGroundTelegraphComponent*> Telegraphs(Actor);
	for (URRGroundTelegraphComponent* Telegraph : Telegraphs)
	{
		Telegraph->Hide();
	}
}

void URRGroundTelegraphComponent::DisableDecalsOn(const AActor* Actor)
{
	if (!Actor)
	{
		return;
	}
	TInlineComponentArray<UPrimitiveComponent*> Primitives(Actor);
	for (UPrimitiveComponent* Primitive : Primitives)
	{
		if (Primitive->bReceivesDecals)
		{
			Primitive->SetReceivesDecals(false);
		}
	}
}
