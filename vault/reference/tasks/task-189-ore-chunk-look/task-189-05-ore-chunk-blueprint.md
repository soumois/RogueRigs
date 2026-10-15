---
id: task-189-05-ore-chunk-blueprint
description: "BP: BP_PickupOre uses SM_OreChunk, its materials and the NS_OreTrail trail activated on magnet; doc bp-pickup-ore rewritten."
parent: task-189-ore-chunk-look
kind: task
status: done
---
# Ore chunk: Blueprint wiring

- goal: BP_PickupOre: one of the three meshes SM_OreChunk_01..03 per dropped chunk (random variant per drop and rest orientation Full through the mechanism of task-192-01), PickupMesh at scale 1 with its material overrides, no shadow (as the XP nut), a TrailFX Niagara component (NS_OreTrail, auto activate off, relative scale so its effective size matches the XP nut trail) activated in OnMagnetStart with the same look-at as BP_PickupXP; pop distances and C++ defaults unchanged; sys-pickups gotcha on the ore trail updated; bp-pickup-ore rewritten.
- agent: bp
- depends_on: task-189-03-ore-chunk-materials, task-189-04-ore-chunk-trail, task-192-01-pickup-spawn-orientation-variants
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), bp-pickup-ore (vault/reference/tech/blueprints/fx/bp-pickup-ore.md), bp-pickup-xp (vault/reference/tech/blueprints/fx/bp-pickup-xp.md), sys-pickups (vault/reference/tech/systems/world/sys-pickups.md), bp-arena-grid (vault/reference/tech/blueprints/world/bp-arena-grid.md)
- expected_output: BP_PickupOre saved by explicit path; doc nodes bp-pickup-ore and sys-pickups updated.
- done_when: Blueprint compiles with warnings as errors; defaults read back (mesh, materials, TrailFX asset, CastShadow false); BP_ArenaGrid still references it as OrePickupClass; docs updated; lint clean; committed by path. On-screen look is the owner's to judge (no screenshot run).
- editor_state: open
- token_ceiling: open
- model: sonnet
- tokens: input 50, cache write 64028, cache read 1083720, output 400
- runs: 1
- milestone: M1
- result: BP_PickupOre: SM_OreChunk_01 at scale 1, OverrideMaterials cleared (slot defaults MI_OreChunk_Bright/Dark), RestOrientation Full, MeshVariants 01-03, CastShadow false, TrailFX NS_OreTrail (scale 0.3, auto activate off) activated on OnMagnetStart, no look-at (Orchestrator choice, keeps the random orientation); BP_ArenaGrid OrePickupClass unchanged; compiled and saved; docs updated; look, trail and runtime draw not seen in PIE; commit 8cf8486b

## Relations
- [[task-189-ore-chunk-look]] — parent global task.
