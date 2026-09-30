---
id: sys-enemy-ai
description: "Enemy AI: framework choice (open), grid flow-field pathfinding, grid line of sight, stun reaction, shooter repositioning."
parent: systems-index
kind: doc
---
# Enemy AI

- responsibility: movement and decisions of every robot.
- main_classes: the template's `ATwinStickNPC` (an `ACharacter`) and `ATwinStickAIController` with `UStateTreeAIComponent` (StateTree plugin enabled).

## Decisions
- Framework: open. Options: StateTree (template default), Behavior Tree, or a C++ state machine in the AI controller. Only the C++ state machine is readable by agents without the editor.
- Pathfinding (provisional): a flow field on the grid instead of navmesh. The grid is the source of truth; one BFS from the miner gives every chaser and kamikaze its direction. Reason: `RuntimeGeneration=Dynamic` with 1000 uu tiles rebuilds tiles on each destroyed block, then 200 agents re-path.
- Line of sight (provisional): a walk over grid cells instead of physics traces, at a throttled rate; blocks are the only obstacles.
- Shooter repositioning when line of sight is lost: EQS was the candidate; depends on the framework choice (open).
- Stun: the AI listens to `State.Stunned` and stops or resumes movement ([[sys-status-effects]]).
- Movement fallback if stress test S0 fails: a pawn with simple flat-floor movement and hand-computed separation.

- data: per-robot CSV rows.
- depends_on: [[sys-grid]], [[sys-gas-core]].
- implements: [[enemy-chaser]], [[enemy-kamikaze]], [[enemy-shooter]], [[enemy-heavy]].
- gotchas: see [[perf-risks]].

## Relations
- [[systems-index]] — parent index.
