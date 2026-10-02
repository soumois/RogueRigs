// Rogue Rigs - target dummy: immobile, never-dying test target with an ASC and health, hit flash and debug damage numbers.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Actor.h"
#include "RRTargetDummy.generated.h"

class ARRFloatingDamageText;
class UMaterialInstanceDynamic;
class UStaticMeshComponent;
class URRAbilitySystemComponent;
class URRHealthAttributeSet;

/**
 * Punching ball to test weapons and hit feedback. Owns a URRAbilitySystemComponent and a URRHealthAttributeSet
 * (Health starts at MaxHealth). The Mesh root has object type Pawn and overlaps the Projectile channel, so
 * projectiles overlap it and apply their damage spec. On each hit (URRHealthAttributeSet::OnDamageReceived):
 * logs the hit, flashes the mesh to FlashColor, spawns a floating damage number (bigger, white-gold on a crit).
 * Health refills to MaxHealth RefillDelay seconds after the last hit, or instantly when it reaches 0: it never dies.
 * Abstract: place a Blueprint subclass that sets the mesh and its M_GreyBox material.
 */
UCLASS(Abstract)
class ROGUERIGS_API ARRTargetDummy : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	/** Creates the mesh root (Pawn object type, overlaps Projectile), the ASC and the health set. */
	ARRTargetDummy(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** IAbilitySystemInterface: the dummy's own ASC (lets projectiles find it). */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	/** Initializes the ASC actor info, sets MaxHealth then Health, binds the damage event, creates the flash material instance. */
	virtual void BeginPlay() override;

	/** Unbinds the damage event and clears the flash and refill timers. */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Per-hit feedback: log, flash, floating number, refill scheduling. Bound to URRHealthAttributeSet::OnDamageReceived. */
	void HandleDamageReceived(float Damage, bool bIsCritical, float HealthLeft);

	/** Sets the Color parameter of the mesh's dynamic material to FlashColor and schedules EndFlash. */
	void StartFlash();

	/** Restores the Color parameter to the material's original value. */
	void EndFlash();

	/** Spawns FloatingTextClass above the dummy with the rounded damage, styled for a normal or critical hit. */
	void SpawnFloatingText(float Damage, bool bIsCritical);

	/** Sets Health back to the current MaxHealth. */
	void RefillHealth();

	/** Visible body and hit volume. Object type Pawn, blocks everything except the Projectile channel (overlap). Mesh and material set in the Blueprint. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Target Dummy")
	TObjectPtr<UStaticMeshComponent> Mesh;

	/** The dummy's ability system; receives the projectiles' damage specs. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Target Dummy")
	TObjectPtr<URRAbilitySystemComponent> AbilitySystemComponent;

	/** Health, MaxHealth and IncomingDamage; registered on the ASC as a default subobject. */
	UPROPERTY()
	TObjectPtr<URRHealthAttributeSet> HealthSet;

	/** MaxHealth (and starting Health) given to the health set at BeginPlay. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Target Dummy|Health", meta = (ClampMin = "1"))
	float MaxHealth = 1000.f;

	/** Seconds after the last hit before Health refills to MaxHealth. 0 or less refills after every hit. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Target Dummy|Health", meta = (ClampMin = "0", Units = "s"))
	float RefillDelay = 3.f;

	/** Duration of the hit flash. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Target Dummy|Flash", meta = (ClampMin = "0.01", Units = "s"))
	float FlashDuration = 0.1f;

	/** Color written to the material during the flash. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Target Dummy|Flash")
	FLinearColor FlashColor = FLinearColor::White;

	/** Vector parameter of the mesh material (slot 0) driven by the flash; "Color" on M_GreyBox. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Target Dummy|Flash")
	FName ColorParameterName = TEXT("Color");

	/** Actor spawned per hit to show the damage number. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Target Dummy|Damage Text")
	TSubclassOf<ARRFloatingDamageText> FloatingTextClass;

	/** Spawn point of the damage number, relative to the actor location (world axes). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Target Dummy|Damage Text")
	FVector TextSpawnOffset = FVector(0.f, 0.f, 150.f);

	/** Random horizontal offset (uu, each axis in [-Jitter, Jitter]) so stacked numbers stay readable. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Target Dummy|Damage Text", meta = (ClampMin = "0"))
	float TextSpawnJitter = 25.f;

	/** Color of a normal hit's number (white, color-code). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Target Dummy|Damage Text")
	FLinearColor NormalTextColor = FLinearColor::White;

	/** Color of a critical hit's number (white-gold, color-code). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Target Dummy|Damage Text")
	FLinearColor CriticalTextColor = FLinearColor(1.f, 0.85f, 0.45f);

	/** World size (uu) of a normal hit's number. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Target Dummy|Damage Text", meta = (ClampMin = "1"))
	float NormalTextSize = 60.f;

	/** World size (uu) of a critical hit's number (larger than normal). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Target Dummy|Damage Text", meta = (ClampMin = "1"))
	float CriticalTextSize = 90.f;

	/** Dynamic instance of the mesh material (slot 0), created at BeginPlay; null if the mesh has no material. */
	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> FlashMaterial;

	/** Value of ColorParameterName before any flash, restored by EndFlash. */
	FLinearColor BaseColor = FLinearColor::White;

	/** Pending EndFlash call. */
	FTimerHandle FlashTimerHandle;

	/** Pending RefillHealth call, restarted by each hit. */
	FTimerHandle RefillTimerHandle;

	/** Binding on HealthSet->OnDamageReceived. */
	FDelegateHandle DamageReceivedHandle;
};
