---
id: task-011-04-barrel
description: "Barrel cells get 20 health from shots and explode through the queue when destroyed by any source, so barrels chain."
parent: task-011-explosions
kind: task
status: done
---
# Barrel

- goal: barrels explode when shot, dug or caught in a blast, and chain.
- agent: cpp
- depends_on: task-011-03-explosion-system
- inputs: [[barrel]], [[explosions]], [[sys-explosions]], [[sys-grid]], [[sys-projectiles]], [[destruction-rules]], [[sys-automation-tests]], [[cpp-standard]].
- expected_output: grid barrel cells keep their grid block (HISM) with a separate shot health (editable `BarrelShotHealth` 20); a projectile that stops on a barrel instance applies its damage value to that cell (cell from the hit item or location; no GAS on barrels per [[sys-explosions]]); at 0 the cell is destroyed. Whenever a barrel cell is destroyed, whatever the source (shots, drone dig, explosion), the grid queues a `Barrel` explosion at the cell center with the cell as the source object. Docs [[sys-grid]], [[sys-explosions]], [[sys-projectiles]]. Tests `RogueRigs.Barrel.*`: two rifle shots destroy a barrel and queue one explosion; a barrel inside another barrel's radius chains 0.08 s later; digging a barrel makes it explode.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs match; lint clean.
- editor_state: closed
- token_ceiling: 80000
- milestone: M1
- result: barrel shot health 20 per cell (dig health unchanged); projectile stop on a barrel instance → ApplyShotDamage with the bullet damage snapshot from the spec (no crit, enemy flat shots ignored); any barrel destruction (Shot, Drone, Barrel, Kamikaze) queues a Barrel explosion with one URRBarrelSource per cell. 3 Barrel tests (two shots explode; pair (73, 31)+(73, 32) chains 0.083 s later; dug barrel explodes). Build exit 0, 44/44 (orchestrator re-ran). Found: 3 of the 4 layout pairs are farther than 350 uu (orchestrator spec said "within 2 cells"), sent back to task-011-02. About 104k tokens.

## Relations
- [[task-011-explosions]] — parent global task.
