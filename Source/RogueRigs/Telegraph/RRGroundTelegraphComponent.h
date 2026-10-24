// Rogue Rigs - ground telegraph (sys-ground-telegraph): a decal projected downward drawing a zone circle and a progress circle growing to its edge.

#pragma once

#include "CoreMinimal.h"
#include "Components/DecalComponent.h"
#include "RRGroundTelegraphComponent.generated.h"

class AActor;
class UMaterialInstanceDynamic;
class UMaterialInterface;

/** Look of a ground telegraph (GDD ground-telegraph, readability-rules): the zone circle colour the material receives. */
UENUM(BlueprintType)
enum class ERRGroundTelegraphMode : uint8
{
	/** Danger zone (barrel fuse, heavy slam): orange, the colour of "explodes". */
	Danger,
	/** Zone the player must stay in while a condition is missing (hack blocked by marked rocks): dotted gold. */
	ZoneBlocked,
	/** Zone the player must stay in, active: dotted white-blue. */
	ZoneActive
};

/**
 * Generic ground signal (GDD ground-telegraph): a decal projected straight down (it follows the floor and the top of rocks)
 * whose material draws a zone circle of Radius and a progress circle growing from the centre to the edge (Progress 0..1).
 * A decal is a scene component, not a primitive: it has no collision, no overlap events and is not navigation relevant, so
 * robots, the miner and player projectiles ignore it. A deferred decal colours every primitive inside its box that receives
 * decals, so actors standing in the zone would take the zone colour: the miner, robots and the drone do not receive decals
 * (DisableDecalsOn, called by ARRMiner, ARREnemy and ARRDrone), and at BeginPlay the telegraph turns decals off on its own
 * owner (bKeepDecalsOffOwner: console, bonus chest, heavy; off for the arena grid, whose floor and rocks must receive it), so
 * the disc and circles only show on the ground under actors (task-312-01). Starts hidden: the owner calls SetRadius / SetMode / SetProgress and Show.
 * It hides itself at once when the owner dies or is pending death (ARREnemy, ARRMiner), when the owner is released to the
 * actor pool (ARREnemy::OnPoolReleased calls HideAllOn; any other owner is caught by the visible-only tick check), and is
 * destroyed with its owner. Material: DecalMaterial when set on the component, else DefaultMaterialPath (config); it drives a
 * dynamic instance with the scalars Radius, Progress, Opacity and the vectors ZoneColor, ProgressColor. Without a material it
 * keeps its values (applied when SetTelegraphMaterial gives one) and logs one warning per run.
 */
UCLASS(Config = Game, ClassGroup = (RogueRigs), meta = (BlueprintSpawnableComponent))
class ROGUERIGS_API URRGroundTelegraphComponent : public UDecalComponent
{
	GENERATED_BODY()

public:
	/** Material parameter names (the material of task-112-02 declares exactly these). */
	static const FName RadiusParameterName;
	static const FName ProgressParameterName;
	static const FName ZoneColorParameterName;
	static const FName ProgressColorParameterName;
	static const FName OpacityParameterName;

	/** Hidden by default, downward projection with absolute rotation, never fades with screen size, no navigation. */
	URRGroundTelegraphComponent(const FObjectInitializer& ObjectInitializer);

	/** Zone radius in uu (clamped to >= 0): sizes the decal box and sets the Radius parameter. */
	UFUNCTION(BlueprintCallable, Category = "Ground Telegraph")
	void SetRadius(float NewRadius);

	/** Progress circle radius as a fraction of the zone radius (clamped to [0, 1]; 1 = touches the zone circle). */
	UFUNCTION(BlueprintCallable, Category = "Ground Telegraph")
	void SetProgress(float NewProgress);

	/** Look of the zone circle: sets ZoneColor from the mode colour. */
	UFUNCTION(BlueprintCallable, Category = "Ground Telegraph")
	void SetMode(ERRGroundTelegraphMode NewMode);

	/** Global opacity of both circles (clamped to [0, 1]). */
	UFUNCTION(BlueprintCallable, Category = "Ground Telegraph")
	void SetOpacity(float NewOpacity);

	/** Shows the telegraph; refused (stays hidden, returns false) while the owner is dead, pending death, pooled idle or being destroyed. */
	UFUNCTION(BlueprintCallable, Category = "Ground Telegraph")
	bool Show();

	/** Hides the telegraph at once and stops its owner check tick. */
	UFUNCTION(BlueprintCallable, Category = "Ground Telegraph")
	void Hide();

	/** True while shown. */
	UFUNCTION(BlueprintPure, Category = "Ground Telegraph")
	bool IsShown() const { return IsVisible(); }

	/** Uses Material as the parent of the dynamic instance and applies the stored values to it (null: drops the instance). */
	UFUNCTION(BlueprintCallable, Category = "Ground Telegraph")
	void SetTelegraphMaterial(UMaterialInterface* Material);

	/** The dynamic instance the parameters are written to (null until a material exists). */
	UMaterialInstanceDynamic* GetTelegraphMaterialInstance() const { return TelegraphMaterialInstance; }

	/** Stored zone radius (uu). */
	float GetRadius() const { return Radius; }
	/** Stored progress [0, 1]. */
	float GetProgress() const { return Progress; }
	/** Stored opacity [0, 1]. */
	float GetOpacity() const { return Opacity; }
	/** Current mode. */
	ERRGroundTelegraphMode GetMode() const { return Mode; }

	/** Colour of the zone circle for Mode (data on the component). */
	FLinearColor GetModeColor(ERRGroundTelegraphMode InMode) const;

	/** True when the owner is gone for the telegraph: invalid, being destroyed, pooled idle, or a dead / pending-death robot or miner. */
	bool IsOwnerGone() const;

	/** Hides every ground telegraph of Actor (owners call it on death and on pool release). */
	static void HideAllOn(const AActor* Actor);

	/** Turns bReceivesDecals off on every primitive of Actor, so ground decals never tint it (miner, robots, drone, carried weapons, telegraph owners). */
	static void DisableDecalsOn(const AActor* Actor);

	/** When true (default), BeginPlay turns decals off on every primitive of the owner (DisableDecalsOn). Set false before registering for an owner whose primitives are the ground (arena grid). */
	void SetKeepDecalsOffOwner(bool bKeep) { bKeepDecalsOffOwner = bKeep; }
	bool GetKeepDecalsOffOwner() const { return bKeepDecalsOffOwner; }

	/** Resolves the material (DecalMaterial or DefaultMaterialPath), creates the dynamic instance, warms the material up (ARRDecalWarmUp) and, with bKeepDecalsOffOwner, keeps decals off the owner. */
	virtual void BeginPlay() override;
	/** Runs only while shown: hides as soon as IsOwnerGone. */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	/** Loads DefaultMaterialPath once per run (cached; a missing asset logs one warning and is not retried). */
	UMaterialInterface* ResolveDefaultMaterial() const;

	/** Writes every stored value to the dynamic instance, if any. */
	void ApplyParameters();

	/** Default material when DecalMaterial is not set on the component (created by task-112-02). Config: [/Script/RogueRigs.RRGroundTelegraphComponent]. */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Ground Telegraph")
	FSoftObjectPath DefaultMaterialPath = FSoftObjectPath(TEXT("/Game/RogueRigs/FX/M_GroundTelegraph.M_GroundTelegraph"));

	/** Half height of the projection box in uu (above and below the component): reaches the floor and the top of rocks. */
	UPROPERTY(EditDefaultsOnly, Category = "Ground Telegraph", meta = (ClampMin = "1"))
	float ProjectionHalfDepth = 256.f;

	/** Owner primitives receive no decals (set at BeginPlay): the zone colour stays on the ground, never on the owner (task-312-01). False for the arena grid. */
	UPROPERTY(EditDefaultsOnly, Category = "Ground Telegraph")
	bool bKeepDecalsOffOwner = true;

	/** Zone circle colour in Danger mode (orange). */
	UPROPERTY(EditDefaultsOnly, Category = "Ground Telegraph")
	FLinearColor DangerColor = FLinearColor(1.f, 0.45f, 0.05f, 1.f);

	/** Zone circle colour in ZoneBlocked mode (gold). */
	UPROPERTY(EditDefaultsOnly, Category = "Ground Telegraph")
	FLinearColor ZoneBlockedColor = FLinearColor(1.f, 0.8f, 0.2f, 1.f);

	/** Zone circle colour in ZoneActive mode (white-blue). */
	UPROPERTY(EditDefaultsOnly, Category = "Ground Telegraph")
	FLinearColor ZoneActiveColor = FLinearColor(0.7f, 0.9f, 1.f, 1.f);

	/** Progress circle colour (white). */
	UPROPERTY(EditDefaultsOnly, Category = "Ground Telegraph")
	FLinearColor ProgressColor = FLinearColor::White;

	/** Zone radius in uu. */
	UPROPERTY(EditAnywhere, Category = "Ground Telegraph", meta = (ClampMin = "0"))
	float Radius = 200.f;

	/** Progress fraction [0, 1]. */
	UPROPERTY(VisibleInstanceOnly, Category = "Ground Telegraph")
	float Progress = 0.f;

	/** Opacity [0, 1]. */
	UPROPERTY(EditAnywhere, Category = "Ground Telegraph", meta = (ClampMin = "0", ClampMax = "1"))
	float Opacity = 1.f;

	/** Current look. */
	UPROPERTY(EditAnywhere, Category = "Ground Telegraph")
	ERRGroundTelegraphMode Mode = ERRGroundTelegraphMode::Danger;

	/** Dynamic instance of the telegraph material (null without a material). */
	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> TelegraphMaterialInstance;
};
