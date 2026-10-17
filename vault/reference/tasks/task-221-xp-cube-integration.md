---
id: task-221-xp-cube-integration
description: "Replace the big XP chip by the validated data cube in game (BP_PickupXPBig), any-angle rest, then delete the chip assets and sources."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Big XP: data cube replaces the chip

- request: "Tu peux recommencer et en même temps intégrer le nouveau cube XP qui remplace la puce XP (et clean celle-ci lorsque c'est fait)." (owner 2026-10-17)
- goal: BP_PickupXPBig shows SM_XPCubeBig with the chip's glow behaviour, rests in any orientation; the chip mesh, its materials, its FBX and scripts are removed.
- acceptance: BP read back with the cube; a `-game` capture of big XP cubes on the floor sent to the owner; no asset or file of the chip left; docs updated.
- gdd_check: [[art-prompts-xp-core]] (cube validated, task-217), [[bp-pickup-xp]] (chip slots, glow disc, scale 1.53, RestOrientation YawOnly), [[color-code]] (big XP core lilac versus magenta mismatch still open: the cube keeps the chip's current core and disc instances, so the in-game hue does not change). Defaults (Orchestrator): scale 1.53 kept (the cube is 30 cm across like the chip); RestOrientation Full (the reason for the cube).
- confirmed_by_owner: yes
- milestone: M1
- next_step: task-221-03-xp-chip-cleanup
- result: open

## Subtasks
- [[task-221-01-xp-cube-import]] — bp: import SM_XPCubeBig and its material instances.
- [[task-221-02-bp-pickup-xp-big-cube]] — bp: BP_PickupXPBig uses the cube, RestOrientation Full, doc node.
- [[task-221-03-xp-chip-cleanup]] — bp: delete the chip assets once unreferenced; delete its FBX and scripts; docs.
- [[task-221-04-xp-cube-capture]] — cpp: `-game` capture of big XP cubes.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[art-prompts-xp-core]] — the cube.
- [[bp-pickup-xp]] — the Blueprint changed.
