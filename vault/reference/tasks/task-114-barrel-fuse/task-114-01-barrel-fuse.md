---
id: task-114-01-barrel-fuse
description: "Barrels have a doubled explosion radius and explode 1.5 s after reaching 0 health, with an orange danger zone and a growing circle."
parent: task-114-barrel-fuse
kind: task
status: done
---
# Barrel fuse and double radius

- goal: Barrels have a doubled explosion radius and explode 1.5 s after reaching 0 health, with an orange danger zone and a growing circle.
- agent: cpp
- depends_on: task-115-01-heavy-slam-telegraph, the barrel tasks of the other session (task-107, task-108-barrel-on-floor) done
- inputs: barrel (vault/reference/gdd/terrain/barrel.md), explosions (vault/reference/gdd/terrain/explosions.md), ground-telegraph (vault/reference/gdd/art-audio/ground-telegraph.md), sys-explosions (vault/reference/tech/systems/sys-explosions.md), sys-grid (vault/reference/tech/systems/sys-grid.md), bp-explosion-fx (vault/reference/tech/blueprints/bp-explosion-fx.md), sys-automation-tests (vault/reference/tech/systems/sys-automation-tests.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: owner rules 2026-10-12 (already written in the GDD nodes barrel and explosions): (1) the Barrel row of Content/RogueRigs/Data/Tables/ExplosionStats.csv radius 350 -> 700 uu (damage 40 unchanged; the visible explosion is 1.05 x as set by task-097); (2) when a barrel reaches 0 health it does not explode at once: it explodes 1.5 s later, whatever killed it (a barrel hit by another explosion fuses too, so chains become slow); further damage during the fuse is ignored; the barrel mesh/cell stays until the explosion; the terrain is destroyed at the explosion, not at 0 health; (3) from 0 health a danger zone is drawn on the ground at the explosion radius (orange, mode Danger) with the growing circle reaching the edge at the explosion (progress = elapsed / 1.5), using the ground telegraph (task-112-01) attached to the barrel; it disappears if the barrel is destroyed otherwise. FIRST read how barrels are implemented NOW: the other session task-107-barrel-integration and task-108-barrel-on-floor changed the barrel code and mesh (git log, sys-grid, the barrel doc nodes): build on their final state, do not revert their work, and report any conflict. Check the effect on: the chain queue (0.08 s link delay), kamikaze deaths by barrel explosions (unchanged rules), the huge radius (700 uu = about 10 cells: terrain destruction, damage falloff, performance of the block destruction batch at one frame: measure the frame cost of one 700 uu explosion on arena_m1_fine and report it), the existing tests that assume radius 350 or an instant barrel (update expectations only where this change breaks them). New tests: fuse of 1.5 s, chain of two barrels = 3 s total, radius 700 destroys the expected cells, telegraph progress. Docs sys-grid / sys-explosions / sys-automation-tests. Editor closed.
- done_when: build exits 0 with the editor closed; all RogueRigs.* tests pass headless; docs updated; lint clean; committed.
- editor_state: closed
- token_ceiling: 60000
- model: opus
- tokens: input 120, cache write 161164, cache read 6636074, output 65927
- runs: 1
- milestone: M2
- result: Barrel radius 700 uu, 1.5 s fuse from 0 health (BarrelFuseSeconds), ground telegraph hosted by the arena grid (pooled), damage ignored during the fuse, chains add 1.5 s each; one 700 uu explosion costs about 8 ms (4.6-5.6 ms block destruction): proposal to spread it; 261/261 tests. Commits 459dda6, f044d39.

## Relations
- [[task-114-barrel-fuse]] — parent global task.
