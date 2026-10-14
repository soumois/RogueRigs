---
id: task-166-05-debris-not-visible
description: "Find why the debris pieces do not show in game (the death puff does) with a real game run, and fix it if the cause is in C++."
parent: task-166-robot-debris-pieces
kind: task
status: in-progress
---
# Robot debris: not visible in game

- goal: Find why the debris pieces do not show in game while the death puff does (owner, 2026-10-14), with a real -game run; fix it if the cause is in C++, otherwise report the exact Niagara defect for a bp fix.
- agent: cpp
- depends_on: task-166-04-debris-niagara
- inputs: enemy-death (vault/reference/gdd/enemies/enemy-death.md), sys-enemies-base (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-base.md), sys-actor-pool (vault/reference/tech/systems/world/sys-actor-pool.md), bp-robot-debris (vault/reference/tech/blueprints/fx/bp-robot-debris.md), bp-robot-death-puff (vault/reference/tech/blueprints/fx/bp-robot-death-puff.md), bp-chaser (vault/reference/tech/blueprints/enemies/bp-chaser.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), orchestrator-lessons-2026-10-14 (vault/reference/agents/orchestrator-lessons-2026-10-14.md)
- expected_output: the cause, proven by a game run (log values and/or rr.Debug.Shots images described); a C++ fix if the cause is in C++; otherwise the exact Niagara setting to change
- done_when: cause proven by a run; if fixed in C++, a rerun shows debris pieces (described from shots); build passes; committed by path
- editor_state: any (open for a -game run; closed only for a C++ build)
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M3
- result: open

## Relations
- [[task-166-robot-debris-pieces]] — parent global task.
