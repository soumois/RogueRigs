---
id: task-448-03-doc
description: "Doc: Record what the simulation showed for both puffs."
parent: task-448-death-puffs-sim-check
kind: task
status: todo
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
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-448-death-puffs-sim-check]] — parent global task.
