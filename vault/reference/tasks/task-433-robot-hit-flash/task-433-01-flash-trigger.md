---
id: task-433-01-flash-trigger
description: "Cpp: A robot taking damage from any source raises and clears its flash value at low cost."
parent: task-433-robot-hit-flash
kind: task
status: todo
---
# Robot hit flash: cpp

- goal: A robot taking damage from any source raises and clears its flash value at low cost.
- agent: cpp
- depends_on: none
- inputs: readability-rules (vault/reference/gdd/art-audio/readability-rules.md), sys-enemies-base (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-base.md), sys-damage (vault/reference/tech/systems/core/sys-damage.md)
- expected_output: see done_when
- done_when: Build passes and existing enemy and damage tests pass; damage to a robot sets the flash value and it returns to zero after 0.08 s, a new hit restarts it, no change to damage or death; the value is exposed for materials on a slot that does not collide with the animation data.
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
- [[task-433-robot-hit-flash]] — parent global task.
