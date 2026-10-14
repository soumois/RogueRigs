---
id: task-149-xp-disc-depth-cut
description: "Global task: Remove the flat cut at the bottom of the glow disc by pushing the camera-facing disc toward the camera."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# XP glow disc: flat bottom cut

- request: The bottom of the glow disc looks cut flat instead of diffuse on the nut and the chip (owner, 2026-10-13); probable cause: the camera-facing disc, centred 30 cm above the floor, dips into the floor and the depth test cuts it (owner, 2026-10-14)
- goal: Remove the flat cut at the bottom of the glow disc by pushing the camera-facing disc toward the camera.
- acceptance: DepthPush reads 60 and 90 on the instances; the material compiles; assets saved and modified in git; doc updated; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-14).
- confirmed_by_owner: yes (2026-10-14)
- milestone: M2
- next_step: task-149-01-xp-disc-depth-cut
- result: open

## Subtasks
1. task-149-01-xp-disc-depth-cut — bp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-149-01-xp-disc-depth-cut]] — step 1, bp.
