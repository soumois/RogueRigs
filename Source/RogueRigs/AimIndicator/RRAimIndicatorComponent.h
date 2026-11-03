// Rogue Rigs - aim indicator (gdd aim-indicator): a thin white-gold laser from the shown rifle Muzzle along the miner's aim, stopped at the first robot or block.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "RRAimIndicatorComponent.generated.h"

class ARRMiner;
class UMaterialInstanceDynamic;
class UMaterialInterface;
class UStaticMesh;
class UStaticMeshComponent;

/**
 * Aim laser of the miner (gdd aim-indicator, task-415-02): at BeginPlay this scene component creates LaserMesh, a transient
 * static mesh component drawing a flat engine Plane (DefaultMeshPath, /Engine/BasicShapes/Plane) with M_AimLaser
 * (DefaultMaterialPath), both loaded by path. Pure visual: no collision, no overlap, no shadow, no navigation, no effect on
 * aim, fire, turrets or the drone. The component itself stays a plain scene component (not a primitive) so the AimIndicator
 * subobject that BP_Miner saved when it was a decal still loads (a static mesh component reads more native data than a decal
 * wrote: serial size mismatch).
 * Each tick (TG_PostPhysics, after the miner turned and moved and the hand pose moved the rifle) the plane is laid
 * horizontally at the Muzzle height (parallel to the floor, never in it), starting at the Muzzle of the shown hand weapon
 * (ARRWeapon::FindHandWeapon; without one, the first weapon's GetShotOrigin; without a weapon, hidden), along
 * ARRMiner::GetAimDirection kept horizontal. Its width is Width; its length (GetLaserLength) is the nearer of the first block
 * cell on the aim line (ARRArenaGrid::GetClearDistance from the Muzzle) and the first living robot hit capsule on the aim line
 * (FindRobotDistance), capped at MaxLength (MaxLength without grid and robot). The plane's UV.x runs from 0 at the Muzzle to
 * 1 at the tip, UV.y across (the M_AimLaser contract). Whatever ends it, its last TipFadeLength uu fade to zero (task-423):
 * the material TipFadeFrac is pushed again only when the length changes it.
 * Visible whenever the owning miner is alive, mouse and stick aim alike; hidden at death (ARRMiner::HandleDeath calls
 * HideIndicator, and the tick hides it while the miner IsDead). A missing mesh or material logs one warning per run and
 * nothing is drawn (tick off).
 */
UCLASS(Config = Game, ClassGroup = (RogueRigs), meta = (BlueprintSpawnableComponent))
class ROGUERIGS_API URRAimIndicatorComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	/** Material parameter names (M_AimLaser of task-415-01 declares exactly these). */
	static const FName ColorParameterName;
	static const FName EmissiveGainParameterName;
	static const FName OpacityParameterName;
	static const FName CoreFracParameterName;
	static const FName GlowOpacityParameterName;
	static const FName TipFadeFracParameterName;

	/** Ticks in TG_PostPhysics; no navigation. The laser mesh is created at BeginPlay, hidden until the first refresh. */
	URRAimIndicatorComponent(const FObjectInitializer& ObjectInitializer);

	/** Resolves the plane mesh and the material, creates LaserMesh and the dynamic instance, writes every parameter, then places the laser once. */
	virtual void BeginPlay() override;

	/** Destroys LaserMesh. */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Each frame: RefreshIndicator. */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/**
	 * Hidden when the owner is not a living ARRMiner, there is no mesh, material or shot origin, or the length is 0; otherwise
	 * lays the plane from the Muzzle along the aim over GetLaserLength and shows it.
	 */
	void RefreshIndicator();

	/** Hides the laser at once (miner death). The next RefreshIndicator shows it again only while the miner is alive. */
	void HideIndicator();

	/** True while drawn. */
	UFUNCTION(BlueprintPure, Category = "Aim Indicator")
	bool IsIndicatorShown() const;

	/** The plane drawing the laser (created at BeginPlay; null before it or without mesh or material). Absolute transform. */
	UFUNCTION(BlueprintPure, Category = "Aim Indicator")
	UStaticMeshComponent* GetLaserMesh() const { return LaserMesh; }

	/** Maximum laser length in uu (MaxLength): the length with no block and no robot on the aim line. */
	UFUNCTION(BlueprintPure, Category = "Aim Indicator")
	float GetMaxLength() const { return MaxLength; }

	/** Laser length in uu from the Muzzle at the last refresh (MaxLength before the first one). */
	UFUNCTION(BlueprintPure, Category = "Aim Indicator")
	float GetLaserLength() const { return LaserLength; }

	/** Fixed fade length in uu at the tip (TipFadeLength). */
	UFUNCTION(BlueprintPure, Category = "Aim Indicator")
	float GetTipFadeLength() const { return TipFadeLength; }

	/** Material TipFadeFrac for a laser of Length uu: min(TipFadeLength / Length, 0.5); 0.5 for a length of 0 or less. */
	UFUNCTION(BlueprintPure, Category = "Aim Indicator")
	float ComputeTipFadeFrac(float Length) const;

	/** TipFadeFrac last pushed to the material (negative before the first push). */
	UFUNCTION(BlueprintPure, Category = "Aim Indicator")
	float GetPushedTipFadeFrac() const { return PushedTipFadeFrac; }

	/** World location the laser started from at the last refresh (the hand weapon Muzzle). */
	UFUNCTION(BlueprintPure, Category = "Aim Indicator")
	FVector GetLaserOrigin() const { return LaserOrigin; }

	/** The dynamic instance the parameters are written to (null without a material). */
	UMaterialInstanceDynamic* GetIndicatorMaterialInstance() const { return IndicatorMaterialInstance; }

protected:
	/** Loads Path into Cache, again only when the path changed or the cached asset is gone; a missing asset logs one warning per run naming What. */
	static UObject* ResolveAsset(const FSoftObjectPath& Path, const TCHAR* What, FSoftObjectPath& CachedPath, TWeakObjectPtr<UObject>& Cache, bool& bWarned);

	/** Writes Color, EmissiveGain, Opacity, CoreFrac, GlowOpacity and the TipFadeFrac of LaserLength to the dynamic instance, if any. */
	void ApplyParameters();

	/** The owner as a miner (null for any other owner: the laser stays hidden). */
	ARRMiner* GetOwnerMiner() const;

	/**
	 * Laser start for Miner: the Muzzle of its shown hand weapon; without one, the first weapon's GetShotOrigin. False without a
	 * weapon. OutVerticalHalfHeight: the half height of the hand weapon's shot capsule (ARRWeapon::GetShotPlacement), 0 otherwise.
	 */
	bool FindLaserOrigin(const ARRMiner& Miner, FVector& OutOrigin, float& OutVerticalHalfHeight) const;

	/**
	 * Distance in uu from Origin along Direction (horizontal, unit) to the first living robot hit capsule (ARREnemy::GetHitCapsule,
	 * not IsDead) touched by a vertical capsule of radius RobotSweepRadius and half height max(VerticalHalfHeight, radius),
	 * swept over MaxDistance against Pawn objects, the miner ignored (drone, turrets, projectiles and pickups are not robots and
	 * are skipped). MaxDistance when none.
	 */
	float FindRobotDistance(const ARRMiner& Miner, const FVector& Origin, const FVector& Direction, float VerticalHalfHeight, float MaxDistance) const;

	/** Laser plane mesh (task-415-02). Config: [/Script/RogueRigs.RRAimIndicatorComponent]. */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Aim Indicator")
	FSoftObjectPath DefaultMeshPath = FSoftObjectPath(TEXT("/Engine/BasicShapes/Plane.Plane"));

	/** Laser material (task-415-01). Config: [/Script/RogueRigs.RRAimIndicatorComponent]. */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Aim Indicator")
	FSoftObjectPath DefaultMaterialPath = FSoftObjectPath(TEXT("/Game/RogueRigs/FX/M_AimLaser.M_AimLaser"));

	/** Side of the plane mesh in uu (engine Plane: 100), used to scale it to the laser length and width. */
	UPROPERTY(EditDefaultsOnly, Category = "Aim Indicator", meta = (ClampMin = "1", Units = "cm"))
	float LaserMeshSize = 100.f;

	/** Maximum laser length in uu (task-423 Q2: 600, provisional, owner 2026-11-03). */
	UPROPERTY(EditDefaultsOnly, Category = "Aim Indicator", meta = (ClampMin = "0", Units = "cm"))
	float MaxLength = 600.f;

	/** Full laser width in uu: 3 uu core + 8 uu glow each side (task-415 Q5: 19, provisional). */
	UPROPERTY(EditDefaultsOnly, Category = "Aim Indicator", meta = (ClampMin = "0", Units = "cm"))
	float Width = 19.f;

	/** Radius in uu of the sweep that finds the first robot on the aim line (half the laser Width: the drawn laser stops where it touches a robot). */
	UPROPERTY(EditDefaultsOnly, Category = "Aim Indicator", meta = (ClampMin = "0", Units = "cm"))
	float RobotSweepRadius = 9.5f;

	/** Laser colour, linear RGB (white-gold sRGB (255, 236, 170), M_AimLaser default). Material Color. */
	UPROPERTY(EditDefaultsOnly, Category = "Aim Indicator")
	FLinearColor Color = FLinearColor(1.f, 0.84f, 0.40f, 1.f);

	/** Emissive multiplier of the colour (M_AimLaser default 1). Material EmissiveGain. */
	UPROPERTY(EditDefaultsOnly, Category = "Aim Indicator", meta = (ClampMin = "0"))
	float EmissiveGain = 1.f;

	/** Opacity of the core (M_AimLaser default 1). Material Opacity. */
	UPROPERTY(EditDefaultsOnly, Category = "Aim Indicator", meta = (ClampMin = "0", ClampMax = "1"))
	float Opacity = 1.f;

	/** Share of the width taken by the bright core (M_AimLaser default 0.16 = 3 / 19). Material CoreFrac. */
	UPROPERTY(EditDefaultsOnly, Category = "Aim Indicator", meta = (ClampMin = "0", ClampMax = "1"))
	float CoreFrac = 0.16f;

	/** Opacity of the soft glow around the core (M_AimLaser default 0.3). Material GlowOpacity. */
	UPROPERTY(EditDefaultsOnly, Category = "Aim Indicator", meta = (ClampMin = "0", ClampMax = "1"))
	float GlowOpacity = 0.3f;

	/**
	 * Length in uu, at the tip, fading to zero, whatever ends the laser (maximum range, block, robot) (task-423 Q3: 150,
	 * provisional, owner 2026-11-03). Pushed as the material TipFadeFrac = min(TipFadeLength / length, 0.5): a laser shorter
	 * than twice this length fades over its last half.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Aim Indicator", meta = (ClampMin = "0", Units = "cm"))
	float TipFadeLength = 150.f;

	/** Pushes TipFadeFrac for Length to the dynamic instance, only when it differs from the last pushed value. */
	void UpdateTipFade(float Length);

	/** Laser length in uu at the last refresh. */
	float LaserLength = 0.f;

	/** TipFadeFrac last written to the dynamic instance (negative before the first write: the next update always pushes). */
	float PushedTipFadeFrac = -1.f;

	/** Laser start (world) at the last refresh. */
	FVector LaserOrigin = FVector::ZeroVector;

	/** Plane component drawing the laser (transient, created at BeginPlay). */
	UPROPERTY(Transient)
	TObjectPtr<UStaticMeshComponent> LaserMesh;

	/** Dynamic instance of the laser material (null without a material). */
	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> IndicatorMaterialInstance;
};
