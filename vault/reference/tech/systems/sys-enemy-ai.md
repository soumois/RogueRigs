---
id: sys-enemy-ai
description: "Enemy AI: framework choice (open), grid flow-field pathfinding, grid line of sight, stun reaction, shooter repositioning."
parent: systems-index
kind: doc
---
# Enemy AI

- responsibility: movement and decisions of every robot.
- main_classes: `IRRPathProvider`, `URRFlowFieldSubsystem`, `URREnemyMovementComponent` (`Source/RogueRigs/FlowField/`, [[sys-flow-field]]). The template's `ATwinStickNPC` and `ATwinStickAIController` (`Variant_TwinStick/`) stay as reference only.

## Decisions
- Framework (approved, owner 2026-10-02): a C++ state machine per robot (chaser: Chase, WindUp, Attack, Recover), readable and testable by agents.
- Pathfinding (approved, owner 2026-10-02): a flow field on the grid instead of navmesh, behind a generic movement component that asks a path provider for a direction, so the provider can be swapped. The grid is the source of truth; one BFS from the miner gives every chaser and kamikaze its direction. Reason: `RuntimeGeneration=Dynamic` with 1000 uu tiles rebuilds tiles on each destroyed block, then 200 agents re-path.
- Line of sight (provisional): a walk over grid cells instead of physics traces, at a throttled rate; blocks are the only obstacles. The same grid test filters shotgun targets ([[sys-weapons]]).
- Shooter repositioning when line of sight is lost: EQS was the candidate; depends on the framework choice (open).
- Stun: the AI listens to `State.Stunned` and stops or resumes movement ([[sys-status-effects]]).
- Movement fallback if stress test S0 fails: a pawn with simple flat-floor movement and hand-computed separation.

- data: per-robot CSV rows.
- depends_on: [[sys-grid]], [[sys-gas-core]].
- implements: [[enemy-chaser]], [[enemy-kamikaze]], [[enemy-shooter]], [[enemy-heavy]].
- gotchas: see [[perf-risks]].

## Relations
- [[systems-index]] — parent index.
- [[sys-flow-field]] — the flow field and movement component as built.
