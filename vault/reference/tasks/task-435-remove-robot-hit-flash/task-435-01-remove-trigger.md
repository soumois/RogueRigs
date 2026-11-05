---
id: task-435-01-remove-trigger
description: "Cpp: Taking damage no longer raises any flash value on a robot, and the flash test is gone."
parent: task-435-remove-robot-hit-flash
kind: task
status: todo
---
# Remove the robot hit flash: cpp

- goal: Taking damage no longer raises any flash value on a robot, and the flash test is gone.
- agent: cpp
- depends_on: none
- inputs: task-433-robot-hit-flash (vault/reference/tasks/task-433-robot-hit-flash.md), bp-robot-hit-flash (vault/reference/tech/blueprints/enemies/bp-robot-hit-flash.md), sys-enemies-base (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-base.md)
- expected_output: see done_when
- done_when: Build ok; no flash value written on a hit; Enemies test family passes without the flash test.
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
- [[task-435-remove-robot-hit-flash]] — parent global task.
