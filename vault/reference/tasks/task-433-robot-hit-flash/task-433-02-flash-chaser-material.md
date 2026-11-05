---
id: task-433-02-flash-chaser-material
description: "Bp: Representative slice: the chaser reads the flash value and turns plain white, baked animation intact."
parent: task-433-robot-hit-flash
kind: task
status: in-progress
---
# Robot hit flash: bp

- goal: Representative slice: the chaser reads the flash value and turns plain white, baked animation intact.
- agent: bp
- depends_on: task-433-01-flash-trigger
- inputs: readability-rules (vault/reference/gdd/art-audio/readability-rules.md), enemy-chaser (vault/reference/gdd/enemies/enemy-chaser.md), chaser-meshy-pipeline (vault/reference/tech/pipelines/chaser-meshy-pipeline.md)
- expected_output: see done_when
- done_when: Chaser material compiled with no error, reads the flash slot, plain white at 1 and unchanged at 0; animation data slots untouched; change readable by the next subtask as the pattern for the other classes.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-05T16:13:51Z
- waiting_for: none
- result: open

## Relations
- [[task-433-robot-hit-flash]] — parent global task.
