---
id: task-017-01-radius
description: "Set the kamikaze explosion radius to 312 uu in data, make sure the explosion visual scales with it, update tests and docs."
parent: task-017-kamikaze-radius
kind: task
status: done
---
# Kamikaze radius 312 uu

- goal: the kamikaze explodes at 312 uu.
- agent: cpp
- depends_on: none
- inputs: [[enemy-kamikaze]], [[explosions]], [[sys-explosions]], [[sys-enemies]], [[bp-kamikaze]], [[bp-explosion-fx]], [[sys-automation-tests]], [[cpp-standard]].
- expected_output: kamikaze radius 312 in its stats data (CSV or Blueprint default, wherever it lives; if it is a Blueprint default, report it for the bp agent instead of editing the asset); the explosion FX scale derived from the radius (confirm, or report what the bp agent must change); tests that assumed 250 updated and one asserting a target at about 300 uu is hit; [[sys-explosions]], [[sys-enemies]] updated.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless (new ones included); docs updated; lint clean.
- editor_state: closed
- token_ceiling: 50000
- model: opus
- tokens: 51465
- runs: 1
- milestone: M1
- result: ExplosionStats.csv row Kamikaze Radius 250 → 312 (data only); FX already scales from the row radius (sphere scale 6.24). New test Explosions.KamikazeReach (dummy at 300 uu hit, at 400 uu untouched); RifleKillsKamikazeExplodes now breaks 2 blocks. Build 0, 98/98. Docs sys-explosions, sys-enemies, sys-automation-tests. Commit 4afbf4a, about 50k tokens.

## Relations
- [[task-017-kamikaze-radius]] — parent global task.
