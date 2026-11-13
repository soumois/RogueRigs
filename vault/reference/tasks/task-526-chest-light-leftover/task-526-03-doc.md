---
id: task-526-03-doc
description: "Doc: sys-objectives-console-chest: effect lifetime, if the fix changes how it is built"
parent: task-526-chest-light-leftover
kind: task
status: done
---
# Lumière du coffre restée du niveau précédent: doc

- goal: sys-objectives-console-chest: effect lifetime, if the fix changes how it is built
- agent: doc
- depends_on: task-526-02-capture
- inputs: sys-objectives-console-chest (vault/reference/tech/systems/world/sys-objectives/sys-objectives-console-chest.md)
- expected_output: see done_when
- done_when: A3 doc part holds; lint clean; commit.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-526
- usage: included in the executor run of task-526
- runs: 1
- milestone: M2
- started: 2026-11-13T14:19:19Z
- waiting_for: none
- result: sys-objectives-console-chest updated (commit 02790659): snapshot of every world FX component with its state, new components tracked even before activation, IsOpeningEffectAlive, Deactivate before DeactivateImmediate, task-526 capture note; relation to task-526 on both ends. Lint clean.

## Relations
- [[task-526-chest-light-leftover]] — parent global task.
