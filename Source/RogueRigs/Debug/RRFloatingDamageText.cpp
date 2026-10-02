// Rogue Rigs - debug floating damage number: a world-space text that rises, fades and faces the camera, then destroys itself.

#include "Debug/RRFloatingDamageText.h"

#include "Camera/PlayerCameraManager.h"
#include "Components/TextRenderComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRFloatingDamageText)

ARRFloatingDamageText::ARRFloatingDamageText(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	TextRender = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextRender"));
	TextRender->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TextRender->SetGenerateOverlapEvents(false);
	TextRender->CastShadow = false;
	TextRender->SetHorizontalAlignment(EHTA_Center);
	TextRender->SetVerticalAlignment(EVRTA_TextCenter);
	SetRootComponent(TextRender);

	// Dark copy behind the main text: drawn first (lower sort priority) and pushed away from the camera.
	OutlineRender = CreateDefaultSubobject<UTextRenderComponent>(TEXT("OutlineRender"));
	OutlineRender->SetupAttachment(TextRender);
	OutlineRender->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	OutlineRender->SetGenerateOverlapEvents(false);
	OutlineRender->CastShadow = false;
	OutlineRender->SetHorizontalAlignment(EHTA_Center);
	OutlineRender->SetVerticalAlignment(EVRTA_TextCenter);
	OutlineRender->TranslucencySortPriority = -1;

	// Engine default text material is opaque; the translucent one reads vertex color alpha, which the fade drives.
	static ConstructorHelpers::FObjectFinderOptional<UMaterialInterface> TranslucentTextMaterial(TEXT("/Engine/EngineMaterials/DefaultTextMaterialTranslucent"));
	TextMaterial = TranslucentTextMaterial.Get();
}

void ARRFloatingDamageText::InitDamageText(float Damage, const FLinearColor& Color, float WorldSize)
{
	BaseColor = Color;
	TextRender->SetText(FText::AsNumber(FMath::RoundToInt(Damage)));
	TextRender->SetTextRenderColor(BaseColor.ToFColor(true));
	TextRender->SetWorldSize(WorldSize);

	OutlineRender->SetText(FText::AsNumber(FMath::RoundToInt(Damage)));
	OutlineRender->SetTextRenderColor(OutlineColor.ToFColor(true));
	OutlineRender->SetWorldSize(WorldSize * OutlineSizeScale);
	OutlineRender->SetRelativeLocation(OutlineOffset);
}

void ARRFloatingDamageText::BeginPlay()
{
	Super::BeginPlay();

	if (TextMaterial)
	{
		TextRender->SetTextMaterial(TextMaterial);
		OutlineRender->SetTextMaterial(TextMaterial);
	}
	// Safety net: the actor goes away even if ticking is stopped.
	SetLifeSpan(Lifetime + 0.1f);
	FaceCamera();
}

void ARRFloatingDamageText::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	Age += DeltaSeconds;
	if (Age >= Lifetime)
	{
		Destroy();
		return;
	}

	AddActorWorldOffset(FVector(0.f, 0.f, RiseSpeed * DeltaSeconds));

	FLinearColor FadedColor = BaseColor;
	FadedColor.A = BaseColor.A * (1.f - Age / Lifetime);
	TextRender->SetTextRenderColor(FadedColor.ToFColor(true));

	FLinearColor FadedOutline = OutlineColor;
	FadedOutline.A = OutlineColor.A * (1.f - Age / Lifetime);
	OutlineRender->SetTextRenderColor(FadedOutline.ToFColor(true));

	FaceCamera();
}

void ARRFloatingDamageText::FaceCamera()
{
	const UWorld* World = GetWorld();
	const APlayerController* PlayerController = World ? World->GetFirstPlayerController() : nullptr;
	const APlayerCameraManager* CameraManager = PlayerController ? PlayerController->PlayerCameraManager.Get() : nullptr;
	if (CameraManager == nullptr)
	{
		return;
	}

	// The text front faces +X: point +X back at the camera, keep the camera's up so the number stays upright on screen.
	const FRotator CameraRotation = CameraManager->GetCameraRotation();
	const FRotationMatrix CameraMatrix(CameraRotation);
	SetActorRotation(FRotationMatrix::MakeFromXZ(-CameraMatrix.GetUnitAxis(EAxis::X), CameraMatrix.GetUnitAxis(EAxis::Z)).Rotator());
}
