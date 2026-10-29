---
id: task-309-07-chest-hinge-recut
description: "Lid pivot on the hinge pins and open-pose mesh fixes (3d): bonus chest opening, step 07, runs first."
parent: task-309-chest-opening
kind: task
status: in-progress
---
# Lid pivot on the hinge pins and open-pose mesh fixes

- goal: Blender route from the task-306-02 parts (owner 2026-10-24: "le pivot et la pose ouverte se règlent dans 309"), fixing what the cut_open_* renders show: (1) lid origin on the axis of the hinge pins, not on the body back top edge; (2) each hinge block belongs to one part, so no hinge-block stub stays on the body back wall when the lid is open; (3) the latch long hasp no longer dangles: latch swung 90 deg outward and up about its top edge (Q9) with the lid at 100 and 115 deg (Q10, Q18), nothing hangs below the lid front edge; (4) the flat dark recess behind the latch filled with the body front wall in the body texture; (5) report the sign of the rotation about X that opens the lid up and backward and swings the latch outward and up, in the Blender frame and in Unreal Mesh space (the import mirrors Y, bp-bonus-chest orientation). Keep the size about 130 x 70 x 66 uu, body bottom-centre origin, latch origin on its top edge, slots LockLight and Interior (face counts), and about 3864 tris (accepted by the owner 2026-10-24; report any change); FBX_SCALE_UNITS; renders closed and open with the camera and light of the cut_* renders, before | after; report with pivot positions relative to the body origin; Next steps line of art-prompts-chest updated. The Orchestrator sends the renders to the owner and waits for his go.
- agent: 3d
- depends_on: none (task-306-02-chest-cut done)
- inputs: agent-3d (vault/reference/agents/agent-3d.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), art-prompts-chest (vault/reference/gdd/art-audio/art-prompts-chest.md), bp-bonus-chest (vault/reference/tech/blueprints/world/bp-bonus-chest.md); files: Art/Source/chest/clean/chest_body.fbx, chest_lid.fbx, chest_latch.fbx, chest_flat_basecolor.png, chest_cut.report.json, renders/cut_open_*.png, Tools/art/blender/chest_cut.py; vault/assets/chest-turnaround.webp, vault/assets/chest-lock-latch.png
- expected_output: Art/Source/chest/clean/chest_body.fbx, chest_lid.fbx, chest_latch.fbx updated, chest_cut.py updated, renders, chest_cut.report.json with pivots, slot face counts, tris and opening signs
- done_when: open renders show the lid turning about the pins, no stub on the body, no dangling hasp, no dark recess; pivots, size, slots, tris and signs in the report; renders sent to the owner and his go; no Blender process left; committed
- editor_state: any
- token_ceiling: 1.2 M units
- model: open
- tokens: open
- runs: open
- milestone: M2
- started: 2026-10-29T18:12:13Z
- result: open

## Relations
- [[task-309-chest-opening]] — parent global task.
