---
id: task-179-01-debris-debug-plane-off
description: "Remove the red debug square drawn on the floor under each active debris system (NS_RobotDebris collision plane debug draw)."
parent: task-179-debris-debug-plane-off
kind: task
status: done
---
# Robot debris: hide the red debug plane

- goal: Remove the red debug square drawn on the floor under each active debris system (NS_RobotDebris collision plane debug draw).
- agent: bp
- depends_on: none
- inputs: bp-robot-debris (vault/reference/tech/blueprints/fx/bp-robot-debris.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md)
- expected_output: In /Game/RogueRigs/FX/NS_RobotDebris: the debug drawing of the Collision module (Debug Plane / Debug Mode / DebugDraw inputs inherited from the UpwardMeshBurst template) and any other debug draw turned off; if the square does not come from this system, stop, change nothing and report what draws it; bp-robot-debris updated
- done_when: The debug inputs read back off; compiles with no stack issue; saved by explicit path; a -game run with 1 or 2 rr.Debug.Shots right after a chaser death shows no red square on the floor (images described); doc updated; committed by path; the editor stays open
- editor_state: open
- token_ceiling: open
- model: sonnet
- tokens: input 40, cache write 105254, cache read 1315529, output 8256
- runs: 1
- milestone: M3
- result: Collision module Analytical Plane Debug Mode 1 -> 0 (template default drew a red 100x100 plane at each system); NS_RobotDeathPuff has no debug draw; compiles clean; -game shots Saved/Debris179: debris visible, no red square; quirk noted; commit d3928f5

## Relations
- [[task-179-debris-debug-plane-off]] — parent global task.
