---
id: task-147-xp-disc-billboard
description: "Global task: Make the glow disc always face the camera and stay centred on the pickup, in the material."
parent: tasks-done-index
kind: task
status: done
---
# XP glow disc: camera-facing and centred

- request: The glow disc of the XP nut looks off-centre (parallax: the pickup floats 30 cm above the floor and the camera is in perspective); make the disc a camera-facing billboard centred on the pickup (owner, 2026-10-13) (owner, 2026-10-14)
- goal: Make the glow disc always face the camera and stay centred on the pickup, in the material.
- acceptance: The material compiles (recompile returns without error), Radius reads 50 and 80 on the two instances, the old values (Color, Intensity, Falloff) unchanged; assets saved; doc updated; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-14).
- confirmed_by_owner: yes (2026-10-14)
- milestone: M2
- next_step: none
- result: Done. M_XPGlowDisc gets a World Position Offset making the disc a camera-facing billboard centred on the pickup (Radius parameter 50 nut, 80 chip); compiles; visual centring untested, for the owner to judge.

## Subtasks
1. task-147-01-xp-disc-billboard — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-147-01-xp-disc-billboard]] — step 1, bp.
