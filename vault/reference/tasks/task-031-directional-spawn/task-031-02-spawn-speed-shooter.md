---
id: task-031-02-spawn-speed-shooter
description: "Spawn band 100–500 uu beyond the view, spawn side biased 55/35/10 toward the run heading, robots 30 % slower, shooters never back away; tests."
parent: task-031-directional-spawn
kind: task
status: done
---
# Closer directional spawns, slower robots, steadfast shooters

- goal: running away meets robots ahead and the first contact comes fast; robots are slower; shooters hold their ground.
- agent: cpp
- depends_on: task-031-01-remove-dummy
- inputs: [[enemies-index]], [[enemy-shooter]], [[sys-spawner]], [[sys-spawner-director]], [[sys-enemies-shooter]], [[sys-enemies]], [[sys-automation-tests]], [[cpp-standard]]
- expected_output: spawner band 100–500 uu beyond the visible floor rectangle edge (replace ViewMargin / MaxSpawnDistanceCells or add uu-based properties; reachable floor cells only, fallback outward when the band has none); miner heading from movement over the last 1 s (speed threshold editable); while moving, each spawn (lone, pack, horde side) picks a sector by weights 55 front (±45°), 35 sides (45–135°), 10 back, editable; uniform when still; EnemyStats.csv MoveSpeed chaser 202, shield chaser 202, kamikaze 168, shooter 168, heavy 112; ARRShooter never increases its distance to the miner (no retreat ring; line-of-sight repositioning that never moves away); tests: sector split measured over many spawns while moving and when still, first contact time for a still miner, shooter holds when approached; docs sys-spawner, sys-spawner-director, sys-enemies-shooter, sys-enemies.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 110000
- model: opus
- tokens: input 162, cache write 399313, cache read 10459966, output 89294
- runs: 3
- milestone: M2
- result: Spawn band 100–500 uu (SpawnBandMin/Max, fallback outward to 3000), detour filter MaxPathDetourRatio 1.5 (owner ok); heading over 1 s, sectors 55/35/10 (measured 56.0/34.7/9.3 lone, 56.7/35.3/8.0 packs running; still matches cell shares); first contact still miner on screen at 2.25 s; shooter never moves away (measured 0.00 uu); speeds 202/168/168/112. 4 new tests; build 0, 188/189 then the failing kamikaze test (time limit 15 → 25 s) passes alone; full suite not rerun. Late-run starvation explained: alive cap 150 filled by robots left behind, plus the arena edge. Commits d269d52, 5bfde91.

## Relations
- [[task-031-directional-spawn]] — parent global task.
