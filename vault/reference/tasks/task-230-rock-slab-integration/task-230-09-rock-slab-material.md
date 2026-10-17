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
- expected_output: M_RockSlab + MI_RockSlab: top faces (normal up) sample T_RockTop by absolute world position (excluding material offsets) at two scales, sides by vertex colour G, rim tone by vertex colour R, crack mask by quadrant custom data, WPO shake from custom data, opaque, Nanite-compatible; doc node in tech/blueprints/.
- done_when: material compiles; doc node written; committed.
- editor_state: open
- token_ceiling: 60000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-230-rock-slab-integration]] — parent global task.
