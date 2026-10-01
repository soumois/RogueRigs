---
id: task-005-02-bp-drone
description: "Create BP_Drone (40 uu grey-box sphere, white and teal material), assign it as the miner's drone class, and check the orbit in PIE."
parent: task-005-drone-orbit
kind: task
status: in-progress
---
# BP_Drone

- goal: the drone visible in the arena.
- agent: bp
- depends_on: task-005-01-drone-cpp
- inputs: `sys-drone`, [[drone]], [[color-code]], [[unreal-mcp]], [[blueprints-index]].
- expected_output: `/Game/RogueRigs/Materials/GreyBox/MI_GB_Drone` (instance of `M_GreyBox`, teal tint after [[color-code]]); `/Game/RogueRigs/Blueprints/Player/BP_Drone` (parent `ARRDrone`, mesh `/Engine/BasicShapes/Sphere` scaled to 40 uu, `MI_GB_Drone`); `BP_Miner` `DroneClass` = `BP_Drone`. Doc nodes `bp-drone` (new) and `bp-miner` (updated). Save all dirty packages.
- note: MCP quirks to add to [[unreal-mcp]] (from task-004-04): no tool creates Enhanced Input assets (duplicate existing ones); `set_properties` on an array that changes size and elements at once fails, set it to `[]` first; an instanced subobject is created with `{"refPath": "/Script/Module.Class"}` (the `{"instance": ...}` form gives None); PIE subobject paths use `.` (e.g. `BP_Miner_C_0.CharMoveComp`); `compile_blueprint` returns null on success.
- done_when: in PIE one `BP_Drone_C` exists; two location reads 0.5 s apart differ; its distance to the miner in the floor plane is about 150 uu and its height about 150 uu above the floor; files changed on disk; lint clean.
- editor_state: open
- token_ceiling: 40000
- milestone: M1
- result: open

## Relations
- [[task-005-drone-orbit]] — parent global task.
