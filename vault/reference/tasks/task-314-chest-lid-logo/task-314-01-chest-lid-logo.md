---
id: task-314-01-chest-lid-logo
description: "Add the yellow Kessler-Voss mark on top of the chest lid, as on the validated sheet, and show it in game."
parent: task-314-chest-lid-logo
kind: task
status: done
---
# Yellow logo on the chest lid

- goal: Blender route from Art/Source/chest/clean/chest_lid.fbx (task-306-02): cut the yellow mark of vault/assets/chest-turnaround.webp into the lid top faces as its own material slot Logo (defaults Q1 to Q5 of the global task), keep the lid pivot on the hinge pins (task-309-07), the size and the M_BonusChest slot; FBX_SCALE_UNITS; renders top-down and three-quarter with the camera and light of the task-306 renders, next to the sheet; report with tris before and after, Logo face count and pivot check. The Orchestrator sends the renders to the owner and waits for his go.
- agent: 3d
- depends_on: task-309-07-chest-hinge-recut (same chest_lid.fbx, lid pivot moved to the hinge pins)
- inputs: agent-3d (vault/reference/agents/agent-3d.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), art-prompts-chest (vault/reference/gdd/art-audio/art-prompts-chest.md), kessler-voss-logo (vault/reference/gdd/art-audio/kessler-voss-logo.md), color-code (vault/reference/gdd/art-audio/color-code.md); files: Art/Source/chest/clean/chest_lid.fbx, chest_flat_basecolor.png, chest_cut.report.json, Tools/art/blender/chest_cut.py; vault/assets/chest-turnaround.webp
- expected_output: Art/Source/chest/clean/chest_lid.fbx with the mark, updated cut script or a logo script under Tools/art/blender/, renders top-down and three-quarter next to the sheet, report
- done_when: mark centred on the lid top as on the sheet, in its own slot Logo; lid pivot, size and other slots unchanged (numbers in the report); renders sent to the owner and his go; no Blender process left; committed
- editor_state: any
- token_ceiling: 1.2 M units
- model: sonnet+opus
- tokens: input 74, cache write 116430, cache read 2278662, output 33564
- usage: turns 37, context start 11k peak 93k, resumes 0, 8 min, units 0.54 M
- runs: 1
- milestone: M2
- started: 2026-11-05T23:11:43Z
- waiting_for: none
- result: Mark cut into the lid top band as its own slot Logo by Tools/art/blender/chest_lid_logo.py (commit faddb143): 22 x 9.7 cm centred on the band, 20 Logo faces, tris 1315 -> 1355 (+40, Q4 cap 150), pivot and bbox unchanged, M_BonusChest slot kept; renders next to the sheet in Art/Source/chest/clean/renders/logo_*.png. Owner go 2026-11-05: 'Yes, go on' (shape and size accepted). Lid-top shading streaks come from the Meshy source, not fixed.

## Relations
- [[task-314-chest-lid-logo]] — parent global task.
