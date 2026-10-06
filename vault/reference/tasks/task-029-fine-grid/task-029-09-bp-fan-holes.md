---
id: task-029-09-bp-fan-holes
description: "BP_Drone set up for the laser fan, PIE check of the fan and of explosion holes on the 67 uu grid next to open floor."
parent: task-029-fine-grid
kind: task
status: done
---
# Laser fan and explosion holes in PIE

- goal: the laser fan and the rounder explosion holes are checked in the editor.
- agent: bp
- depends_on: task-029-08-laser-fan
- inputs: [[bp-drone]], [[sys-drone]], [[sys-explosions]], [[sys-cheats]], [[unreal-mcp]]
- expected_output: BP_Drone properties set for the beam pool if task-029-08 lists any; PIE: miner against a wall, number of visible beams equals dug cells (read through MCP); an explosion triggered next to rock reachable from open floor (cheat, kamikaze or barrel; no layout change) and the destroyed cells counted; no log error; docs bp-drone.
- done_when: PIE check with no error; beam and hole counts reported; docs updated; lint clean.
- editor_state: open
- token_ceiling: 60000
- model: sonnet
- tokens: input 98, cache write 79144, cache read 2353522, output 29233
- runs: 2
- milestone: M2
- result: PIE: visible beams match dug cells (4/4, 3/3, 2/2, 1/1; samples not frame-exact, reads not atomic), all hidden in Orbit; no BP_Drone change. Kamikaze moved to the rock edge at (8476, 7437) exploded: 41 rock cells destroyed, exactly those with centre within 312 uu, half disc up to 9 × 5 cells, round at 67 uu. No game log error. Barrel 350 uu radius not measured. Doc bp-drone. Commit 22faf9f.

## Relations
- [[task-029-fine-grid]] — parent global task.
