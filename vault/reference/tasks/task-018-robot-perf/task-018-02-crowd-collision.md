---
id: task-018-02-crowd-collision
description: "Robots stop sweeping against each other’s capsules; separation left to RVO with lighter settings; robots pack tighter; stress before and after."
parent: task-018-robot-perf
kind: task
status: done
---
# Crowd collision

- goal: let packs be denser (owner wish); the trace (018-01) shows only 61 robot-robot blocking hits per frame (0.05 ms) and RVO 0.33 ms at 400, so the expected perf gain is small (−0.3 to −1 ms).
- agent: cpp
- depends_on: task-018-01-trace
- inputs: task-018-01 result, [[perf-risks]], [[sys-enemies]], [[sys-enemy-ai]], [[sys-flow-field]], [[sys-automation-tests]], [[cpp-standard]].
- expected_output: robot capsules ignore other robots (collision profile or channel response set in C++ so Blueprints inherit it; miner, blocks, barrels, projectiles and shotgun queries still hit robots); RVO consideration radius and update tuned lighter, values documented; melee and kamikaze proximity still work; stress before/after; tests updated or added (robots overlap allowed, still reach and damage the miner). [[sys-enemies]], [[sys-enemy-ai]], [[perf-risks]] updated.
- done_when: build exits 0; all `RogueRigs.*` tests pass headless; before/after stress numbers (S0 and S2 at 100, 200, 400, same procedure as [[stress-final-m1]]) recorded; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 90000
- model: opus
- tokens: 123924
- runs: 1
- milestone: M1
- result: Robot capsules carry RobotMaskFilter and robot movement sweeps ignore it (miner, terrain, barrels, projectiles still collide); ARREnemy SetIgnoreOtherRobots, Enemy|Crowd settings: avoidance consideration 500 → 250 uu, radius expansion 1.5 → 1.1, lock time 0.2 → 0.3 s. Packing (40 chasers, 10 s): within 300 uu 11 → 19, mean distance 617 → 496 uu, attacks 20 → 28. Stress game avg: S0 400 11.31 → 10.96 (noise); S2 400 13.34 → 14.6–14.7 (+1.3 ms, likely more hits in the denser crowd, unconfirmed). Tests RobotsOverlapRobots, Integration.CrowdPacking; build 0, 100/100. Docs sys-enemies, sys-enemy-ai, perf-risks, sys-automation-tests. Commit 270955a, about 125k tokens.

## Relations
- [[task-018-robot-perf]] — parent global task.
