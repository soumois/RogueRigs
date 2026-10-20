---
id: task-255-camera-arm-1400
description: "Global task: The miner camera arm is 1400 uu instead of 1700, to test the closer view."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Camera distance 1400

- request: camera distance is 1700, the owner wants to test 1400 (owner, 2026-10-20)
- goal: The miner camera arm is 1400 uu instead of 1700, to test the closer view.
- acceptance: Build passes; the default reads 1400 in the C++ and no Blueprint override of CameraArmLength exists; docs updated; lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-20).
- confirmed_by_owner: yes (2026-10-20)
- milestone: M1
- next_step: task-255-01-camera-arm-1400
- result: open

## Subtasks
1. task-255-01-camera-arm-1400 — cpp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-255-01-camera-arm-1400]] — step 1, cpp.
