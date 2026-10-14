---
id: task-161-chaser-cut-pose-check
description: "Global task: Check the part cut of the chaser and render four fixed run poses from the baked animation data."
parent: tasks-done-index
kind: task
status: done
---
# Chaser: cut check and fixed poses

- request: In game the chaser animation falls apart while it runs (owner, 2026-10-14); the owner asks first to be sure that the parts were cut correctly, then for images of 3 or 4 fixed poses, to separate a data problem from an Unreal problem (owner, 2026-10-14)
- goal: Check the part cut of the chaser and render four fixed run poses from the baked animation data.
- acceptance: The script runs headless; the report states the anomalies with numbers and no verdict; the pose images exist (front and side, 4 poses each) from the EXR path and from the keyed path; the Orchestrator and the owner judge the images; nothing imported in Unreal; the script and the report committed by name, images untracked; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-14).
- confirmed_by_owner: yes (2026-10-14)
- milestone: M2
- next_step: none
- result: Done. cut check: 10 parts single component except ShinFoot_L (18 tri sole fragment); EXR path equals keyed path within 0.034 cm; 4 fixed run poses rendered

## Subtasks
1. task-161-01-chaser-cut-pose-check — 3d

## Relations
- [[tasks-done-index]] — parent index.
- [[task-161-01-chaser-cut-pose-check]] — step 1, 3d.
