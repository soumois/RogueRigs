---
id: task-449-02-doc
description: "Doc: Record that the wave follows the miner."
parent: task-449-level-up-wave-follows
kind: task
status: done
---
# Level-up floor wave follows the miner: doc

- goal: Record that the wave follows the miner.
- agent: doc
- depends_on: task-449-01-wave-follows
- inputs: sys-level-up (vault/reference/tech/systems/core/sys-level-up.md), bp-level-up-burst (vault/reference/tech/blueprints/fx/bp-level-up-burst.md)
- expected_output: see done_when
- done_when: Both nodes state the current behaviour; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-449
- usage: included in the executor run of task-449
- runs: 1
- milestone: M2
- started: 2026-11-05T22:49:50Z
- waiting_for: none
- result: Commit 5e984eb5. sys-level-up (burst entry, GetWaveLocation, GetFollowedWave, tick only while the wave plays, test figures) and bp-level-up-burst (purpose, verified, unverified) state that the wave follows the miner, the text stays and the push is one blow. Lint clean.

## Relations
- [[task-449-level-up-wave-follows]] — parent global task.
