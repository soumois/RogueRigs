---
id: task-448-03-doc
description: "Doc: Record what the simulation showed for both puffs."
parent: task-448-death-puffs-sim-check
kind: task
status: done
---
# Check the robot death puffs in the simulation: doc

- goal: Record what the simulation showed for both puffs.
- agent: doc
- depends_on: task-448-02-heavy-puff-check
- inputs: bp-robot-death-puff (vault/reference/tech/blueprints/fx/bp-robot-death-puff.md), bp-heavy-death-puff (vault/reference/tech/blueprints/fx/bp-heavy-death-puff.md)
- expected_output: see done_when
- done_when: Both nodes state the check and its figures, and any fix; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-448
- usage: included in the executor run of task-448
- runs: 1
- milestone: M2
- started: 2026-11-05T22:40:52Z
- waiting_for: none
- result: Done (8dd1eeba): bp-robot-death-puff and bp-heavy-death-puff carry a simulation_check line with the task-448 per-emitter figures, no fix, and the probe traps (level-up pause, background throttle); the former 'Not verified' text kept as not_verified. Lint clean (warnings only, none on these nodes).

## Relations
- [[task-448-death-puffs-sim-check]] — parent global task.
