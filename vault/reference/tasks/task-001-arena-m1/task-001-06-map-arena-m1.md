---
id: task-001-06-map-arena-m1
description: "Create map L_ArenaM1 with BP_ArenaGrid, lights, a dark sky, a player start on the spawn cell and the top-down game mode."
parent: task-001-arena-m1
kind: task
status: done
---
# Map L_ArenaM1

- goal: a map the owner opens and plays to see the M1 arena.
- agent: bp
- depends_on: task-001-05-bp-arena-grid
- inputs: [[sys-grid]], [[unreal-mcp]], the `bp-arena-grid` doc node.
- expected_output: `/Game/RogueRigs/Maps/L_ArenaM1` with: one `BP_ArenaGrid` at the origin; a directional light and a sky light; a dark background (the void); one PlayerStart above the center of the spawn cell (position from `GetSpawnCell` and `GetCellCenter`, or the same formula); World Settings game mode override = the existing top-down template game mode. No other actor. The `bp-arena-grid` doc node lists this map.
- done_when: the map is saved and reopens showing the asteroid; PIE spawns the character on the spawn cell and it walks on the floor; lint clean.
- editor_state: open
- token_ceiling: 40000
- milestone: M1
- result: `L_ArenaM1` (duplicated from Lvl_TopDown, template actors removed, nav mesh included); BP_ArenaGrid at origin, PlayerStart (6100, 6100, 120), directional light, sky light 0.3, game mode `BP_TopDownGameMode`. Counts floor 2228, rock 869, heal ore 8. PIE: pawn spawns at the spawn cell and stays on the floor; walking with input not tested. About 85k tokens over two runs (turn limit hit once).

## Relations
- [[task-001-arena-m1]] — parent global task.
