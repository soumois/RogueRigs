---
id: task-031-02-spawn-speed-shooter
description: "Spawn band 100–500 uu beyond the view, spawn side biased 55/35/10 toward the run heading, robots 30 % slower, shooters never back away; tests."
parent: task-031-directional-spawn
kind: task
status: in-progress
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
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-031-directional-spawn]] — parent global task.
