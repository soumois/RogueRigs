---
id: task-332-01-space-material
description: "Build M_SpaceBackground, an Unlit procedural space material, and its three instances for the star, nebula and dust layers."
parent: task-332-space-background
kind: task
status: in-progress
---
# Space background: procedural material

- goal: one master material M_SpaceBackground with three instances (MI_Space_Stars, MI_Space_Nebula, MI_Space_Dust) that draw procedural stars, nebula and dust from world-position noise, compiled and saved, no texture.
- agent: bp
- depends_on: none
- inputs: the global `request` and `gdd_check` with the owner answers to Q1 to Q9 (vault/reference/tasks/task-332-space-background.md); bp-floor-plates-material (vault/reference/tech/blueprints/world/bp-floor-plates-material.md, the HLSL value-noise method and conventions to reuse); bp-rock-slab-material (vault/reference/tech/blueprints/world/bp-rock-slab-material.md, folder and Custom HLSL conventions); mpp-player-light-mask (vault/reference/tech/blueprints/world/mpp-player-light-mask.md, the darkener that multiplies the background); ppv-xp-glow (vault/reference/tech/blueprints/world/ppv-xp-glow.md, fixed exposure and bloom); color-code (vault/reference/gdd/art-audio/color-code.md); readability-rules (vault/reference/gdd/art-audio/readability-rules.md); bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md).
- expected_output: in /Game/RogueRigs/Environment/Space/: M_SpaceBackground (shading model Unlit; a static switch or blend parameter choosing Opaque stars or Additive nebula and dust is allowed, or two masters if the blend mode cannot be switched, still one subtask; no world position offset; no parallax occlusion mapping); noise from world XY (Custom HLSL, value noise as in M_FloorPlates, no texture fetch) so each plane shows a seamless pattern at its own depth; stars: thresholded cell noise, size, density and twinkle speed as parameters; nebula: 2 to 3 octaves, slow flow by Time, colour ramp of 2 to 3 hues per Q3; dust: sparse faint specks; one Intensity scalar per instance; instances MI_Space_Stars, MI_Space_Nebula, MI_Space_Dust with provisional values per Q3 and Q4. Change summary for the doc subtask: asset paths, parameter table with defaults, blend mode per instance, estimated instruction count, commit hash. No vault doc node.
- done_when: the three instances compile with no log error and are saved; parameters read back from each instance; blend mode per instance read back (stars opaque, nebula and dust additive, per Q2); no texture sampler in the graph; estimated pixel shader cost stated; committed by explicit paths.
- editor_state: open
- token_ceiling: 1.0 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M3
- started: 2026-10-27T21:27:42Z
- waiting_for: none
- result: open

## Relations
- [[task-332-space-background]] — parent global task.
