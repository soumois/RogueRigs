---
id: task-001-04-greybox-materials
description: "Create the grey-box base material with a color parameter and one material instance per arena type, after the color code."
parent: task-001-arena-m1
kind: task
status: in-progress
---
# Grey-box materials

- goal: colored test materials for the arena, made in the editor.
- agent: bp
- depends_on: none
- inputs: [[color-code]], [[unreal-mcp]].
- expected_output: `/Game/RogueRigs/Materials/GreyBox/M_GreyBox` (opaque, vector parameter `Color`, scalar parameter `Roughness`) and, in the same folder: `MI_GB_Floor` dark warm grey (provisional, not in the GDD); `MI_GB_Rock` light grey tinted violet; `MI_GB_HealOre` saturated red; `MI_GB_Ore` violet; `MI_GB_SpecialOre` placeholder white (look open, unused by this layout). No barrel material. The report lists the tools the MCP server exposes; record that inventory in [[unreal-mcp]] (currently open).
- done_when: the six assets exist and are saved; the [[unreal-mcp]] tool inventory is filled; lint clean.
- editor_state: open
- token_ceiling: 40000
- milestone: M1
- result: open

## Relations
- [[task-001-arena-m1]] — parent global task.
