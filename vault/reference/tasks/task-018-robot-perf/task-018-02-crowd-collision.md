---
id: task-018-02-crowd-collision
description: "Robots stop sweeping against each other’s capsules; separation left to RVO with lighter settings; robots pack tighter; stress before and after."
parent: task-018-robot-perf
kind: task
status: todo
---
# Crowd collision

- goal: cut the crowd collision cost and let packs be denser.
- agent: cpp
- depends_on: task-018-01-trace
- inputs: task-018-01 result, [[perf-risks]], [[sys-enemies]], [[sys-enemy-ai]], [[sys-flow-field]], [[sys-automation-tests]], [[cpp-standard]].
- expected_output: robot capsules ignore other robots (collision profile or channel response set in C++ so Blueprints inherit it; miner, blocks, barrels, projectiles and shotgun queries still hit robots); RVO consideration radius and update tuned lighter, values documented; melee and kamikaze proximity still work; stress before/after; tests updated or added (robots overlap allowed, still reach and damage the miner). [[sys-enemies]], [[sys-enemy-ai]], [[perf-risks]] updated.
- done_when: build exits 0; all `RogueRigs.*` tests pass headless; before/after stress numbers (S0 and S2 at 100, 200, 400, same procedure as [[stress-final-m1]]) recorded; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 90000
- milestone: M1
- result: open

## Relations
- [[task-018-robot-perf]] — parent global task.
