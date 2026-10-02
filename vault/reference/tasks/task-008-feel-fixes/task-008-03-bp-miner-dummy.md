---
id: task-008-03-bp-miner-dummy
description: "BP_Miner weapon on the body (no hand socket), BP_TargetDummy at 160 uu standing on the floor in L_ArenaM1, grid shake defaults, save."
parent: task-008-feel-fixes
kind: task
status: todo
---
# Miner and dummy Blueprints

- goal: the Blueprint side of the feel fixes.
- agent: bp
- depends_on: task-008-02-feel-values
- inputs: [[bp-miner]], [[bp-burst-rifle]], [[bp-target-dummy]], [[bp-arena-grid]], [[sys-weapons]], [[unreal-mcp]].
- expected_output: `BP_Miner` `WeaponAttachSocketName` = None (and the attach offset from [[sys-weapons]] so the rifle sits at hip height in front of the miner); `BP_TargetDummy` mesh scale 1.6 (160 uu); the three dummies in `L_ArenaM1` moved so the sphere rests on the floor (center Z 80) at their current X and Y; `BP_ArenaGrid` shake values not overriding the C++ defaults (15 uu, 10 Hz). Doc nodes updated. Save all dirty packages (Ctrl+Shift+S if `save_assets` does not write the map's external actors) and check `git status`. Record the MCP quirks listed in the result of [[task-007-04-bp-rifle-dummy]] in [[unreal-mcp]].
- done_when: properties read back; dummies' transforms read back; files changed on disk; lint clean.
- editor_state: open
- token_ceiling: 40000
- milestone: M1
- result: open

## Relations
- [[task-008-feel-fixes]] — parent global task.
