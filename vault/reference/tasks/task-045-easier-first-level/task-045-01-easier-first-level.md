---
id: task-045-01-easier-first-level
description: "About 15 % fewer robots (lone 2.35 s, waves 7.1 → 4.1 s, hordes 34–51) and rarer heavies (weight 4, 2–3 per horde, cap 3)."
parent: task-045-easier-first-level
kind: task
status: in-progress
---
# Easier first level

- goal: About 15 % fewer robots (lone 2.35 s, waves 7.1 → 4.1 s, hordes 34–51) and rarer heavies (weight 4, 2–3 per horde, cap 3).
- agent: cpp
- depends_on: none
- inputs: enemies-index (vault/reference/gdd/enemies/enemies-index.md), sys-spawner-director (vault/reference/tech/systems/sys-spawner-director.md), sys-spawner (vault/reference/tech/systems/sys-spawner.md), sys-automation-tests (vault/reference/tech/systems/sys-automation-tests.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md)
- expected_output: FRRWaveDirectorConfig::MakeDefault: lone chaser interval 2 → 2.35 s; wave interval 6 → 3.5 s becomes 7.1 → 4.1 s over the same ramp; horde size 40–60 → 34–51; overtime unchanged; heavy pack weight 8 → 4 (other weight raised to keep 100); heavies per horde 3–5 → 2–3; heavy alive cap 5 → 3 (wherever it lives: spawner WeightedEnemyClasses MaxAlive on BP_EnemySpawner is an asset: if the cap is only there, report it for a bp change instead of editing the asset); update only existing tests this breaks; docs sys-spawner-director, sys-spawner.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 50000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-045-easier-first-level]] — parent global task.
