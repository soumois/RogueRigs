// Rogue Rigs - damage number world subsystem: every hit number of the world, one per hit from a random point of a spawn disc, pushed in batches to one Niagara system.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "RRDamageNumberSubsystem.generated.h"

class AActor;
class UNiagaraComponent;
class UNiagaraSystem;

/** One damage number ready to show: hit value, style and world position. */
USTRUCT(BlueprintType)
struct ROGUERIGS_API FRRDamageNumber
{
	GENERATED_BODY()

	/** World position of the number (hit location + its random spawn-disc offset + its stack offset). */
	UPROPERTY(BlueprintReadOnly, Category = "Damage Numbers")
	FVector Location = FVector::ZeroVector;

	/** Damage of the hit (raw damage, before the health clamp). */
	UPROPERTY(BlueprintReadOnly, Category = "Damage Numbers")
	float Value = 0.f;

	/** True when the hit was critical (never for a heal number). */
	UPROPERTY(BlueprintReadOnly, Category = "Damage Numbers")
	bool bCritical = false;

	/** True for a heal number (URRDamageNumberSubsystem::AddHeal): "+" glyph before the digits, HealColor. */
	UPROPERTY(BlueprintReadOnly, Category = "Damage Numbers")
	bool bHeal = false;

	/** True for an ore number (URRDamageNumberSubsystem::AddOre): OreGlyph icon (white) then "+" before the digits, OreColor. */
	UPROPERTY(BlueprintReadOnly, Category = "Damage Numbers")
	bool bOre = false;

	/** World size of the glyphs (NumberSize x the damage factor SizeMin..SizeMax, x CriticalScale on a crit; URRDamageNumberSubsystem::GetHitSize). */
	UPROPERTY(BlueprintReadOnly, Category = "Damage Numbers")
	float Size = 0.f;

	/** Color of the number (NormalColor, CriticalColor, HealColor or OreColor; the ore icon glyph is always drawn white). */
	UPROPERTY(BlueprintReadOnly, Category = "Damage Numbers")
	FLinearColor Color = FLinearColor::White;

	/** World time (seconds, pauses with the game) at which the number was pushed. */
	UPROPERTY(BlueprintReadOnly, Category = "Damage Numbers")
	float SpawnTime = 0.f;

	/** World rise velocity (uu/s) of the number, drawn once per number at the push (URRDamageNumberSubsystem::DrawRiseVelocity). */
	UPROPERTY(BlueprintReadOnly, Category = "Damage Numbers")
	FVector Velocity = FVector::ZeroVector;
};

/** One digit sprite of a damage number, laid out by URRDamageNumberSubsystem: what the Niagara emitter draws per particle. */
USTRUCT(BlueprintType)
struct ROGUERIGS_API FRRDamageDigit
{
	GENERATED_BODY()

	/** World position of the digit centre (number centre + its offset along DigitDirection). */
	UPROPERTY(BlueprintReadOnly, Category = "Damage Numbers")
	FVector Location = FVector::ZeroVector;

	/** Atlas cell of the glyph: digit 0-9, URRDamageNumberSubsystem::PlusGlyph (10, "+") before the digits of a heal or ore number, or OreGlyph (11, Duskite icon) first in an ore number. */
	UPROPERTY(BlueprintReadOnly, Category = "Damage Numbers")
	int32 Digit = 0;

	/** Glyph world size (the number's size). */
	UPROPERTY(BlueprintReadOnly, Category = "Damage Numbers")
	float Size = 0.f;

	/** Color (the number's color; white for the ore icon glyph so it keeps its texture colours). */
	UPROPERTY(BlueprintReadOnly, Category = "Damage Numbers")
	FLinearColor Color = FLinearColor::White;

	/** True when the number is critical (drives the pop). */
	UPROPERTY(BlueprintReadOnly, Category = "Damage Numbers")
	bool bCritical = false;

	/** World time of the push (the number's spawn time). */
	UPROPERTY(BlueprintReadOnly, Category = "Damage Numbers")
	float SpawnTime = 0.f;

	/** World rise velocity (uu/s): the number's velocity, shared by all its digits so the number stays readable. */
	UPROPERTY(BlueprintReadOnly, Category = "Damage Numbers")
	FVector Velocity = FVector::ZeroVector;
};

/** Native event of URRDamageNumberSubsystem: the batch of numbers pushed this tick (never broadcast empty). */
DECLARE_MULTICAST_DELEGATE_OneParam(FRROnDamageNumbersPushed, const TArray<FRRDamageNumber>& /*Numbers*/);

/**
 * The single path of floating damage numbers (damage-numbers, sys-damage-numbers). Damage sources call AddHit on each hit
 * (ARREnemy). Every hit is its own number (no merging, task-059-01): it starts at a random point of a horizontal disc
 * around the hit (radius SpawnRadius, or the caller's radius, e.g. ARREnemy::DamageNumberSpawnRadius), so repeated hits on
 * one robot do not start from the same spot, and it stays where it was hit even if the robot dies or is destroyed. A new
 * number whose screen footprint would overlap another pending number, or a number still on screen (pushed less than
 * DisplayLifetime ago, risen by RiseVelocity), is stacked above it along StackDirection (crowd stacking, damage-numbers).
 * Nothing is spawned per hit: on each tick the pending numbers (at most
 * MaxNumbersPerTick, the rest wait one tick) are laid out into digits here (left to right along DigitDirection, centred
 * on the number position, advance DigitAdvance x size) and pushed to one persistent UNiagaraComponent as one entry per
 * digit in Niagara array user parameters, with the digit count and a batch id incremented on every push (never reset),
 * so the emitter spawns DigitCount particles once per batch id change and particle N reads entry N. The Niagara system is NumberSystem (config, DefaultGame.ini section
 * [/Script/RogueRigs.RRDamageNumberSubsystem]); unset or missing = no rendering, the numbers and OnNumbersPushed still run.
 * Heal numbers (task-324-02): AddHeal records a green "+N" through the same path (size by value, never critical, same
 * stacking with the "+" counted in the footprint); ARRPickup calls it when a heal pickup heals the miner.
 * Ore numbers (task-430-02): AddOre records "<Duskite icon> +N" the same way (icon glyph OreGlyph in white, then "+N" in
 * OreColor, icon cell counted in the footprint); ARRPickup calls it when the miner collects an Ore pickup.
 * The player toggle is URRGameUserSettings::bShowDamageNumbers: off = hits, heals and ore numbers ignored and pending numbers dropped.
 * Ticks with the world (stops while the game is paused, like the Niagara component).
 */
UCLASS(Config = Game)
class ROGUERIGS_API URRDamageNumberSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	/** Niagara user parameter (Niagara Position Array): world position of each digit of the batch. */
	static const FName DigitPositionsParameter;
	/** Niagara user parameter (Niagara Int32 Array): atlas cell of each glyph (0-9 digits, PlusGlyph 10 = "+", OreGlyph 11 = Duskite icon). */
	static const FName DigitIndicesParameter;
	/** Atlas cell of the "+" glyph of T_DamageDigitsOutline (task-324-01), drawn before the digits of a heal number. */
	static constexpr int32 PlusGlyph = 10;
	/** Atlas cell of the Duskite icon glyph of T_DamageDigitsOutline (task-430-01), drawn first (white) in an ore number. */
	static constexpr int32 OreGlyph = 11;
	/** Niagara user parameter (Niagara Float Array): glyph world size of each digit (its number's GetHitSize). */
	static const FName DigitSizesParameter;
	/** Niagara user parameter (Niagara Color Array): color of each digit. */
	static const FName DigitColorsParameter;
	/** Niagara user parameter (Niagara Int32 Array): 1 when the digit's number is critical, else 0 (drives the pop). */
	static const FName DigitCriticalsParameter;
	/** Niagara user parameter (Niagara Float Array): world time of the push, same for the whole batch. */
	static const FName DigitSpawnTimesParameter;
	/** Niagara user parameter (Niagara Vector Array): world rise velocity (uu/s) of each digit, one draw per number (DrawRiseVelocity). */
	static const FName DigitVelocitiesParameter;
	/** Niagara user parameter (int32): number of digits of the last batch (array length); kept until the next batch. */
	static const FName DigitCountParameter;
	/** Niagara user parameter (int32): id of the last batch, +1 per push, never reset (0 = nothing pushed yet). */
	static const FName DigitBatchIdParameter;

	/** Default values of the config properties (NumberSystem path included). */
	URRDamageNumberSubsystem();

	/**
	 * Records a hit of Value on Target (may be null; hits never merge) shown at Location (world, e.g. above the capsule): one
	 * new number, pushed on the next tick, starting at a random point of a horizontal disc of radius InSpawnRadius around
	 * Location (negative = the config SpawnRadius). Ignored when Value <= 0 or the player turned damage numbers off. Safe
	 * inside a GE execution (no spawn, no broadcast).
	 */
	UFUNCTION(BlueprintCallable, Category = "Damage Numbers")
	void AddHit(AActor* Target, float Value, bool bCritical, FVector Location, float InSpawnRadius = -1.f);

	/**
	 * Records a heal of Amount shown at Location (world, e.g. miner capsule top + 40 uu): one green "+N" number (HealColor,
	 * size by value like a hit, never critical), same spawn disc (InSpawnRadius, negative = SpawnRadius), stacking and push as
	 * AddHit. Ignored when Amount <= 0 or the player turned damage numbers off (damage-numbers, heal numbers line).
	 */
	UFUNCTION(BlueprintCallable, Category = "Damage Numbers")
	void AddHeal(FVector Location, float Amount, float InSpawnRadius = -1.f);

	/**
	 * Records an ore gain of Amount shown at Location (world, e.g. miner capsule top + 40 uu): one "<icon> +N" number, the
	 * OreGlyph icon drawn white (own texture colours) then "+N" in OreColor, size by value like a hit, never critical, same
	 * spawn disc (InSpawnRadius, negative = SpawnRadius), stacking and push as AddHit. Ignored when Amount <= 0 or the player
	 * turned damage numbers off (damage-numbers, ore numbers line).
	 */
	UFUNCTION(BlueprintCallable, Category = "Damage Numbers")
	void AddOre(FVector Location, float Amount, float InSpawnRadius = -1.f);

	/** True when the player setting shows damage numbers (true when the game user settings are not URRGameUserSettings). */
	UFUNCTION(BlueprintPure, Category = "Damage Numbers")
	bool AreNumbersShown() const;

	/** Number of hits recorded and not pushed yet (pushed on the next tick, MaxNumbersPerTick at most per tick). */
	UFUNCTION(BlueprintPure, Category = "Damage Numbers")
	int32 GetPendingCount() const { return Pending.Num(); }

	/** Default radius (uu) of the horizontal disc a number starts in around its hit (callers without their own radius). */
	UFUNCTION(BlueprintPure, Category = "Damage Numbers")
	float GetSpawnRadius() const { return SpawnRadius; }

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
	 * A heal number gets a PlusGlyph glyph before its digits; an ore number gets an OreGlyph icon (white, OreIconAdvance x Size
	 * before the "+") then a PlusGlyph glyph; the whole is centred (midpoint of the first and last glyph centres).
	 */
	void AppendDigits(const FRRDamageNumber& Number, TArray<FRRDamageDigit>& OutDigits) const;

	/**
	 * Glyph world size of a number of Value (damage-numbers, provisional, owner 2026-10-19): NumberSize x
	 * lerp(SizeMin, SizeMax, clamp(Value / DamageForMaxSize, 0, 1)), x CriticalScale when bCritical. Used for the pushed
	 * size and the stacking footprint.
	 */
	float GetHitSize(float Value, bool bCritical) const;

	/**
	 * Random rise velocity of one number (damage-numbers, provisional, owner 2026-10-19): RiseVelocity rotated in the screen
	 * plane (around the view axis DigitDirection x StackDirection) by a uniform angle in [-RiseAngleMax, RiseAngleMax] degrees,
	 * scaled by a uniform factor in [RiseSpeedMin, RiseSpeedMax]. Drawn once per number at the push; all its digits share it.
	 */
	FVector DrawRiseVelocity() const;

	/** Seconds a pushed number stays on screen (particle lifetime of NumberSystem when it could be read, else the config value). */
	float GetDisplayLifetime() const { return DisplayLifetime; }

	/** World velocity (uu/s) of a displayed number (particle velocity of NumberSystem when it could be read, else the config value). */
	FVector GetRiseVelocity() const { return RiseVelocity; }

	/** True when DisplayLifetime was read from the NumberSystem asset at world BeginPlay. */
	bool IsDisplayReadFromAsset() const { return bDisplayReadFromAsset; }

	/** Numbers pushed and still on screen (pushed less than DisplayLifetime ago); pruned on each tick. */
	int32 GetDisplayedCount() const { return Displayed.Num(); }

	/**
	 * Reads the particle lifetime (the direct InitializeParticle Lifetime constant of an emitter spawn script; Min / Max are ignored) and the particle
	 * velocity (a non-zero Particles.Velocity constant of an emitter spawn script) from the rapid iteration parameters of System.
	 * False when no lifetime is found; OutVelocity is left unchanged when no non-zero velocity constant exists.
	 */
	static bool ReadDisplayFromSystem(const UNiagaraSystem& System, float& OutLifetime, FVector& OutVelocity);

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
	/** One hit waiting for the next push. */
	struct FPendingNumber
	{
		/** Hit location (without offsets). */
		FVector HitLocation = FVector::ZeroVector;
		/** Random horizontal offset inside the spawn disc, drawn at the hit. */
		FVector SpawnOffset = FVector::ZeroVector;
		/** Offset along StackDirection set at the hit when it would overlap another number (crowd stacking). */
		FVector Stack = FVector::ZeroVector;
		/** Damage of the hit. */
		float Value = 0.f;
		/** Critical hit. */
		bool bCritical = false;
		/** Heal number ("+" glyph, HealColor). */
		bool bHeal = false;
		/** Ore number (OreGlyph icon and "+" glyph, OreColor). */
		bool bOre = false;
	};

	/** One pushed number still on screen: avoided by the stacking until it expires. */
	struct FDisplayedNumber
	{
		/** World position at the push (the number's centre). */
		FVector Location = FVector::ZeroVector;
		/** Screen footprint (width along DigitDirection, height along StackDirection). */
		FVector2D Footprint = FVector2D::ZeroVector;
		/** World time of the push. */
		double SpawnTime = 0.0;
	};

	/** Game worlds only (game, PIE): no numbers in editor worlds. */
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

	/** Loads NumberSystem and creates the persistent Niagara component (null-safe). */
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	/** Drops pending entries and the component. */
	virtual void Deinitialize() override;

	/** Records one pending number (AddHit, AddHeal, AddOre): spawn disc point, stack offset, style. Callers check World, Value and the toggle. */
	void AddPendingNumber(float Value, bool bCritical, bool bHeal, const FVector& Location, float InSpawnRadius, double Now, bool bOre = false);

	/** Screen footprint of a number (width along DigitDirection, height along StackDirection), from its value and style; a heal number counts its "+" glyph, an ore number its icon and "+". */
	FVector2D GetFootprint(float Value, bool bCritical, bool bHeal = false, bool bOre = false) const;

	/**
	 * Stack offset for a new number at Location recorded at Now: lifted along StackDirection above each pending number or
	 * displayed number (risen by RiseVelocity since its push) whose footprint it would overlap, MaxStackSteps lifts at most.
	 */
	FVector ComputeStackOffset(const FVector& Location, float Value, bool bCritical, bool bHeal, double Now, bool bOre = false) const;

	/** Sends LastDigits and DigitBatchId to the Niagara component, when there is one (parameters the asset lacks are skipped). */
	void PushToNiagara();

	/** Fills MissingParameters with the digit user parameters System does not expose; logs one warning listing them. */
	void CheckUserParameters(const UNiagaraSystem& System);

	/** True when the Niagara system exposes the user parameter (always true before CheckUserParameters ran). */
	bool HasParameter(FName Parameter) const { return !MissingParameters.Contains(Parameter); }

	/** Niagara system rendering the digits (digit atlas, CPU sim, one emitter spawning DigitCount particles per batch id). */
	UPROPERTY(Config)
	TSoftObjectPtr<UNiagaraSystem> NumberSystem;

	/** Reference glyph world size (uu), scaled by the damage factor SizeMin..SizeMax (GetHitSize). */
	UPROPERTY(Config)
	float NumberSize = 60.f;

	/** Size factor of the smallest hit (damage 0 and up; damage-numbers: 0.8, provisional, owner 2026-10-19). */
	UPROPERTY(Config)
	float SizeMin = 0.8f;

	/** Size factor reached at DamageForMaxSize and above (damage-numbers: 1.4, provisional, owner 2026-10-19). */
	UPROPERTY(Config)
	float SizeMax = 1.4f;

	/**
	 * Hit damage at which the size factor reaches SizeMax (damage-numbers: twice the base Damage of the starting weapon,
	 * BurstRifle 10 in WeaponStats.csv, so 20; provisional, owner 2026-10-19). 0 or less = every hit at SizeMax.
	 */
	UPROPERTY(Config)
	float DamageForMaxSize = 20.f;

	/** Size multiplier of a critical number, on top of the damage factor (damage-numbers: 1.6). */
	UPROPERTY(Config)
	float CriticalScale = 1.6f;

	/** Color of a normal number (white). */
	UPROPERTY(Config)
	FLinearColor NormalColor = FLinearColor::White;

	/** Color of a critical number (damage-numbers: lighter orange, linear (1, 0.7, 0.35), provisional, owner 2026-10-19; was gold (1, 0.55, 0.08)). */
	UPROPERTY(Config)
	FLinearColor CriticalColor = FLinearColor(1.f, 0.7f, 0.35f);

	/** Color of a heal number "+N" (damage-numbers and color-code: the heal ring green, linear (0.1, 1, 0.2), provisional, owner 2026-10-27). */
	UPROPERTY(Config)
	FLinearColor HealColor = FLinearColor(0.1f, 1.f, 0.2f);

	/** Color of the "+N" of an ore number (damage-numbers and color-code: white, the icon carries the Duskite violet; provisional, owner 2026-11-04). */
	UPROPERTY(Config)
	FLinearColor OreColor = FLinearColor::White;

	/**
	 * Distance between the ore icon centre and the "+" centre, as a fraction of the glyph size: wider than DigitAdvance so the
	 * icon (about 0.73 x Size wide, task-430-01) does not overlap the "+" (provisional, task-430-02).
	 */
	UPROPERTY(Config)
	float OreIconAdvance = 0.8f;

	/** Distance between two digit centres, as a fraction of the glyph size (matches the atlas glyph width). */
	UPROPERTY(Config)
	float DigitAdvance = 0.62f;

	/** World direction of reading order (screen right): +Y for the top-down camera at yaw 0. Normalized on use. */
	UPROPERTY(Config)
	FVector DigitDirection = FVector(0.f, 1.f, 0.f);

	/** Most digits per number; larger sums show that many nines. */
	UPROPERTY(Config)
	int32 MaxDigits = 5;

	/** Default radius (uu) of the horizontal disc around the hit where a number starts, uniform random point (damage-numbers: 30). */
	UPROPERTY(Config)
	float SpawnRadius = 30.f;

	/** World direction of screen up for the stacking (camera up of the top-down camera at pitch -50, yaw 0). Normalized on use. */
	UPROPERTY(Config)
	FVector StackDirection = FVector(0.766f, 0.f, 0.643f);

	/** Seconds a pushed number stays on screen (NS_DamageNumbers particle lifetime 0.7); replaced by the asset value at BeginPlay when readable. */
	UPROPERTY(Config)
	float DisplayLifetime = 0.7f;

	/**
	 * Mean world velocity (uu/s) of a displayed number (NS_DamageNumbers particle velocity); replaced by the asset value at
	 * BeginPlay when readable. Each number varies it (DrawRiseVelocity); the stacking keeps this mean as approximation.
	 */
	UPROPERTY(Config)
	FVector RiseVelocity = FVector(0.f, 0.f, 85.7f);

	/** Smallest rise speed factor of a number, applied to RiseVelocity (damage-numbers: 0.7, provisional, owner 2026-10-19). */
	UPROPERTY(Config)
	float RiseSpeedMin = 0.7f;

	/** Largest rise speed factor of a number, applied to RiseVelocity (damage-numbers: 1.3, provisional, owner 2026-10-19). */
	UPROPERTY(Config)
	float RiseSpeedMax = 1.3f;

	/**
	 * Largest rise angle (degrees) of a number, left or right of screen up (StackDirection), in the screen plane
	 * (damage-numbers: 35, provisional, owner 2026-10-19). 0 = every number rises straight up.
	 */
	UPROPERTY(Config)
	float RiseAngleMax = 35.f;

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

	/** True when DisplayLifetime comes from the asset. */
	bool bDisplayReadFromAsset = false;

	/** Pushed numbers still on screen, oldest first (the stacking avoids them). */
	TArray<FDisplayedNumber> Displayed;

	/** Hits not pushed yet, oldest first. */
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
