---
id: task-230-09-rock-slab-material
description: "Material M_RockSlab and its instance: world-UV top, sides, rim, cracks, shake."
parent: task-230-rock-slab-integration
kind: task
status: todo
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
- notes (2026-10-18): implement the rigid plate tilt exactly as documented under `channels` and `material_rigid_tilt` in Art/Meshes/rock-slab/SM_RockSlabBroken.report.json (UV2 plate centre, UV3 seed and radius, vertex colour R 1 plate top / 0.5 rim, G edges and walls, B base cap), but take the per-plate random value from a world-position hash of the plate centre combined with the stored seed, so instances do not repeat the same tilt (Orchestrator, to remove the repeated pattern seen in the previews).
- result: open

## Relations
- [[task-230-rock-slab-integration]] — parent global task.
