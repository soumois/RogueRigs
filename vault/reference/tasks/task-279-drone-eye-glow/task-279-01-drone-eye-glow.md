---
id: task-279-01-drone-eye-glow
description: "The miner drone camera eye glows a little, orange, through an emissive restricted to the eye texels of M_Drone."
parent: task-279-drone-eye-glow
kind: task
status: todo
---
# Drone eye glow, orange

- goal: The miner drone camera eye glows a little, orange, through an emissive restricted to the eye texels of M_Drone.
- agent: bp
- depends_on: none
- inputs: bp-drone-materials (vault/reference/tech/blueprints/player/bp-drone-materials.md), bp-drone (vault/reference/tech/blueprints/player/bp-drone.md), drone (vault/reference/gdd/player/drone.md), art-prompts-drone (vault/reference/gdd/art-audio/art-prompts-drone.md), color-code (vault/reference/gdd/art-audio/color-code.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md)
- expected_output: Locate the amber eye texels in Art/Meshes/drone/drone_basecolor.png (T_Drone_BaseColor) and choose a mask that excludes the rust chips (colour plus brightness test, or a UV rectangle); report the texel count inside and outside the eye. M_Drone gets an emissive = mask x EyeGlowColor x EyeGlowAmount with scalar EyeGlowAmount 6 and vector EyeGlowColor (1, 0.45, 0.08) orange (proposed, provisional, owner tunes by eye; before: no emissive). Recompile, save, read back; bp-drone-materials updated with before, after, revert (disconnect emissive or EyeGlowAmount 0); the owner colour decision (orange eye on the player drone, a detail) recorded in color-code.
- done_when: Mask texel report; parameters read back; M_Drone compiles and is saved; docs and colour decision recorded; lint clean.
- editor_state: open (editor + MCP)
- token_ceiling: 120k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-279-drone-eye-glow]] — parent global task.
