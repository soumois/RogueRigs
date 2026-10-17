---
id: task-230-05-rock-crack-mask
description: "Procedural tileable crack mask (grayscale) whose value orders crack growth from 0 to 1."
parent: task-230-rock-slab-integration
kind: task
status: done
---
# Procedural tileable crack mask (grayscale) whose value orders crack growth from 0 to 1

- goal: Procedural tileable crack mask (grayscale) whose value orders crack growth from 0 to 1.
- agent: 3d
- depends_on: none
- inputs: vault/reference/tech/systems/world/sys-grid/rock-state-render-options/rock-render-damage.md, vault/reference/tech/systems/world/sys-grid/rock-state-render-options/rock-render-top-texture.md, vault/reference/tech/pipelines/blender-asset-route.md, vault/reference/gdd/art-audio/readability-rules.md
- owner_correction (2026-10-17, after the first run 132e0612 and the DRGS capture drgs_damage_shards.webp): the mask is for light damage only; cracks fine, straight and angular, few of them, drawn lighter than the top (not dark), about 15-20 % coverage at d 0.6; heavy damage is the 3D shard cluster of task-230-06.
- expected_output: deterministic script in `Tools/art/` (Blender or node), `Art/Textures/T_RockCrackMask.png` (tileable, same tile size as the top texture; irregular branching cracks, thick enough at game distance; mask > 1 - d shows cracks for damage d), preview strip at d 0.1, 0.3, 0.6, 0.9.
- done_when: texture tiles without seam; preview sent to the owner; committed.
- editor_state: any
- token_ceiling: 40000
- model: sonnet
- tokens: input 30, cache write 46970, cache read 338383, output 850
- runs: 2
- milestone: M2
- result: T_RockCrackMask.png 1024 seamless, make_rock_crack_mask.js seed 230052 (commit e0d0424e; first run 132e0612 rejected: twig-like, too dense); straight angular segments, branches meet into plates, 3 px light lines, coverage 0.7 % at d 0.05, 13.4 % at 0.6; validated by the owner 2026-10-17; tile repetition to hide in the material (UV offset or rotation)

## Relations
- [[task-230-rock-slab-integration]] — parent global task.
