---
id: task-028-03-xp-pacing
description: "Scripted -game run measuring time to each level; tune the XP curve to level 2 at 30 s, 5 at 1:40, 10 at 3:10."
parent: task-028-balance
kind: task
status: todo
---
# XP pacing

- goal: level-ups keep a fast early rhythm.
- agent: cpp
- depends_on: task-028-02-bp-shield-heavy
- inputs: [[xp-level-up]], [[sys-progression]], [[sys-stress]], [[drgs-analysis]], [[cpp-standard]], [[sys-automation-tests]], [[drgs-analysis]]
- expected_output: a scripted bot run (miner aiming at the nearest robot, invulnerable, auto-picking the first card, same seed) logging level times; XPCurve.csv tuned (formula documented) to hit the targets ±20 %; table before/after in sys-progression; tests updated. From task-028-01: update Integration.PlacedSpawnerMix for the fifth WeightedEnemyClasses entry (BP_ShieldChaser_C, weight 0) added in task-028-02.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless (new ones included); docs updated; lint clean.
- editor_state: closed
- token_ceiling: 100000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-028-balance]] — parent global task.
