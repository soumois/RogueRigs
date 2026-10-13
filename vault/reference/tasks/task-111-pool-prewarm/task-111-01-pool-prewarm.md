---
id: task-111-01-pool-prewarm
description: "Staggered prewarm per class at level start: XP 30 over 20 frames, chasers 30 over 20, other robots up to 5 over 60."
parent: task-111-pool-prewarm
kind: task
status: done
---
# Staggered prewarm

- goal: The pools are prewarmed at level start, spread over frames per class (XP 30 over 20, chasers 30 over 20, other robots 5 or fewer over 60), and projectile pooling is evaluated.
- agent: cpp
- depends_on: task-110-01-xp-big-block
- inputs: sys-actor-pool (vault/reference/tech/systems/sys-actor-pool.md), sys-spawner (vault/reference/tech/systems/sys-spawner.md), sys-spawner-director (vault/reference/tech/systems/sys-spawner-director.md), sys-pickups (vault/reference/tech/systems/sys-pickups.md), difficulty (vault/reference/gdd/run/difficulty.md), sys-automation-tests (vault/reference/tech/systems/sys-automation-tests.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: owner 2026-10-12: prewarm must spread over more frames than a normal acquire. Use the pool Prewarm(Class, Count, MaxFrames) API (sys-actor-pool; it already spreads creation: check that MaxFrames 60 gives at most ceil(Count / 60) creations per frame and fix it if not). At level start (after the grid is built, not in the first loading frame; choose the place: the run flow or the spawner begin play, document it): XP cubes 30 over 20 frames; chaser 30 over 20 frames; every other robot class used by the level (shield chaser, shooter, kamikaze, heavy): min(5, the class MaxAlive cap) over 60 frames, and 0 when the class is not part of the level (read the wave configuration: WeightedEnemyClasses weight 0 or MaxAlive 0, director config, per-class caps; document how the "in the level" decision is made). Counts and frames are data (EditDefaultsOnly / config), the defaults being the owner values. Tests: counts and frame spread per class, 0 for an absent class, the cap respected, the prewarmed actors count as pooled idle (not alive) and are used by the first spawns. Docs sys-actor-pool, sys-spawner, sys-automation-tests. Editor closed.
- done_when: build exits 0 with the editor closed; all RogueRigs.* tests pass headless; docs updated; lint clean; committed.
- editor_state: closed
- token_ceiling: 60000
- model: opus
- tokens: input 124, cache write 174886, cache read 7717457, output 57130
- runs: 2
- milestone: M2
- result: Prewarm at the spawner second tick: XP 30/20, chasers 30/20, others min(5, cap)/60; absent class = not in the director data nor weighted > 0 (differs from the literal weight 0 rule: director spawns weight 0 classes); pool batches now really spread (creation i due on frame ceil((i+1) x MaxFrames / Count)); 248/248 tests. Commits fa0f54c, 80061c3.

## Relations
- [[task-111-pool-prewarm]] — parent global task.
