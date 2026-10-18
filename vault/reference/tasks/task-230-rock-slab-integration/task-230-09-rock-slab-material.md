---
id: task-230-09-rock-slab-material
description: "Material M_RockSlab and its instance: world-UV top, sides, rim, cracks, shake."
parent: task-230-rock-slab-integration
kind: task
status: in-progress
---
# Material M_RockSlab and its instance: world-UV top, sides, rim, cracks, shake

- goal: Material M_RockSlab and its instance: world-UV top, sides, rim, cracks, shake.
- agent: bp
- depends_on: task-230-08-rock-damage-data
- inputs: vault/reference/tech/systems/world/sys-grid/rock-state-render-options/rock-render-top-texture.md, vault/reference/tech/systems/world/sys-grid/rock-state-render-options/rock-render-damage.md, vault/reference/tech/systems/world/sys-grid/rock-state-render-options/rock-render-piece-set.md, vault/reference/agents/bp-mcp-lessons.md, vault/reference/gdd/art-audio/color-code.md; textures from the 03 and 05 results
- expected_output: M_RockSlab + MI_RockSlab: top faces (normal up) sample T_RockTop by absolute world position (excluding material offsets) at two scales, sides by vertex colour G, rim tone by vertex colour R, crack mask by quadrant custom data, WPO shake from custom data, WPO sag of the broken variants (bilinear corner damage x distance toward the empty side, top never below 60 uu, rock-render-damage), opaque, Nanite-compatible; top colour desaturated about 50-60 % and slightly brightened, tint and saturation as instance parameters (owner 2026-10-17, review 230-04 colour-code fail); doc node in tech/blueprints/.
- done_when: material compiles; doc node written; committed.
- editor_state: open
- token_ceiling: 60000
- model: open
- tokens: open
- runs: open
- milestone: M2
- notes (2026-10-18, updated for 06 version 4 c0ac9903): the broken variants are closed prisms (blocks); move each block rigidly exactly as documented under `channels` and `material_rigid_tilt` in Art/Meshes/rock-slab/SM_RockSlabBroken.report.json (UV2 block centre = pivot, UV3 seed and radius, UV4 rim flag and block index; vertex colour R 1 block top / 0.5 rim, G sides, B bottoms): sink toward the empty side, small tilt, random offset, top never below 60 uu; take the per-block random value from a world-position hash of the block centre combined with the stored seed, so instances do not repeat the same pattern (Orchestrator).
- result: open

## Relations
- [[task-230-rock-slab-integration]] — parent global task.
