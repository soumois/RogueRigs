---
id: task-191-01-debris-gpu-df-collision
description: "Switch the NS_RobotDebris emitter to GPU simulation and make the pieces bounce off walls with distance field collision, all current behaviour kept."
parent: task-191-debris-wall-collision
kind: task
status: done
---
# Robot debris: GPU sim and wall collision

- goal: Switch the NS_RobotDebris emitter to GPU simulation and make the pieces bounce off walls with distance field collision, all current behaviour kept.
- agent: bp
- depends_on: none
- inputs: bp-robot-debris (vault/reference/tech/blueprints/fx/bp-robot-debris.md), enemy-death (vault/reference/gdd/enemies/enemy-death.md), sys-grid (vault/reference/tech/systems/world/sys-grid.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md)
- expected_output: NS_RobotDebris emitter Debris: GPU compute sim, fixed bounds large enough for a 15 m throw plus margin (no dynamic bounds on GPU); every existing module and expression checked to compile and behave on GPU (array DIs User.PieceMeshIndex, burst count from the array length, MeshIndex, PieceLocal table, two-step position with Z clamp, throw with Emitter.ThrowScale and ±15 degrees, spin damping with PrevVelZ, sink); wall collision with the Collision module in distance field mode (mesh distance fields of the grid, already on), restitution and friction as the floor; the analytic floor plane kept for the floor; both collisions disabled from age 4.5 s so the sink still goes through the floor; one-shot loop settings kept; bp-robot-debris updated; quirks found added to unreal-mcp-quirks
- done_when: Compiles on GPU with no stack issue; saved by explicit path; a -game run (fx.Niagara.DumpComponents values, plus 1 or 2 rr.Debug.Shots near a wall, described) shows pieces spawning at the robot, thrown, stopping or bouncing at a wall instead of entering it, resting on the floor and sinking at 4.5 s; docs updated; committed by path; editor stays open
- editor_state: open
- token_ceiling: open
- model: sonnet
- tokens: input 124, cache write 160531, cache read 5238111, output 39372
- runs: 2
- milestone: M3
- result: GPU compute sim, fixed bounds ±2000/-200..1500, Collision GPU distance fields (walls and floor; analytic plane impossible on GPU, one mode), collision off from age 4.5 s; compiles clean; clean -game check (1 chaser, slomo 0.25, timed shots): pieces spawn at the robot, stop at the wall foot (none inside), rest on the floor 1.3-4.5 s, sink 4.5-5 s; wall bounce vs stop not distinguished; commits bda304fb, 68da4286, cf37ff06; validated on screen by the owner: pieces stop at the walls (2026-10-15)

## Relations
- [[task-191-debris-wall-collision]] — parent global task.
