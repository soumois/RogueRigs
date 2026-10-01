---
id: task-004-04-bp-miner
description: "Create BP_Miner, BP_RRPlayerController and BP_RRGameMode, switch L_ArenaM1 and project defaults to them, and check the miner in PIE."
parent: task-004-miner-gas
kind: task
status: todo
---
# Miner Blueprints and map switch

- goal: the arena plays with the new miner.
- agent: bp
- depends_on: task-004-03-miner-classes
- inputs: `sys-player` (created in task-004-03), [[sys-gas-core]], [[unreal-mcp]], [[cpp-bp-split]], [[blueprints-index]].
- expected_output: in `/Game/RogueRigs/Blueprints/Player/`: `BP_Miner` (parent `ARRMiner`; skeletal mesh, anim class and capsule like `BP_TwinStickCharacter`), `BP_RRPlayerController` (mapping context and input actions reused from `/Game/Variant_TwinStick/Input/`: move, mouse aim, stick aim only), `BP_RRGameMode` (default pawn `BP_Miner`, controller `BP_RRPlayerController`). `L_ArenaM1` game mode override and project `GlobalDefaultGameMode` = `BP_RRGameMode`. One doc node per Blueprint. Save the map and its external actors (save all dirty packages).
- done_when: in PIE the pawn is `BP_Miner_C` and spawns near (8100, 8100); `MaxWalkSpeed` reads 600 and the ASC reports `MoveSpeed` 600, `MaxHealth` 100, `MiningSpeed` 1 (console `AbilitySystem.DebugAttribute` or property reads); owner checks movement and aim by hand; files changed on disk (git status); lint clean.
- editor_state: open
- token_ceiling: 50000
- milestone: M1
- result: open

## Relations
- [[task-004-miner-gas]] — parent global task.
