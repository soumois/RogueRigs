---
id: sys-enemy-ai
description: "Enemy AI decisions: per-robot C++ state machines, grid flow-field pathfinding, grid line of sight, shooter repositioning search, stun, crowd separation."
parent: systems-index
kind: doc
---
# Enemy AI

- responsibility: movement and decisions of every robot.
- main_classes: `IRRPathProvider`, `URRFlowFieldSubsystem`, `URREnemyMovementComponent` (`Source/RogueRigs/FlowField/`, [[sys-flow-field]]); per-robot state machines in `ARRChaser`, `ARRKamikaze`, `ARRShooter`, `ARRHeavy` ([[sys-enemies]]). The template's `ATwinStickNPC` and `ATwinStickAIController` (`Variant_TwinStick/`) stay as reference only.

## Decisions
- Framework (approved, owner 2026-10-02): a C++ state machine per robot (chaser: Chase, WindUp, Attack, Recover), readable and testable by agents.
- Pathfinding (approved, owner 2026-10-02): a flow field on the grid instead of navmesh, behind a generic movement component that asks a path provider for a direction, so the provider can be swapped. The grid is the source of truth; one BFS from the miner gives every chaser and kamikaze its direction. Reason: `RuntimeGeneration=Dynamic` with 1000 uu tiles rebuilds tiles on each destroyed block, then 200 agents re-path.
- Line of sight (provisional): a walk over grid cells instead of physics traces; blocks are the only obstacles. The same grid test filters shotgun targets ([[sys-weapons]]) and gates the shooter (every tick, only within its range).
- Shooter repositioning (task-021-01, follows the approved C++ state machine framework): a C++ grid search instead of EQS: candidate cells from `GatherReachableCells` around the robot, in its preferred ring, with grid sight to the miner; the shortest local BFS path wins and the robot walks it ([[sys-enemies-shooter]]).
- Stun: the AI listens to `State.Stunned` and stops or resumes movement ([[sys-status-effects]]).
- Crowd separation (task-018-02, owner wish for denser packs): robots ignore each other in movement sweeps (mask filter, [[sys-enemies]]) and RVO alone keeps them apart, with lighter settings; they may overlap. 40 chasers after 10 s: 12 within 200 uu of the miner instead of 7, 19 within 300 uu instead of 11 (`Integration.CrowdPacking`, [[sys-automation-tests]]).
- Movement fallback if stress test S0 fails: a pawn with simple flat-floor movement and hand-computed separation.

- data: per-robot CSV rows.
- depends_on: [[sys-grid]], [[sys-gas-core]].
- implements: [[enemy-chaser]], [[enemy-kamikaze]], [[enemy-shooter]], [[enemy-heavy]].
- gotchas: see [[perf-risks]].

## Relations
- [[systems-index]] — parent index.
- [[sys-flow-field]] — the flow field and movement component as built.
- [[sys-enemies]] — the robots and the chaser state machine as built.
