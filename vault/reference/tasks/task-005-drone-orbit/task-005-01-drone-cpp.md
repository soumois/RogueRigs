---
id: task-005-01-drone-cpp
description: "Write ARRDrone: a separate actor spawned by ARRMiner, with a state enum and an orbit movement around the miner from editable data."
parent: task-005-drone-orbit
kind: task
status: todo
---
# C++: ARRDrone (orbit)

- goal: the drone actor and its orbit behaviour.
- agent: cpp
- depends_on: task-004-miner-gas
- inputs: [[drone]], [[sys-gas-core]], `sys-player`, [[cpp-bp-split]], [[naming]], [[cpp-standard]].
- expected_output: `Source/RogueRigs/Drone/RRDrone.h/.cpp`: `ARRDrone : AActor` with a root static mesh component (no collision with pawns), `ERRDroneState` (Orbit, Dig, Hack; only Orbit implemented), editable `OrbitRadius` 150, `OrbitPeriod` 2 s, `HoverHeight` 150, `FollowInterpSpeed`; `SetOwnerMiner`, `GetState`, `GetMiningSpeed()` (reads `MiningSpeed` from the miner's ASC, 1 if missing). `ARRMiner` gets an editable `TSubclassOf<ARRDrone> DroneClass`, spawns it at `BeginPlay` (owner set, kept in a UPROPERTY pointer), destroys it at `EndPlay`. New doc node `sys-drone` (template `system.md`) linked from [[systems-index]]; `sys-player` updated.
- done_when: build exits 0 with the editor closed; no new warnings in touched files; docs match; lint clean.
- editor_state: closed
- token_ceiling: 50000
- milestone: M1
- result: open

## Relations
- [[task-005-drone-orbit]] — parent global task.
