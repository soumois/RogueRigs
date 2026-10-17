---
id: task-230-05-rock-crack-mask
description: "Procedural tileable crack mask (grayscale) whose value orders crack growth from 0 to 1."
parent: task-230-rock-slab-integration
kind: task
status: in-progress
---
# Procedural tileable crack mask (grayscale) whose value orders crack growth from 0 to 1

- goal: Procedural tileable crack mask (grayscale) whose value orders crack growth from 0 to 1.
- agent: 3d
- depends_on: none
- inputs: vault/reference/tech/systems/world/sys-grid/rock-state-render-options/rock-render-damage.md, vault/reference/tech/systems/world/sys-grid/rock-state-render-options/rock-render-top-texture.md, vault/reference/tech/pipelines/blender-asset-route.md, vault/reference/gdd/art-audio/readability-rules.md
- expected_output: deterministic script in `Tools/art/` (Blender or node), `Art/Textures/T_RockCrackMask.png` (tileable, same tile size as the top texture; irregular branching cracks, thick enough at game distance; mask > 1 - d shows cracks for damage d), preview strip at d 0.1, 0.3, 0.6, 0.9.
- done_when: texture tiles without seam; preview sent to the owner; committed.
- editor_state: any
- token_ceiling: 40000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-230-rock-slab-integration]] — parent global task.
