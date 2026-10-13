---
id: task-119-01-heavy-telegraph-far-bug
description: "A heavy robot shows its slam zone only during its telegraph, never while it runs toward the miner from far away."
parent: task-119-heavy-telegraph-far-bug
kind: task
status: in-progress
---
# Heavy zone visible when far

- goal: A heavy robot shows its slam zone only during its telegraph, never while it runs toward the miner from far away.
- agent: cpp
- depends_on: none
- inputs: enemy-heavy (vault/reference/gdd/enemies/enemy-heavy.md), ground-telegraph (vault/reference/gdd/art-audio/ground-telegraph.md), sys-enemies-heavy (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-heavy.md), sys-ground-telegraph (vault/reference/tech/systems/combat/sys-ground-telegraph.md), sys-actor-pool (vault/reference/tech/systems/world/sys-actor-pool.md), sys-enemies-base (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-base.md)
- expected_output: Cause found and fixed (likely pool reuse or component activation outside the telegraph); the existing heavy telegraph tests still pass, expectations updated only where the fix changes them.
- done_when: cause written in the result; build exits 0 with the editor closed; RogueRigs.Heavy.* and RogueRigs.GroundTelegraph.* pass headless; sys-enemies-heavy updated; lint clean
- editor_state: editor closed (C++ build)
- token_ceiling: 60 turns, commit after each group
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: cause: not the decal (its show/hide was correct) but BP_Heavy's ShockRing, a slam-radius static mesh hidden at rest; `ARREnemy::SetMeshesVisible(true)` showed every mesh component, so a spawner recycling `RelocateTo` (far heavies moved back to run at the miner) and `HandleDeath` (after OnRecover had hidden it) lit the ring; `ARRHeavy::ResetStateMachine` also left Telegraph/Recover without `OnRecover`. Fix: `SetMeshesVisible(false)` records the meshes it hides and `true` re-shows only those (list cleared on pool reuse); heavy `ResetStateMachine` calls `OnRecover` when leaving Telegraph, Slam or Recover. Build exit 0; RogueRigs.Heavy.* + RogueRigs.GroundTelegraph.* 13/13 pass headless, no expectation changed. Left to agent-bp: BP_Heavy OnTelegraphStart still shows ShockRing during the telegraph as well as the decal.

## Relations
- [[task-119-heavy-telegraph-far-bug]] — parent global task.
