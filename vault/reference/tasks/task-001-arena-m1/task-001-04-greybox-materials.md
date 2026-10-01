---
id: task-001-04-greybox-materials
description: "Create the grey-box base material with a color parameter and one material instance per arena type, after the color code."
parent: task-001-arena-m1
kind: task
status: done
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
- result: `M_GreyBox` (Color, Roughness 0.8) and five instances in `/Game/RogueRigs/Materials/GreyBox/`; linear RGB floor (0.045, 0.038, 0.032), rock (0.45, 0.40, 0.55), heal ore (0.80, 0.02, 0.02), ore (0.30, 0.05, 0.75), special ore white. [[unreal-mcp]] tool inventory filled. About 55k tokens (over the 40k ceiling, mostly tool discovery).

## Relations
- [[task-001-arena-m1]] — parent global task.
