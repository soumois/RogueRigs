---
id: task-273-03-floor-material
description: "Floor material sampling the new maps by world position with a slight world-position noise against repetition, applied to BP_ArenaGrid FloorMaterial."
parent: task-273-floor-texture
kind: task
status: done
---
# Floor material with world-position noise

- goal: Textures imported; a floor material and instance: UVs = WorldPosition XY / TileSize; a low-frequency noise sampled by world position (NoiseScale about 2000-4000 uu) lerps between two samples of the same maps at another scale and rotation, and modulates the albedo value and hue by NoiseAmount (default 0.08); normal strength and roughness as parameters; applied as FloorMaterial of BP_ArenaGrid.
- agent: bp
- depends_on: task-273-02-floor-texture-maps
- inputs: bp-arena-grid (vault/reference/tech/blueprints/world/bp-arena-grid.md), sys-grid (vault/reference/tech/systems/world/sys-grid.md), rock-render-top-texture (vault/reference/tech/systems/world/sys-grid/rock-state-render-options/rock-render-top-texture.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), perf-risks (vault/reference/tech/performance/perf-risks.md)
- expected_output: Texture assets, material and instance in the floor art folder; BP_ArenaGrid FloorMaterial set; bp-arena-grid doc and a material doc updated with before and revert values; instruction count read or estimated.
- done_when: Material compiles; parameters TileSize, NoiseScale, NoiseAmount, NormalStrength, Roughness exist; FloorMaterial read back; no light, exposure or post-process touched; saved; docs updated; lint clean; committed by path.
- editor_state: open
- token_ceiling: 50k
- model: sonnet
- tokens: input 104, cache write 134870, cache read 4671108, output 3634
- runs: 2
- milestone: M1
- result: Commits 3c5a1329 (textures), a446de23 (M_FloorPlates, MI_FloorPlates, BP_ArenaGrid FloorMaterial), c39e2edf (doc bp-floor-plates-material). World-aligned UV (TileSize 800), HLSL value noise (NoiseScale 3000) blending two samples (37 deg, x0.73) and modulating value/hue (NoiseAmount 0.08, HueAmount 0.03), AlbedoBrightness 0.27 (0.35 x old floor linear luminance), NormalStrength 1, Roughness 0.8; world-space normal, Y sign disputed (Orchestrator vs bp), settled by the 04 captures; cost estimated 4 fetches, ~130-170 ALU (no stats tool). No compile error in the log.

## Relations
- [[task-273-floor-texture]] — parent global task.
