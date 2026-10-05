// Rogue Rigs - damage number world subsystem: every hit number of the world, merged per target over a short window, pushed in batches to one Niagara system.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "UObject/ObjectKey.h"
#include "RRDamageNumberSubsystem.generated.h"

class AActor;
class UNiagaraComponent;
class UNiagaraSystem;

/** One damage number ready to show: merged value, style and world position. */
USTRUCT(BlueprintType)
struct ROGUERIGS_API FRRDamageNumber
{
	GENERATED_BODY()

	/** World position of the number (last hit location of its target + the entry's horizontal jitter). */
	UPROPERTY(BlueprintReadOnly, Category = "Damage Numbers")
	FVector Location = FVector::ZeroVector;

	/** Sum of the merged hits (raw damage, before the health clamp). */
	UPROPERTY(BlueprintReadOnly, Category = "Damage Numbers")
	float Value = 0.f;

	/** True when at least one merged hit was critical. */
	UPROPERTY(BlueprintReadOnly, Category = "Damage Numbers")
	bool bCritical = false;

	/** World size of the glyphs (NumberSize, x CriticalScale on a crit). */
	UPROPERTY(BlueprintReadOnly, Category = "Damage Numbers")
	float Size = 0.f;

	/** Color of the number (NormalColor or CriticalColor). */
	UPROPERTY(BlueprintReadOnly, Category = "Damage Numbers")
	FLinearColor Color = FLinearColor::White;

	/** World time (seconds, pauses with the game) at which the number was pushed. */
	UPROPERTY(BlueprintReadOnly, Category = "Damage Numbers")
	float SpawnTime = 0.f;
};

/** Native event of URRDamageNumberSubsystem: the batch of numbers pushed this tick (never broadcast empty). */
DECLARE_MULTICAST_DELEGATE_OneParam(FRROnDamageNumbersPushed, const TArray<FRRDamageNumber>& /*Numbers*/);

/**
 * The single path of floating damage numbers (damage-numbers, sys-damage-numbers). Damage sources call AddHit on each hit
 * (ARREnemy, ARRTargetDummy). Hits on the same target within MergeWindow seconds (world time, from the first hit) add up
 * into one number, critical if any merged hit was; the number keeps the last hit location, so a robot that dies or is
 * destroyed still gets its number where it was. Nothing is spawned per hit: on each tick the due entries (at most
 * MaxNumbersPerTick, the rest wait one tick) are pushed to one persistent UNiagaraComponent through Niagara array user
 * parameters, and the batch size through an int user parameter that the emitter spawns from (reset to 0 on the next
 * tick without numbers). The Niagara system is NumberSystem (config, DefaultGame.ini section
 * [/Script/RogueRigs.RRDamageNumberSubsystem]); unset or missing = no rendering, the merge and OnNumbersPushed still run.
 * The player toggle is URRGameUserSettings::bShowDamageNumbers: off = hits ignored and pending numbers dropped.
 * Ticks with the world (stops while the game is paused, like the Niagara component).
 */
UCLASS(Config = Game)
class ROGUERIGS_API URRDamageNumberSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	/** Niagara user parameter (Niagara Position Array): world position of each new number. */
	static const FName PositionsParameter;
	/** Niagara user parameter (Niagara Float Array): value of each new number (already summed, not rounded). */
	static const FName ValuesParameter;
	/** Niagara user parameter (Niagara Float Array): glyph world size of each new number (crit = NumberSize x CriticalScale). */
	static const FName SizesParameter;
	/** Niagara user parameter (Niagara Color Array): color of each new number. */
	static const FName ColorsParameter;
	/** Niagara user parameter (Niagara Bool Array): critical flag of each new number (drives the pop). */
	static const FName CriticalsParameter;
	/** Niagara user parameter (Niagara Float Array): world time of the push, same for the whole batch. */
	static const FName SpawnTimesParameter;
	/** Niagara user parameter (int32): count of new numbers this frame; 0 on frames without numbers. */
	static const FName NewCountParameter;

	/** Default values of the config properties (NumberSystem path included). */
	URRDamageNumberSubsystem();

	/**
	 * Records a hit of Value on Target shown at Location (world, e.g. above the capsule). Merges into Target's open entry
	 * when its window is still open, else opens a new one (with a random horizontal jitter). Ignored when Value <= 0 or
	 * the player turned damage numbers off. A null Target never merges. Safe inside a GE execution (no spawn, no broadcast).
	 */
	UFUNCTION(BlueprintCallable, Category = "Damage Numbers")
	void AddHit(AActor* Target, float Value, bool bCritical, FVector Location);

	/** True when the player setting shows damage numbers (true when the game user settings are not URRGameUserSettings). */
	UFUNCTION(BlueprintPure, Category = "Damage Numbers")
	bool AreNumbersShown() const;

	/** Number of entries waiting for their merge window to close. */
	UFUNCTION(BlueprintPure, Category = "Damage Numbers")
	int32 GetPendingCount() const { return Pending.Num(); }

	/** Merge window in seconds. */
	UFUNCTION(BlueprintPure, Category = "Damage Numbers")
	float GetMergeWindow() const { return MergeWindow; }

	/** The persistent Niagara component fed by the subsystem; null when NumberSystem is unset or failed to load. */
	UNiagaraComponent* GetNiagaraComponent() const { return NiagaraComponent; }

	/** Total numbers pushed to Niagara (or broadcast without a component) since the world began (tests, debug). */
	int32 GetTotalPushedCount() const { return TotalPushedCount; }

	/**
	 * Live and total spawned particle counts summed over the emitters of the Niagara component (tests, rr.Debug.Shots).
	 * False with both counts 0 when there is no component or no system instance.
	 */
	bool GetParticleStats(int32& OutLiveParticles, int32& OutSpawnedParticles) const;

	/** Batch of numbers pushed on each tick that has some (tests, debug listeners). */
	FRROnDamageNumbersPushed OnNumbersPushed;

	/** Pushes the due numbers. */
	virtual void Tick(float DeltaTime) override;

	/** Stat id of the tick. */
	virtual TStatId GetStatId() const override;

protected:
	/** One open merge entry. */
	struct FPendingNumber
	{
		/** Target key (unset for hits without a target). */
		FObjectKey Target;
		/** Last hit location (without jitter). */
		FVector HitLocation = FVector::ZeroVector;
		/** Horizontal jitter drawn when the entry opened, kept for its later hits. */
		FVector Jitter = FVector::ZeroVector;
		/** Merged value. */
		float Value = 0.f;
		/** Any merged hit critical. */
		bool bCritical = false;
		/** World time of the first hit; the entry is due MergeWindow seconds later. */
		double FirstHitTime = 0.0;
	};

	/** Game worlds only (game, PIE): no numbers in editor worlds. */
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

	/** Loads NumberSystem and creates the persistent Niagara component (null-safe). */
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	/** Drops pending entries and the component. */
	virtual void Deinitialize() override;

	/** Sends Numbers (may be empty) to the Niagara component, when there is one. */
	void PushToNiagara(const TArray<FRRDamageNumber>& Numbers);

	/** Niagara system rendering the numbers (digit atlas, CPU sim, one emitter spawning NewCount particles per frame). */
	UPROPERTY(Config)
	TSoftObjectPtr<UNiagaraSystem> NumberSystem;

	/** Hits on one target within this many seconds (from its first hit) merge into one number (damage-numbers: 0.25 s). */
	UPROPERTY(Config)
	float MergeWindow = 0.25f;

	/** Glyph world size of a normal number (uu). */
	UPROPERTY(Config)
	float NumberSize = 60.f;

	/** Size multiplier of a critical number (damage-numbers: 1.6). */
	UPROPERTY(Config)
	float CriticalScale = 1.6f;

	/** Color of a normal number (white). */
	UPROPERTY(Config)
	FLinearColor NormalColor = FLinearColor::White;

	/** Color of a critical number (white-gold). */
	UPROPERTY(Config)
	FLinearColor CriticalColor = FLinearColor(1.f, 0.85f, 0.45f);

	/** Random horizontal offset (uu, each axis in [-Jitter, Jitter]) drawn per entry so neighbouring numbers stay readable. */
	UPROPERTY(Config)
	float Jitter = 25.f;

	/** At most this many numbers pushed per tick; the oldest go first, the rest wait (keeps horde frames readable and bounded). */
	UPROPERTY(Config)
	int32 MaxNumbersPerTick = 64;

	/** The persistent component (world-owned, not attached), created at world BeginPlay. */
	UPROPERTY(Transient)
	TObjectPtr<UNiagaraComponent> NiagaraComponent;

	/** Open entries, oldest first. */
	TArray<FPendingNumber> Pending;

	/** Count pushed on the last tick, so the 0 reset is sent once. */
	int32 LastPushedCount = 0;

	/** Running total of pushed numbers. */
	int32 TotalPushedCount = 0;
};
