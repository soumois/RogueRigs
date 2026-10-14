---
id: task-174-01-debris-regression
description: "Find with a real game run why debris pieces no longer show after tasks 170 to 173, and name the exact fix."
parent: task-174-debris-regression
kind: task
status: in-progress
---
# Robot debris: regression, pieces no longer visible

- goal: Find with a real game run why debris pieces no longer show after tasks 170 to 173, and name the exact fix.
- agent: cpp
- depends_on: none
- inputs: enemy-death (vault/reference/gdd/enemies/enemy-death.md), bp-robot-debris (vault/reference/tech/blueprints/fx/bp-robot-debris.md), sys-enemies-base (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-base.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), orchestrator-lessons-2026-10-14 (vault/reference/agents/orchestrator-lessons-2026-10-14.md)
- expected_output: The cause proven by a -game run (fx.Niagara.DumpComponents values: NumParticles, Position, Scale, Velocity, Age; log lines; a few rr.Debug.Shots described) and, if needed, by comparing with the NS_RobotDebris versions of commits 75f75c5 (worked), f59f4bc (170), 4248105 (171), 144c7a3 and 86a489b (172) and the C++ of d6ce98a (173); the exact fix and who applies it (cpp or bp)
- done_when: Cause proven by values from a run; the commit that introduced it named; the fix stated precisely; nothing committed unless a C++ fix without build was possible
- editor_state: any
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M3
- result: open

## Relations
- [[task-174-debris-regression]] — parent global task.
