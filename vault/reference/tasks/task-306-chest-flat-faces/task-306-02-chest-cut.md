---
id: task-306-02-chest-cut
description: "Cut the chest into body, lid and latch (3d): bonus chest mesh with flat faces, step 02."
parent: task-306-chest-flat-faces
kind: task
status: done
---
# Cut the chest into body, lid and latch

- goal: From chest_flat: separate the lid (origin on the hinge axis, back top edge), the front lock latch of vault/assets/chest-lock-latch.png (origin on its top edge) and the body (bottom-centre origin); the red light faces go to their own material slot LockLight on the latch, the inner faces of the body (seen when the lid is open) to their own slot Interior (for the task-309 inner light), the rest keeps the texture; keep the chest_flat size, about 130 x 70 x 66 uu (owner 2026-10-24: "On garde ces proportions"), so it fits the 2 x 2 footprint (134 uu), lock face toward the camera (Q3); three FBX with FBX_SCALE_UNITS; report with sizes, pivot positions relative to the body origin and slot face counts; renders closed and with lid and latch rotated open; Next steps line of art-prompts-chest updated.
- agent: 3d
- depends_on: task-306-01-chest-flatten
- inputs: agent-3d (vault/reference/agents/agent-3d.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), mesh-fidelity-loop (vault/reference/tech/pipelines/mesh-fidelity-loop.md), art-prompts-chest (vault/reference/gdd/art-audio/art-prompts-chest.md), bp-bonus-chest (vault/reference/tech/blueprints/world/bp-bonus-chest.md), sys-objectives-console-chest (vault/reference/tech/systems/world/sys-objectives/sys-objectives-console-chest.md); files: Art/Source/chest/clean/chest_flat.fbx; vault/assets/chest-lock-latch.png; vault/assets/chest-turnaround.webp
- expected_output: Art/Source/chest/clean/chest_body.fbx, chest_lid.fbx, chest_latch.fbx, texture, script, renders, report
- done_when: three FBX; pivots on the hinge axis and the latch top edge (numbers in the report); LockLight slot on the latch; Interior slot on the body; renders sent to the owner; no Blender process left; committed
- editor_state: any
- token_ceiling: 40000
- model: sonnet
- tokens: input 88, cache write 134044, cache read 3557947, output 62501
- runs: 1
- milestone: M2
- result: chest_body.fbx 2110 tris (M_BonusChest 1809, Interior 301, cavity 22 cm deep), chest_lid.fbx 1326 tris (pivot on the hinge axis 0, 31.137, 41.25 cm from the body origin), chest_latch.fbx 428 tris (LockLight 9 faces, pivot on its top edge -0.475, -24.149, 51.488; child of the lid); assembled 130 x 69.8 x 65.6 uu; total 3864 tris (caps and cavity, over the 3000 of Q2, accepted by the owner 2026-10-24); lid origin on the body back top edge, not on the hinge pins, moved by task-309-07; script Tools/art/blender/chest_cut.py, report chest_cut.report.json, renders cut_closed_*, cut_latchout_*, cut_open_*.

## Relations
- [[task-306-chest-flat-faces]] — parent global task.
