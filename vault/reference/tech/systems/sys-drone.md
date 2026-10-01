---
id: sys-drone
description: "Drone C++ actor: spawned by the miner, state enum (orbit implemented, dig and hack declared), orbit tunables, MiningSpeed read from the miner's ASC."
parent: systems-index
kind: doc
---
# Drone

- responsibility: the helper drone actor and its state behaviours. Only Orbit is implemented; Dig and Hack are declared with no behaviour (digging and hacking tasks add them). No dialogue, laser or block logic yet.
- main_classes (in `Source/RogueRigs/Drone/`):
  - `ARRDrone : AActor` (`RRDrone.h`, `Abstract`, subclassed in Blueprint): root `UStaticMeshComponent` `Mesh` with profile `NoCollision` (no overlap events, does not affect navigation); mesh set in Blueprint. Editable: `OrbitRadius` (150 uu), `OrbitPeriod` (2 s), `HoverHeight` (150 uu), `FollowInterpSpeed` (10, provisional; 0 = snap).
  - `ERRDroneState` (`uint8`, BlueprintType): `Orbit`, `Dig`, `Hack`.
- entry_points:
  - Spawned by `ARRMiner::BeginPlay` from the miner's editable `DroneClass` (owner and instigator = miner, kept in the miner's `Drone` UPROPERTY); destroyed in `ARRMiner::EndPlay` ([[sys-player]]). `DroneClass` unset = no drone, warning logged.
  - `SetOwnerMiner(ARRMiner*)`: sets the followed miner, adds it as tick prerequisite, resets the orbit angle to 0 and snaps the drone to the orbit point.
  - `GetOwnerMiner()`, `GetState()`, `SetState(ERRDroneState)` (accepts only `Orbit` for now, returns false and logs otherwise), `GetMiningSpeed()` (miner ASC `MiningSpeed`; 1 if no miner, ASC or attribute).
  - Orbit tick: angle += 2π / `OrbitPeriod` per second (wrapped to [0, 2π)); orbit point = miner location + `OrbitRadius` × (cos, sin) in XY, Z = miner capsule bottom + `HoverHeight`; the drone moves toward it with `FMath::VInterpTo` at `FollowInterpSpeed`.
- data: no table; tunables are `EditDefaultsOnly` defaults on the Blueprint subclass. Values from [[drone]].
- depends_on: [[sys-player]] (miner, capsule), [[sys-gas-core]] (`URRPlayerAttributeSet::MiningSpeed`; the drone has no ASC).
- implements: [[drone]].
- gotchas:
  - The drone trails the orbit point slightly (interpolation toward a moving target); raise `FollowInterpSpeed` or set 0 for an exact circle.
  - Orbit angle 0 is world +X; it does not follow the miner's facing.
  - Can the drone be hit: open ([[drone]]); collision is off for now.

## Relations
- [[systems-index]] — parent index.
- [[drone]] — gameplay spec the actor implements.
- [[sys-player]] — the miner that spawns, owns and destroys the drone.
- [[sys-gas-core]] — where `MiningSpeed` lives.
