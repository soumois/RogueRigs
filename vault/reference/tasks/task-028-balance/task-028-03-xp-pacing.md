---
id: task-028-03-xp-pacing
description: "Scripted -game run measuring time to each level; tune the XP curve to level 2 at 30 s, 5 at 1:40, 10 at 3:10."
parent: task-028-balance
kind: task
status: done
---
# XP pacing

- goal: level-ups keep a fast early rhythm.
- agent: cpp
- depends_on: task-028-02-bp-shield-heavy
- inputs: [[xp-level-up]], [[sys-progression]], [[sys-stress]], [[drgs-analysis]], [[cpp-standard]], [[sys-automation-tests]], [[drgs-analysis]]
- expected_output: a scripted bot run (miner aiming at the nearest robot, invulnerable, auto-picking the first card, same seed) logging level times; XPCurve.csv tuned (formula documented) to hit the targets ±20 %; table before/after in sys-progression; tests updated. From task-028-01: update Integration.PlacedSpawnerMix for the fifth WeightedEnemyClasses entry (BP_ShieldChaser_C, weight 0) added in task-028-02. From task-028-02: PlacedSpawnerMix must read the spawner placed in L_ArenaM1 from disk and check it has BP_ShieldChaser_C (weight 0); the bp agent saw stale four-entry weights on the placed instance in the editor: if the saved map lacks the entry, report it precisely for a bp fix (do not edit .uasset). One -game shot run (open and describe): damage digits with the new T_DamageDigitsOutline (outline readable over yellow chasers, glyphs aligned, no clipped digits), shield chaser shell and its break, heavy telegraph ring at 400 uu, kamikaze flare at 10000, heal ring on level-up.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless (new ones included); docs updated; lint clean.
- editor_state: closed
- token_ceiling: 130000
- model: opus
- tokens: input 174, cache write 425983, cache read 8439158, output 62021
- runs: 2
- milestone: M2
- result: open

## Relations
- [[task-028-balance]] — parent global task.
