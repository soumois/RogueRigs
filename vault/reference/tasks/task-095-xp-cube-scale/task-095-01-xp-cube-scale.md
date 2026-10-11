---
id: task-095-01-xp-cube-scale
description: "Reused XP cubes come back at their normal size: the pooled reset restores the scale."
parent: task-095-xp-cube-scale
kind: task
status: done
---
# XP cube scale after reuse

- goal: Reused XP cubes come back at their normal size: the pooled reset restores the scale (and any other visual state that differs from a fresh cube).
- agent: cpp
- depends_on: none
- inputs: sys-pickups (vault/reference/tech/systems/sys-pickups.md), sys-actor-pool (vault/reference/tech/systems/sys-actor-pool.md), bp-pickup-xp (vault/reference/tech/blueprints/bp-pickup-xp.md), bp-pickup-ore (vault/reference/tech/blueprints/bp-pickup-ore.md), bp-pickup-heal (vault/reference/tech/blueprints/bp-pickup-heal.md), sys-automation-tests (vault/reference/tech/systems/sys-automation-tests.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: owner playtest: reused XP cubes (suspected) are much bigger than the first cubes of the run (see the capture: large cyan cubes); the first ones were fine, so it is a reset bug of the pool (tasks 092/090). Find the cause (what scale or transform a pooled cube carries from its previous life or from the Acquire transform: pop animation scale, DropXP passing the dying robot scale, relative scale of components, Niagara or mesh component scale), fix it so a reused pickup equals a fresh one in actor scale, component scales, rotation, visibility, trail state; check ore and heal pickups with the same reset. Test: a cube acquired after release and after a robot death shrink has scale 1 (the BP default) in the actor and in its components; mass drop from a shrunk robot gives normal cubes. Editor closed.
- done_when: build exits 0 with the editor closed; all RogueRigs.* tests pass headless; docs sys-pickups and sys-actor-pool updated if a statement changes; lint clean; committed.
- editor_state: closed
- token_ceiling: 40000
- model: opus
- tokens: input 46, cache write 131332, cache read 835605, output 20285
- runs: 1
- milestone: M2
- result: Root cause: the pool reused cubes at requested scale 1 while the Blueprint root mesh is 0.3 (fresh spawn multiplies), so reused cubes were 3.3x bigger. Pool records root scale / requested scale at creation and reapplies on reuse; 2 regression tests; 227/227. Commit fd920a2.

## Relations
- [[task-095-xp-cube-scale]] — parent global task.
