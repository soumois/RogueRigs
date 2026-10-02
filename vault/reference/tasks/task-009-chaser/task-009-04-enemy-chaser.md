---
id: task-009-04-enemy-chaser
description: "ARREnemy base (ASC, health and enemy sets, CSV stats, single death, shrink, XP cube) and ARRChaser state machine: chase, wind-up, attack, recover."
parent: task-009-chaser
kind: task
status: in-progress
---
# Enemy base and chaser

- goal: a reusable robot base and the first robot built on it.
- agent: cpp
- depends_on: task-009-03-flow-field
- inputs: [[enemy-chaser]], [[enemies-index]], [[enemy-death]], [[sys-enemy-ai]], [[sys-damage]], [[sys-gas-core]], [[sys-pickups]], `sys-flow-field`, [[naming]], [[sys-automation-tests]], [[cpp-standard]].
- expected_output: `ARREnemy` (abstract `ACharacter`, `IAbilitySystemInterface`): own ASC with health and enemy sets; stats from `Content/RogueRigs/Data/Tables/EnemyStats.csv` by row name (columns at least MaxHealth, MoveSpeed, Damage, AttackRate, AttackRange, WindUp, XPValue, KnockbackResistance); row `Chaser`: 30, 450, 10, 1, 120, 0.25, 1, 0; capsule overlaps `Projectile` so shots hit it (check [[sys-projectiles]]); RVO avoidance on its movement; `URREnemyMovementComponent`; on death (single, `State.Dead`): stop, disable collision, shrink to 0 over 0.15 s, drop `XPValue` XP pickups, destroy. `ERRPickupType` gains `XP` (inert on collection, logged). `ARRChaser`: C++ state machine Chase (follow provider) → WindUp (target within `AttackRange`: stop, flash the mesh for `WindUp`) → Attack (if still in range, apply `URRGE_DamageFlat` with `Damage` to the miner) → Recover (until 1/`AttackRate` since the attack) → Chase; state readable by getter. Grid: XP pickup class editable on the enemy, not the grid. New doc `sys-enemies` (template `system.md`); [[sys-enemy-ai]], [[sys-pickups]] updated. Tests `RogueRigs.Enemies.*`: stats loaded from CSV; 3 burst-rifle hits kill a chaser and it drops 1 XP pickup; a chaser next to the miner deals 10 after 0.25 s and not more than once per second; i-frames refuse a second chaser hitting in the same 0.25 s.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs match; lint clean.
- editor_state: closed
- token_ceiling: 100000
- milestone: M1
- result: open

## Relations
- [[task-009-chaser]] — parent global task.
