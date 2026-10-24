---
id: task-314-01-chest-lid-logo
description: "Add the yellow Kessler-Voss mark on top of the chest lid, as on the validated sheet, and show it in game."
parent: task-314-chest-lid-logo
kind: task
status: todo
---
# Yellow logo on the chest lid

- goal: Blender route from Art/Source/chest/clean/chest_lid.fbx (task-306-02): cut the yellow mark of vault/assets/chest-turnaround.webp into the lid top faces as its own material slot Logo (defaults Q1 to Q5 of the global task), keep the lid pivot on the hinge axis, the size and the M_BonusChest slot; FBX_SCALE_UNITS; renders top-down and three-quarter with the camera and light of the task-306 renders, next to the sheet; report with tris before and after, Logo face count and pivot check. The Orchestrator sends the renders to the owner and waits for his go.
- agent: 3d
- depends_on: none
- inputs: agent-3d (vault/reference/agents/agent-3d.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), art-prompts-chest (vault/reference/gdd/art-audio/art-prompts-chest.md), kessler-voss-logo (vault/reference/gdd/art-audio/kessler-voss-logo.md), color-code (vault/reference/gdd/art-audio/color-code.md); files: Art/Source/chest/clean/chest_lid.fbx, chest_flat_basecolor.png, chest_cut.report.json, Tools/art/blender/chest_cut.py; vault/assets/chest-turnaround.webp
- expected_output: Art/Source/chest/clean/chest_lid.fbx with the mark, updated cut script or a logo script under Tools/art/blender/, renders top-down and three-quarter next to the sheet, report
- done_when: mark centred on the lid top as on the sheet, in its own slot Logo; lid pivot, size and other slots unchanged (numbers in the report); renders sent to the owner and his go; no Blender process left; committed
- editor_state: any
- token_ceiling: 30000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-314-chest-lid-logo]] — parent global task.
