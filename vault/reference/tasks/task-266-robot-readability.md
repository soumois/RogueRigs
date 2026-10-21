---
id: task-266-robot-readability
description: "Global task: The robot bodies read better: the yellow is no longer burnt and the robots separate from the floor."
parent: tasks-done-index
kind: task
status: done
---
# Robot readability

- request: the robot body is very burnt yellow even without bloom: something is missing to discern it better (owner, 2026-10-21)
- goal: The robot bodies read better: the yellow is no longer burnt and the robots separate from the floor.
- acceptance: Material parameters read back with before and after values; compile OK; docs updated; lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-21).
- confirmed_by_owner: yes (2026-10-21)
- milestone: M1
- next_step: none
- result: Done. Chaser master materials M_Chaser_Meshy_Anim and M_ShieldChaser_Anim (separate copies) get Readability parameters (no CPD): BodyBrightness 0.7, BodySaturation 0.85, BodyRoughness 0.75 (floor, was fixed 0.6), cool Fresnel rim RimColor (0.25, 0.35, 0.8) RimAmount 0.35 RimPower 3 (peak emissive about 0.13), all masked off the red eye texels; logo plate cannot be separated from the body (same yellow) so it is darkened too; shield chaser emblem instance kept neutral; MI_GB_Shooter and MI_GB_Heavy colours lowered; kamikaze unchanged; animation and CPD untouched; compiles; node bp-robot-readability. Not seen on screen; owner tunes by eye.

## Subtasks
1. task-266-01-robot-readability — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-266-01-robot-readability]] — step 1, bp.
