---
id: task-229-01-pickup-simple-collisions
description: "cpp headless: simple collision primitives on SM_XPNut (box), SM_XPCubeBig (box), the ore chunk variants and the heal pickup mesh, glow disc excluded."
parent: task-229-pickup-physics
kind: task
status: done
---
# Pickup simple collisions

- goal: each pickup mesh has one simple collision primitive fitted to its body (not to the glow disc), ready for physics.
- agent: agent-cpp (headless UnrealEditor-Cmd Python, no C++ change)
- depends_on: none
- inputs: `vault/reference/tech/blueprints/fx/bp-pickup-xp.md` (SM_XPNut 30 cm across, z -5.85 to 5.85; SM_XPCubeBig body 30 x 30 x 27.5 cm; both carry a glow_disc slot that extends the bounds), `vault/reference/tech/blueprints/fx/bp-pickup-ore.md`, `vault/reference/tech/blueprints/fx/bp-pickup-heal.md` (their meshes and variants), `Art/Meshes/xp-cube-big-mesh/SM_XPCubeBig.report.json`, `vault/reference/tech/pipelines/tool-unreal-reimport.md` (companion scripts, `-ExecutePythonScript` run mode), `vault/reference/agents/art-3d-lessons.md`, `vault/reference/agents/agent-cpp.md`.
- method: editor closed. For each mesh, replace any simple collision by one primitive built from the body triangles only (exclude the glow_disc material section): XP nut = box (the owner's "rectangle"), big XP cube = box, ore chunk variants = the best fit per variant (box expected; convex hull only if a box is clearly wrong; justify), heal pickup = box or sphere by its shape (report). Collision complexity "simple as complex" not used: simple only. Save; read back each primitive (type, centre, extents). New script under `Tools/unreal/`.
- done_when: one primitive per mesh read back, sizes matching the body; commit by path.
- editor_state: closed
- token_ceiling: open
- model: opus
- tokens: input 66, cache write 103391, cache read 2097760, output 5976
- runs: 1
- milestone: M1
- result: Commit 5c4805c3: one simple primitive per mesh, glow disc excluded, read back: SM_XPNut box 30 x 25.98 x 11.7; SM_XPCubeBig box 30 x 30 x 27.5 (was 89 x 89 x 30.5 with the disc); SM_OreChunk_01 oriented box 27.1 x 12.2 x 25.8 (tilted 24 degrees); _02 convex hull 48 tris (boxes over 2x its volume); _03 box 28.3 x 14.5 x 30; heal = engine cube box (shared, unchanged). Script Tools/unreal/pickup_simple_collision.py; docs bp-pickup-xp, -ore, -heal, tool-unreal-reimport.

## Relations
- [[task-229-pickup-physics]] — parent global task.
