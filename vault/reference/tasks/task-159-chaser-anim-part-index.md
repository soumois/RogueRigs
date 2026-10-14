---
id: task-159-chaser-anim-part-index
description: "Make each chaser part move separately: part index carried by a UV channel."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Chaser: part index in a UV channel

- request: In game the chaser body stays rigid and tilts around one point instead of running (owner video, 2026-10-14): the material reads the part index from the vertex colours, which probably did not survive the Unreal import (the node was never verified); a UV channel is stored as exact floats and is checkable (owner, 2026-10-14)
- goal: Make the chaser animation move each part separately: carry the part index in a UV channel instead of vertex colours.
- acceptance: In game the chaser runs and strikes with its parts moving separately (legs, arms, torso), not as one rigid body; the part index is read from a UV channel of the mesh; the doc nodes updated; the owner is told what to look at.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-14).
- confirmed_by_owner: yes (2026-10-14)
- milestone: M2
- next_step: task-159-02-chaser-part-index-unreal
- result: open

## Subtasks
1. task-159-01-chaser-anim-part-index — 3d
2. task-159-02-chaser-part-index-unreal — bp (reimport, material)

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-159-01-chaser-anim-part-index]] — step 1, 3d.
- [[task-159-02-chaser-part-index-unreal]] — step 2, bp.
