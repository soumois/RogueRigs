---
id: task-335-03-space-texture-material
description: "Import the two space textures and build M_SpaceTexture (Unlit additive, world-XY UV, slow drift) with MI_Space_Deep and MI_Space_Clouds."
parent: task-335-space-texture-layers
kind: task
status: todo
---
# Space layers: texture material and instances

- goal: one Unlit material that draws a tileable space texture on a plane with world-XY UV, tint, intensity and slow drift, contrast and saturation, and one instance per layer tuned to the B1 band.
- agent: bp
- depends_on: task-335-02-space-textures
- inputs: the global `request` and `gdd_check` with S3 to S7 and B1 to B5 (vault/reference/tasks/task-335-space-texture-layers.md); the task-335-02 result (texture paths); bp-space-background-material (vault/reference/tech/blueprints/world/bp-space-background-material.md, the procedural master and its finding: what not to change); bp-floor-plates-material (vault/reference/tech/blueprints/world/bp-floor-plates-material.md, world-aligned texture sampling); mpp-player-light-mask (vault/reference/tech/blueprints/world/mpp-player-light-mask.md); ppv-xp-glow (vault/reference/tech/blueprints/world/ppv-xp-glow.md, fixed exposure, bloom); color-code (vault/reference/gdd/art-audio/color-code.md); bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md).
- expected_output: in /Game/RogueRigs/Environment/Space/: T_Space_Deep and T_Space_Clouds (sRGB, mips, Wrap); M_SpaceTexture: Unlit, Additive, TwoSided, no shadow, emissive = texture x Tint x Intensity, UV = world XY / TileSize + Time x PanVelocity, parameters Texture, Tint, Intensity, Contrast, Saturation, TileSize, PanVelocity (X, Y); MI_Space_Deep (TileSize about 20000, drift about 5 uu/s) and MI_Space_Clouds (TileSize about 12000, drift about 12 uu/s, another direction) per S4 and S5; Intensity, Contrast and Saturation first set by arithmetic for a cloud peak near 85 sRGB luma (B1 band 70 to 100) under the void far gain 0.5 that task-335-05 gives (state it); the void mean must stay below the floor mean beside the edge. M_SpaceBackground and its three instances unchanged. Change summary for the doc subtask: assets, parameters, values, arithmetic, commit hash. No vault doc node.
- done_when: material and instances compile with no log error and are saved; parameters read back; M_SpaceBackground, MI_Space_Stars, MI_Space_Nebula and MI_Space_Dust show no change in `git status`; committed by explicit path.
- editor_state: open
- token_ceiling: 1.0 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M3
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-335-space-texture-layers]] — parent global task.
