---
id: task-003-02-map-update
description: "Update L_ArenaM1 for the 80 x 80 layout: PlayerStart on the new spawn, sky light without real-time capture, counts and PIE check."
parent: task-003-arena-80-paths
kind: task
status: done
---
# Map L_ArenaM1 for the 80 × 80 layout

- goal: the map shows the new layout cleanly and the player starts on its spawn.
- agent: bp
- depends_on: task-003-01-layout-80-paths
- inputs: [[unreal-mcp]], [[bp-arena-grid]], [[sys-grid]] (cell center formula).
- expected_output: in `/Game/RogueRigs/Maps/L_ArenaM1`: the grid rebuilt on the new layout (reconstruct the actor if counts are stale); PlayerStart moved above the center of the new spawn cell (formula in [[sys-grid]], CellSize 200, about 120 uu above the floor); `SkyLight` real-time capture disabled (it needs an atmosphere and shows an error in the viewport), intensity kept so the scene stays lit and the void dark. Map saved. [[bp-arena-grid]] updated if anything it states changes.
- done_when: HISM counts on the grid match the layout counts given by the orchestrator; PlayerStart location reads back at the spawn cell center; SkyLight real-time capture reads back false and the log shows no sky light error after a viewport refresh; in PIE `BP_TwinStickCharacter` spawns on the spawn cell; map saved; lint clean.
- editor_state: open
- token_ceiling: 40000
- milestone: M1
- result: grid rebuilt (floor 3729, rock 2192, heal ore 12); PlayerStart (8100, 8100, 120); SkyLight real-time capture off, intensity 0.3, no sky light error in the log; PIE spawns BP_TwinStickCharacter at (8100, 8100, 90). [[bp-arena-grid]] updated. About 40k tokens.

## Relations
- [[task-003-arena-80-paths]] — parent global task.
