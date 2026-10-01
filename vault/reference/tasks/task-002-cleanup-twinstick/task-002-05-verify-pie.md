---
id: task-002-05-verify-pie
description: "Check the cleaned project in the editor: L_ArenaM1 opens without errors, shows the new layout, and BP_TwinStickCharacter spawns and moves in PIE."
parent: task-002-cleanup-twinstick
kind: task
status: todo
---
# Verify in the editor and PIE

- goal: proof that the cleanup and the new layout work together in the editor.
- agent: bp
- depends_on: task-002-03-layout-rock-60, task-002-04-delete-template-cpp
- inputs: [[unreal-mcp]], [[bp-arena-grid]].
- expected_output: a report, no asset change except saving `L_ArenaM1` if the grid needs a rebuild to pick up the new layout. Checks: the editor starts on `L_ArenaM1`; the output log shows no missing asset or missing class error; HISM counts on the grid match the new layout (orchestrator gives the numbers); in PIE the pawn is a `BP_TwinStickCharacter`, spawns near (6100, 6100) on the floor, and its location changes when a movement key is held (simulated input or Slate). Record any new MCP quirk in [[unreal-mcp]].
- done_when: all checks reported with values; lint clean.
- editor_state: open
- token_ceiling: 40000
- milestone: M1
- result: open

## Relations
- [[task-002-cleanup-twinstick]] — parent global task.
