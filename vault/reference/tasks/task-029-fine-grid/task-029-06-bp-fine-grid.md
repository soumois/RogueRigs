---
id: task-029-06-bp-fine-grid
description: "Block meshes and materials scaled to 67 uu, cracks and break debris readable at the new size, drone arc dig visual, PIE."
parent: task-029-fine-grid
kind: task
status: done
---
# Fine grid visuals

- goal: the fine grid looks organic.
- agent: bp
- depends_on: task-029-05-arc-mining
- inputs: [[bp-arena-grid]], [[bp-drone]], [[visual-style]], [[color-code]], [[unreal-mcp]], [[sys-cheats]]
- expected_output: block meshes and materials at 67 uu (slight random rotation or bevel per instance if available for an organic look), marked-rock outline still visible, drone laser fan over the arc, PIE: dig a wall by contact, explosions carve rounder holes, no log error; re-save L_ArenaM1 so the old template-component instances are dropped (grid chunks rebuild at load); docs.
- done_when: PIE check with no error; docs updated; lint clean.
- editor_state: open
- token_ceiling: 110000
- model: sonnet
- tokens: input 76, cache write 71604, cache read 1937998, output 18100
- runs: 1
- milestone: M2
- result: M_GreyBoxMarked: per-instance shade 0.85–1.15 and darkened face border (seam/bevel look) for rock, ore, heal and special ore; floor and barrel unchanged; marked-rock pulse kept. PIE: miner teleported against rock at (114, 88), drone Dig then Orbit after about 1.6 s, LaserMesh visible, no log error. L_ArenaM1 re-saved. Not done: laser fan (needs a C++ beam pool in ARRDrone), per-instance yaw (needs C++ in ARRArenaGrid), explosion holes not seen in PIE (barrels enclosed in rock). Docs bp-arena-grid, bp-drone. Commit ed5f893.

## Relations
- [[task-029-fine-grid]] — parent global task.
