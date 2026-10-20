---
id: task-255-camera-arm-1400
description: "Global task: The miner camera arm is 1400 uu instead of 1700, to test the closer view."
parent: tasks-done-index
kind: task
status: done
---
# Camera distance 1400

- request: camera distance is 1700, the owner wants to test 1400 (owner, 2026-10-20)
- goal: The miner camera arm is 1400 uu instead of 1700, to test the closer view.
- acceptance: Build passes; the default reads 1400 in the C++ and no Blueprint override of CameraArmLength exists; docs updated; lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-20).
- confirmed_by_owner: yes (2026-10-20)
- milestone: M1
- next_step: none
- result: Done. ARRMiner CameraArmLength default 1700 -> 1400 (provisional, to test); no BP_Miner override (checked in the asset file); Dig and RunFlow 20 of 20 pass; docs updated (bp-miner also had an outdated 2200). RRHUDWidget.h comment about 1700 left. Look unverified.

## Subtasks
1. task-255-01-camera-arm-1400 — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-255-01-camera-arm-1400]] — step 1, cpp.
