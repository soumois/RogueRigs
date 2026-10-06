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

/** One digit sprite of a damage number, laid out by URRDamageNumberSubsystem: what the Niagara emitter draws per particle. */
USTRUCT(BlueprintType)
struct ROGUERIGS_API FRRDamageDigit
{
	GENERATED_BODY()

	/** World position of the digit centre (number centre + its offset along DigitDirection). */
	UPROPERTY(BlueprintReadOnly, Category = "Damage Numbers")
	FVector Location = FVector::ZeroVector;

	/** Digit value 0-9 (atlas cell). */
	UPROPERTY(BlueprintReadOnly, Category = "Damage Numbers")
	int32 Digit = 0;

	/** Glyph world size (the number's size). */
	UPROPERTY(BlueprintReadOnly, Category = "Damage Numbers")
	float Size = 0.f;

	/** Color (the number's color). */
	UPROPERTY(BlueprintReadOnly, Category = "Damage Numbers")
	FLinearColor Color = FLinearColor::White;

	/** True when the number is critical (drives the pop). */
	UPROPERTY(BlueprintReadOnly, Category = "Damage Numbers")
	bool bCritical = false;

	/** World time of the push (the number's spawn time). */
	UPROPERTY(BlueprintReadOnly, Category = "Damage Numbers")
	float SpawnTime = 0.f;
};

/** Native event of URRDamageNumberSubsystem: the batch of numbers pushed this tick (never broadcast empty). */
DECLARE_MULTICAST_DELEGATE_OneParam(FRROnDamageNumbersPushed, const TArray<FRRDamageNumber>& /*Numbers*/);

/**
 * The single path of floating damage numbers (damage-numbers, sys-damage-numbers). Damage sources call AddHit on each hit
 * (ARREnemy, ARRTargetDummy). Hits on the same target within MergeWindow seconds (world time, from the first hit) add up
 * into one number, critical if any merged hit was; the number keeps the last hit location, so a robot that dies or is
 * destroyed still gets its number where it was. A new entry whose screen footprint overlaps another open entry is stacked
 * above it along StackDirection (crowd stacking, damage-numbers). Nothing is spawned per hit: on each tick the due entries (at most
 * MaxNumbersPerTick, the rest wait one tick) are laid out into digits here (left to right along DigitDirection, centred
 * on the number position, advance DigitAdvance x size) and pushed to one persistent UNiagaraComponent as one entry per
 * digit in Niagara array user parameters, with the digit count and a batch id incremented on every push (never reset),
 * so the emitter spawns DigitCount particles once per batch id change and particle N reads entry N. The Niagara system is NumberSystem (config, DefaultGame.ini section
 * [/Script/RogueRigs.RRDamageNumberSubsystem]); unset or missing = no rendering, the merge and OnNumbersPushed still run.
 * The player toggle is URRGameUserSettings::bShowDamageNumbers: off = hits ignored and pending numbers dropped.
 * Ticks with the world (stops while the game is paused, like the Niagara component).
 */
UCLASS(Config = Game)
class ROGUERIGS_API URRDamageNumberSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	/** Niagara user parameter (Niagara Position Array): world position of each digit of the batch. */
	static const FName DigitPositionsParameter;
	/** Niagara user parameter (Niagara Int32 Array): value 0-9 of each digit (atlas cell). */
	static const FName DigitIndicesParameter;
	/** Niagara user parameter (Niagara Float Array): glyph world size of each digit (crit = NumberSize x CriticalScale). */
	static const FName DigitSizesParameter;
	/** Niagara user parameter (Niagara Color Array): color of each digit. */
	static const FName DigitColorsParameter;
	/** Niagara user parameter (Niagara Int32 Array): 1 when the digit's number is critical, else 0 (drives the pop). */
	static const FName DigitCriticalsParameter;
	/** Niagara user parameter (Niagara Float Array): world time of the push, same for the whole batch. */
	static const FName DigitSpawnTimesParameter;
	/** Niagara user parameter (int32): number of digits of the last batch (array length); kept until the next batch. */
	static const FName DigitCountParameter;
	/** Niagara user parameter (int32): id of the last batch, +1 per push, never reset (0 = nothing pushed yet). */
	static const FName DigitBatchIdParameter;

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

	/**
	 * Lays out Number into digits appended to OutDigits: the value rounded (at least 1, at most MaxDigits nines), most
	 * significant digit first, placed left to right along DigitDirection, DigitAdvance x Size apart, centred on Location.
	 */
	void AppendDigits(const FRRDamageNumber& Number, TArray<FRRDamageDigit>& OutDigits) const;

	/** Digits of the last pushed batch (empty before the first push). */
	const TArray<FRRDamageDigit>& GetLastDigits() const { return LastDigits; }

	/** Id of the last pushed batch (0 before the first push, +1 per push, never reset). */
	int32 GetDigitBatchId() const { return DigitBatchId; }

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
		/** Offset along StackDirection set when the entry opened over another open number (crowd stacking), kept for its later hits. */
		FVector Stack = FVector::ZeroVector;
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

	/** Screen footprint of an entry (width along DigitDirection, height along StackDirection), from its current value and style. */
	FVector2D GetFootprint(float Value, bool bCritical) const;

	/** Stack offset for a new entry at Location: lifted along StackDirection above each open entry whose footprint it overlaps (MaxStackSteps at most). */
	FVector ComputeStackOffset(const FVector& Location, float Value, bool bCritical) const;

	/** Sends LastDigits and DigitBatchId to the Niagara component, when there is one (parameters the asset lacks are skipped). */
	void PushToNiagara();

	/** Fills MissingParameters with the digit user parameters System does not expose; logs one warning listing them. */
	void CheckUserParameters(const UNiagaraSystem& System);

	/** True when the Niagara system exposes the user parameter (always true before CheckUserParameters ran). */
	bool HasParameter(FName Parameter) const { return !MissingParameters.Contains(Parameter); }

	/** Niagara system rendering the digits (digit atlas, CPU sim, one emitter spawning DigitCount particles per batch id). */
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

	/** Color of a critical number (damage-numbers: gold, linear (1, 0.55, 0.08), owner-confirmed after handover-2026-10-06). */
	UPROPERTY(Config)
	FLinearColor CriticalColor = FLinearColor(1.f, 0.55f, 0.08f);

	/** Distance between two digit centres, as a fraction of the glyph size (matches the atlas glyph width). */
	UPROPERTY(Config)
	float DigitAdvance = 0.62f;

	/** World direction of reading order (screen right): +Y for the top-down camera at yaw 0. Normalized on use. */
	UPROPERTY(Config)
	FVector DigitDirection = FVector(0.f, 1.f, 0.f);

	/** Most digits per number; larger sums show that many nines. */
	UPROPERTY(Config)
	int32 MaxDigits = 5;

	/** Random horizontal offset (uu, each axis in [-Jitter, Jitter]) drawn per entry so neighbouring numbers stay readable. */
	UPROPERTY(Config)
	float Jitter = 25.f;

	/** World direction of screen up for the stacking (camera up of the top-down camera at pitch -50, yaw 0). Normalized on use. */
	UPROPERTY(Config)
	FVector StackDirection = FVector(0.766f, 0.f, 0.643f);

	/** Gap (uu) left between a stacked number and the one it was stacked above. */
	UPROPERTY(Config)
	float StackGap = 4.f;

	/** Most stacking moves for one new number (each lifts it above one overlapping open number); 0 = no stacking. */
	UPROPERTY(Config)
	int32 MaxStackSteps = 4;

	/** At most this many numbers pushed per tick; the oldest go first, the rest wait (keeps horde frames readable and bounded). */
	UPROPERTY(Config)
	int32 MaxNumbersPerTick = 64;

	/** The persistent component (world-owned, not attached), created at world BeginPlay. */
	UPROPERTY(Transient)
	TObjectPtr<UNiagaraComponent> NiagaraComponent;

	/** Open entries, oldest first. */
	TArray<FPendingNumber> Pending;

	/** Digits of the last pushed batch, also the content of the Niagara digit arrays. */
	TArray<FRRDamageDigit> LastDigits;

	/** Id of the last pushed batch (DigitBatchIdParameter). */
	int32 DigitBatchId = 0;

	/** Digit user parameters the Niagara system does not expose (set at world BeginPlay); never written, so no warning per push. */
	TSet<FName> MissingParameters;

	/** Running total of pushed numbers. */
	int32 TotalPushedCount = 0;
};
