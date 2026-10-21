---
id: task-279-01-drone-eye-glow
description: "The miner drone camera eye glows a little, orange, through an emissive restricted to the eye texels of M_Drone."
parent: task-279-drone-eye-glow
kind: task
status: done
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
- model: sonnet
- tokens: input 90, cache write 102592, cache read 2788263, output 620
- runs: 2
- milestone: M1
- result: Commits 94dea849 (first run: colour test alone left 250 amber texels on 7 body faces, no change) and b12f7147 (retry). M_Drone emissive = Custom EyeGlowMask: amber test on linear base colour x LocalPosition box (x > 10, |y| < 3.5, -4.5 < z < 2.5) x EyeGlowColor (1, 0.45, 0.08) x EyeGlowAmount 6; 4639 eye texels, 0 non-eye texels pass, laser excluded, rust fails the colour test. Read back: before no parameters and no emissive, after EyeGlowAmount 6 and EyeGlowColor (1, 0.45, 0.08) on MI_Drone (no override), Emissive = Custom_0; recompiled, saved. Docs bp-drone-materials (revert: disconnect Emissive or EyeGlowAmount 0), color-code row. Not seen on screen.

## Relations
- [[task-279-drone-eye-glow]] — parent global task.
