// Rogue Rigs - extraction shuttle: lands far from the miner, counts down, ends the run with a win (miner on board) or a loss (countdown over).

#pragma once

#include "Audio/RRDistanceSound.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RRShuttle.generated.h"

class ARRArenaGrid;
class ARRMiner;
class UBoxComponent;
class UPrimitiveComponent;
class USceneComponent;
class UStaticMeshComponent;
class ARRShuttle;
struct FHitResult;

/** Fired every ticked frame of the countdown with the seconds left (also once at landing). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRROnShuttleCountdownChangedSignature, ARRShuttle*, Shuttle, float, RemainingSeconds);

/** Fired once when the takeoff is over and the shuttle is about to be removed (it left the player camera view, or the safety time ran out). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRROnShuttleTakeoffFinishedSignature, ARRShuttle*, Shuttle);

/** Fired once when the shuttle is done: bExtracted true = the miner boarded, false = the countdown ran out. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRROnShuttleFinishedSignature, ARRShuttle*, Shuttle, bool, bExtracted);

/**
 * Extraction shuttle (extraction gdd). Spawned by URRRunFlowSubsystem on a reachable floor cell once extraction is ready, then
 * InitializeShuttle places it on the cell center and starts the countdown. The miner boards (OnExtracted, OnShuttleFinished true)
 * when its capsule overlaps the BoardZone box (begin-overlap event, plus one check at InitializeShuttle for a miner already inside;
 * task-377), as long as it is alive. Each tick (not while paused) the countdown drops by DeltaSeconds and at 0 the shuttle leaves
 * without the miner (OnMissed, OnShuttleFinished false). A dead miner freezes the countdown (the game over owns the end). Finished
 * is final. The run flow turns OnShuttleFinished into the run end.
 * Not abstract, so headless tests run without the Blueprint; subclass in Blueprint for the mesh and the landing effects.
 */
UCLASS(Blueprintable)
class ROGUERIGS_API ARRShuttle : public AActor
{
	GENERATED_BODY()

public:
	/** Creates the root, the board zone box, the body mesh and the four rotor meshes (no collision) and enables ticking. */
	ARRShuttle();

	/**
	 * Places the shuttle on Cell of Grid (centre of its FootprintCells around Cell, floor top), remembers Miner as the one who must board and starts a
	 * countdown of CountdownSeconds. Fires OnLanded and the first countdown events, then boards the miner at once when its capsule already overlaps
	 * BoardZone. Call once, right after spawning.
	 */
	void InitializeShuttle(ARRArenaGrid* Grid, ARRMiner* Miner, FIntPoint Cell, float CountdownSeconds);

	/**
	 * Arrival mode (task-371-01): places the extractor that dropped the miner at the level start on Cell of Grid (centre of its
	 * FootprintCells around Cell, floor top), already landed. No board (BoardZone collision off, task-377 Q5), no countdown, no
	 * OnLanded and no countdown event: it only waits for StartTakeoff (called by URRRunFlowSubsystem when the fade in ends, or at
	 * once without fade). Call once, right after spawning, instead of InitializeShuttle.
	 */
	void InitializeArrival(ARRArenaGrid* Grid, FIntPoint Cell);

	/** True for the arrival extractor of the level start (InitializeArrival): not an exit, no board, no countdown. */
	UFUNCTION(BlueprintPure, Category = "Shuttle")
	bool IsArrival() const { return bArrival; }

	/** Rotor spin, then the takeoff or the countdown (the boarding comes from the BoardZone overlap). */
	virtual void Tick(float DeltaSeconds) override;

	/** Seconds left before the shuttle leaves. */
	UFUNCTION(BlueprintPure, Category = "Shuttle")
	float GetRemainingSeconds() const { return RemainingSeconds; }

	/** Countdown length given at landing. */
	UFUNCTION(BlueprintPure, Category = "Shuttle")
	float GetCountdownSeconds() const { return CountdownSeconds; }

	/** Box whose overlap by the miner capsule ends the level with a win (task-377); its size, position and rotation are set in the Blueprint subclass. */
	UFUNCTION(BlueprintPure, Category = "Shuttle")
	UBoxComponent* GetBoardZone() const { return BoardZone; }

	/** Grid cell the shuttle landed on (anchor of its footprint). */
	UFUNCTION(BlueprintPure, Category = "Shuttle")
	FIntPoint GetCell() const { return Cell; }

	/** Landing footprint in cells (X along the grid X axis, Y along Y); every footprint cell must be open reachable floor. */
	UFUNCTION(BlueprintPure, Category = "Shuttle")
	FIntPoint GetFootprintCells() const { return FootprintCells; }

	/** Margin in cells cleared of blocks around the footprint when the extractor is set down (task-376). */
	UFUNCTION(BlueprintPure, Category = "Shuttle")
	int32 GetClearMarginCells() const { return ClearMarginCells; }

	/** True once the miner boarded or the countdown ran out. */
	UFUNCTION(BlueprintPure, Category = "Shuttle")
	bool IsFinished() const { return bFinished; }

	/** True when the miner boarded (valid once finished). */
	UFUNCTION(BlueprintPure, Category = "Shuttle")
	bool WasExtracted() const { return bExtracted; }

	/** The four rotor mesh components in the order front-left, front-right, rear-left, rear-right (for the rotor spin and the rotor-wash effects). */
	UFUNCTION(BlueprintPure, Category = "Shuttle")
	TArray<UStaticMeshComponent*> GetRotors() const;

	/**
	 * Rotor speed as a fraction of the flight speed: current rev/s divided by FlightRevPerSecond (0.6 on the ground with the defaults,
	 * 1 in flight, in between during the ramp; 0 when FlightRevPerSecond is 0). For the rotor-wash intensity.
	 */
	UFUNCTION(BlueprintPure, Category = "Shuttle|Rotors")
	float GetRotorSpeedAlpha() const;

	/** Current rotor speed in revolutions per second (GroundRevPerSecond on the ground, FlightRevPerSecond once taking off, ramped between). */
	UFUNCTION(BlueprintPure, Category = "Shuttle|Rotors")
	float GetRotorRevPerSecond() const { return RotorRevPerSecond; }

	/**
	 * Starts the helicopter takeoff (task-370-01): SpinUpSeconds of waiting (the rotors ramp to FlightRevPerSecond, task-373-01), then a vertical rise of RiseHeight over RiseSeconds,
	 * a tilt to PitchDegrees nose down over TiltSeconds, then an acceleration of ForwardAcceleration along the horizontal heading of the nose, climbing ClimbRatio
	 * of that speed, never descending (task-528), until the bounds leave the player camera view;
	 * the shuttle is then destroyed. Ticks while the game is paused. Fires OnTakeoffStarted. Does nothing when already taking off. Called by the run flow on a
	 * successful extraction; callable from C++ for the arrival (task-371).
	 */
	UFUNCTION(BlueprintCallable, Category = "Shuttle|Takeoff")
	void StartTakeoff();

	/** True from StartTakeoff until the removal. */
	UFUNCTION(BlueprintPure, Category = "Shuttle|Takeoff")
	bool IsTakingOff() const { return bTakingOff; }

	/** Seconds since StartTakeoff. */
	UFUNCTION(BlueprintPure, Category = "Shuttle|Takeoff")
	float GetTakeoffSeconds() const { return TakeoffElapsed; }

	/**
	 * Height of the extractor base above the floor (uu), for the rotor blast (task-443): 0 when landed, RiseHeight at the end of the
	 * rise. Holds its highest value during the forward flight: the nose-down flight loses altitude and must not bring the blast back.
	 */
	UFUNCTION(BlueprintPure, Category = "Shuttle|Takeoff")
	float GetHeightAboveFloor() const { return HeightAboveFloor; }

	/** Broadcast once at the end of the takeoff, just before the removal. */
	UPROPERTY(BlueprintAssignable, Category = "Shuttle|Takeoff")
	FRROnShuttleTakeoffFinishedSignature OnShuttleTakeoffFinished;

	/** Broadcast every countdown frame and once at landing. */
	UPROPERTY(BlueprintAssignable, Category = "Shuttle")
	FRROnShuttleCountdownChangedSignature OnShuttleCountdownChanged;

	/** Broadcast once when the miner boards or the countdown runs out. */
	UPROPERTY(BlueprintAssignable, Category = "Shuttle")
	FRROnShuttleFinishedSignature OnShuttleFinished;

protected:
	/** Display event: the shuttle touched down (landing effects). Implement in Blueprint. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Shuttle")
	void OnLanded();

	/** Display event: the countdown changed (every ticked frame, and at landing). Implement in Blueprint (gauge, beacon). */
	UFUNCTION(BlueprintImplementableEvent, Category = "Shuttle")
	void OnCountdown(float Remaining);

	/** Display event: the miner boarded (run won). Implement in Blueprint. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Shuttle")
	void OnExtracted();

	/** Display event: the countdown ran out, the shuttle leaves without the miner (run lost). Implement in Blueprint. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Shuttle")
	void OnMissed();

	/** Display event: the takeoff began (rotor spin-up, rotor wash). Implement in Blueprint. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Shuttle|Takeoff")
	void OnTakeoffStarted();

	/** Display event: the takeoff is over, the shuttle is removed right after. Implement in Blueprint. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Shuttle|Takeoff")
	void OnTakeoffFinished();

	/** Display event: the rotor speed changed during a ramp (every ramp frame, the last one included) and once at BeginPlay; Alpha = GetRotorSpeedAlpha(). Implement in Blueprint (rotor wash). */
	UFUNCTION(BlueprintImplementableEvent, Category = "Shuttle|Rotors")
	void OnRotorSpeedChanged(float Alpha);

	/** Display event: GetHeightAboveFloor() changed (each frame of the rise, also while paused) and once at BeginPlay with 0. Implement in Blueprint (rotor blast). */
	UFUNCTION(BlueprintImplementableEvent, Category = "Shuttle|Takeoff")
	void OnHeightAboveFloorChanged(float Height);

	/** Sets the starting rotor speed (GroundRevPerSecond), fires the first OnRotorSpeedChanged and the first OnHeightAboveFloorChanged. */
	virtual void BeginPlay() override;

	/** Releases the rotor sound (fade out over its row's LoopFadeOut). */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Rotor sound step, after TickRotors: one loop at the actor location while the rotors spin, level by the player distance (task-522). */
	void TickRotorSound();

	/** Rotor step, every tick (also while paused once taking off): ramps the speed toward the ground or flight target, then turns each rotor about its local Z. */
	void TickRotors(float DeltaSeconds);

	/** Takeoff step: advances the phases, then removes the shuttle once out of view. */
	void TickTakeoff(float DeltaSeconds);

	/** True when no corner of the actor bounds projects inside the player 0 viewport (false when there is no viewport to ask). */
	bool IsOutOfPlayerView() const;

	/** Ends the takeoff: OnTakeoffFinished, OnShuttleTakeoffFinished, then Destroy. */
	void FinishTakeoff();

	/** Ends the shuttle once: fires OnExtracted or OnMissed, then OnShuttleFinished. */
	void Finish(bool bInExtracted);

	/** Fires OnCountdown and OnShuttleCountdownChanged with the current value. */
	void BroadcastCountdown();

	/** Binds the BoardZone begin-overlap event (component ready, before BeginPlay). */
	virtual void PostInitializeComponents() override;

	/** BoardZone begin overlap: boards the miner when OtherComp is the miner capsule (the drone and the robots do not count, task-377 Q3). */
	UFUNCTION()
	void HandleBoardZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	/** Ends the level with a win (Finish(true)) when the shuttle is initialized, not finished, not taking off and the miner is valid and alive; returns true when it did. */
	bool TryBoard();

	/** Root of the actor (the landing point, floor top). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shuttle")
	TObjectPtr<USceneComponent> ShuttleRoot;

	/**
	 * Level-ending zone (task-377): the miner capsule overlapping it boards the miner. Attached to ShuttleRoot (not to Mesh, so the
	 * mesh scale does not multiply it). Default 1608 x 1608 x 400 uu (extent 804, 804, 200), centred on the actor, bottom on the
	 * floor (relative Z 200); owner 2026-10-30 (Q2), provisional, resized and moved in BP_ExtractionShuttle. Query only, overlaps the
	 * Pawn channel only, overlap events on, hidden in game, no navigation impact. Collision off on the arrival extractor and from the
	 * finish or the takeoff on.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shuttle")
	TObjectPtr<UBoxComponent> BoardZone;

	/** Shuttle body mesh (the extractor hull), set in the Blueprint subclass; parent of the four rotors. No collision by default. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shuttle")
	TObjectPtr<UStaticMeshComponent> Mesh;

	/** Front-left rotor, attached to the body; mesh and relative location (rotor pivot in the body frame) set in the Blueprint subclass. No collision. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shuttle")
	TObjectPtr<UStaticMeshComponent> RotorFL;

	/** Front-right rotor, attached to the body; mesh and relative location set in the Blueprint subclass. No collision. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shuttle")
	TObjectPtr<UStaticMeshComponent> RotorFR;

	/** Rear-left rotor, attached to the body; mesh and relative location set in the Blueprint subclass. No collision. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shuttle")
	TObjectPtr<UStaticMeshComponent> RotorRL;

	/** Rear-right rotor, attached to the body; mesh and relative location set in the Blueprint subclass. No collision. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shuttle")
	TObjectPtr<UStaticMeshComponent> RotorRR;

	/** Creates one rotor mesh component named Name, attached to the body mesh, with no collision, no overlap events and no navigation impact. */
	UStaticMeshComponent* CreateRotor(FName Name);

	/** Grid the shuttle landed on. */
	UPROPERTY(Transient)
	TObjectPtr<ARRArenaGrid> Grid;

	/** Miner who must board. */
	UPROPERTY(Transient)
	TObjectPtr<ARRMiner> Miner;

	/** Landing cell. */
	FIntPoint Cell = FIntPoint(INDEX_NONE, INDEX_NONE);

	/**
	 * Landing footprint in cells of the grid, rotation zero (16 x 12 cells of 67 uu = 1072 x 804 uu: x4 of the former 4 x 3 for the
	 * mesh scaled x4, owner 2026-10-29, task-375). The shuttle lands only where every cell is reachable floor and stands at the
	 * footprint centre (RRLandingFootprint). Provisional.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shuttle", meta = (ClampMin = 1))
	FIntPoint FootprintCells = FIntPoint(16, 12);

	/**
	 * Margin, in cells, cleared of blocks around the footprint at arrival and at extraction landing (the clear rectangle is the footprint
	 * grown by this on every side, clamped to the grid; ARRArenaGrid::ClearExtractorFootprint). Owner 2026-10-29 (task-376, Q4), provisional.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shuttle", meta = (ClampMin = 0))
	int32 ClearMarginCells = 2;

	/** Takeoff: wait before the rise, rotors spinning up (s). Provisional (extraction takeoff, Q3). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shuttle|Takeoff", meta = (ClampMin = 0, Units = "s"))
	float SpinUpSeconds = 0.5f;

	/** Takeoff: vertical rise (uu). Provisional. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shuttle|Takeoff", meta = (ClampMin = 0, Units = "cm"))
	float RiseHeight = 400.f;

	/** Takeoff: duration of the vertical rise (s). Provisional. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shuttle|Takeoff", meta = (ClampMin = 0.01, Units = "s"))
	float RiseSeconds = 1.5f;

	/** Takeoff: nose-down tilt after the rise (degrees, positive = nose down). Provisional. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shuttle|Takeoff", meta = (ClampMin = 0, Units = "deg"))
	float PitchDegrees = 20.f;

	/** Takeoff: duration of the tilt (s); the forward acceleration starts with it. Provisional. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shuttle|Takeoff", meta = (ClampMin = 0.01, Units = "s"))
	float TiltSeconds = 0.4f;

	/** Takeoff: acceleration along the horizontal heading of the nose once tilting (uu/s2). Provisional. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shuttle|Takeoff", meta = (ClampMin = 0))
	float ForwardAcceleration = 1500.f;

	/** Takeoff: climb speed during the forward flight, as a fraction of the horizontal speed (0.1 = 1 uu up per 10 uu forward); never negative, so it never descends (task-528, owner 2026-11-13, provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shuttle|Takeoff", meta = (ClampMin = 0))
	float ClimbRatio = 0.1f;

	/** Takeoff: safety, the shuttle is removed after this many seconds even if still in view or with no viewport (headless). Provisional. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shuttle|Takeoff", meta = (ClampMin = 1, Units = "s"))
	float TakeoffMaxSeconds = 12.f;

	/** Rotors: speed on the ground, before StartTakeoff (rev/s). Owner 2026-10-29 (extraction Q6), provisional. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shuttle|Rotors", meta = (ClampMin = 0))
	float GroundRevPerSecond = 3.f;

	/** Rotors: speed in flight, from StartTakeoff (rev/s). Owner 2026-10-29 (extraction Q6), provisional. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shuttle|Rotors", meta = (ClampMin = 0))
	float FlightRevPerSecond = 5.f;

	/** Rotors: time of the linear ramp between the ground and the flight speed (s); 0 = instant. Owner 2026-10-29 (extraction Q6), provisional. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shuttle|Rotors", meta = (ClampMin = 0, Units = "s"))
	float RampSeconds = 0.5f;

	/** Rotors: front-left turns clockwise seen from above (positive yaw about its local Z) when true. Q6: clockwise. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shuttle|Rotors")
	bool bRotorFLClockwise = true;

	/** Rotors: front-right turns clockwise seen from above when true. Q6: counter-clockwise. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shuttle|Rotors")
	bool bRotorFRClockwise = false;

	/** Rotors: rear-left turns clockwise seen from above when true. Q6: counter-clockwise. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shuttle|Rotors")
	bool bRotorRLClockwise = false;

	/** Rotors: rear-right turns clockwise seen from above when true. Q6: clockwise. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shuttle|Rotors")
	bool bRotorRRClockwise = true;

	/**
	 * Rotor sound (task-522, audio-direction): event of the sound table played as one distance loop for the four rotors, at the actor
	 * location, while the rotors spin in the level (arrival until out of view; extraction from landing to the end of the takeoff, behind
	 * the end screen). Level and distances in its table row (ExtractorRotors). None = silent.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shuttle|Rotors")
	FName RotorSoundEvent = TEXT("ExtractorRotors");

	/** The rotor loop (FRRDistanceSound), built at BeginPlay from RotorSoundEvent. */
	FRRDistanceSound RotorSound{ NAME_None };

	/** Current rotor speed (rev/s). */
	float RotorRevPerSecond = 0.f;

	/** True from StartTakeoff until the removal. */
	bool bTakingOff = false;

	/** Seconds since StartTakeoff. */
	float TakeoffElapsed = 0.f;

	/** Actor location and rotation at StartTakeoff (the rise and the tilt are relative to them). */
	FVector TakeoffStartLocation = FVector::ZeroVector;
	FRotator TakeoffStartRotation = FRotator::ZeroRotator;

	/** See GetHeightAboveFloor. */
	float HeightAboveFloor = 0.f;

	/** Along-the-nose speed during the forward phase (uu/s). */
	float ForwardSpeed = 0.f;

	/** Countdown length given at landing (s). */
	float CountdownSeconds = 0.f;

	/** Seconds left. */
	float RemainingSeconds = 0.f;

	/** True once InitializeShuttle ran. */
	bool bInitialized = false;

	/** True once InitializeArrival ran (arrival mode: never boards nor counts down). */
	bool bArrival = false;

	/** True once finished (final). */
	bool bFinished = false;

	/** True when the miner boarded. */
	bool bExtracted = false;
};
