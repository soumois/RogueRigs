---
id: task-004-miner-gas
description: "Global task: enable GAS and build the C++ miner from scratch (ASC, attribute sets, CSV base stats, move, aim), replacing the TwinStick pawn."
parent: tasks-index
kind: task
status: in-progress
---
# Miner with GAS

- request: "Le personnage : on en refait un from scratch en s'inspirant [du TwinStick] [...] intégrer le ASC. Propose des valeurs de base et applique-les." Owner confirmed the from-scratch option (2026-10-01).
- goal: `L_ArenaM1` plays with `BP_Miner`, a C++ `ARRMiner` that moves and aims like the TwinStick character, has no dash, shot or AoE, owns an ASC with the health, movement and player attribute sets initialised from a CSV, and whose `MoveSpeed` attribute drives its walk speed.
- acceptance: (1) GAS plugin and modules enabled, build exits 0; (2) `ARRMiner`, `ARRPlayerController`, `ARRGameMode` and the three attribute sets exist with the [[naming]] conventions; (3) base stats come from `Content/RogueRigs/Data/Tables/PlayerBaseStats.csv` at runtime, values from [[player-stats]]; (4) in PIE on `L_ArenaM1` the pawn is `BP_Miner`, spawns on the spawn cell, moves, faces the mouse, and its `MaxWalkSpeed` equals `MoveSpeed` (600); (5) `Variant_TwinStick` untouched; (6) [[sys-gas-core]] and a new `sys-player` doc match the code; lint clean, every subtask committed.
- gdd_check: read [[miner]], [[player-stats]], [[sys-gas-core]], [[gas-scope]], [[cpp-bp-split]], [[naming]]. No dash, shot or AoE in the GDD: dropped. Weapons fire on their own with their own ASC: out of this task. Turret button: out of this task. Base stat values were open: provisional values set (owner asked to apply them; [[player-stats]]). Camera values: kept from the TwinStick template (pitch −50°, arm 2200 uu, FOV 75) as editable properties.
- confirmed_by_owner: yes
- milestone: M1
- next_step: task-004-02-gas-core
- result: open

## Subtasks
1. task-004-01-enable-gas — cpp: executes [[task-000-enable-gas]].
2. task-004-02-gas-core — cpp: custom ASC, three attribute sets, CSV base stats loader.
3. task-004-03-miner-classes — cpp: `ARRMiner`, `ARRPlayerController`, `ARRGameMode`.
4. task-004-04-bp-miner — bp: `BP_Miner`, `BP_RRPlayerController`, `BP_RRGameMode`, map switch, PIE check.

## Relations
- [[tasks-index]] — parent index.
- [[task-004-01-enable-gas]] — step 1, cpp, GAS plugin and modules.
- [[task-004-02-gas-core]] — step 2, cpp, ASC, attribute sets, CSV.
- [[task-004-03-miner-classes]] — step 3, cpp, miner, controller, game mode.
- [[task-004-04-bp-miner]] — step 4, bp, Blueprints, map, PIE.
