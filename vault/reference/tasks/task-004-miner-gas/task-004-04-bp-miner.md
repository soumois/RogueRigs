---
id: task-004-04-bp-miner
description: "Create BP_Miner, BP_RRPlayerController and BP_RRGameMode, switch L_ArenaM1 and project defaults to them, and check the miner in PIE."
parent: task-004-miner-gas
kind: task
status: done
---
# Miner Blueprints and map switch

- goal: the arena plays with the new miner.
- agent: bp
- depends_on: task-004-03-miner-classes
- inputs: `sys-player` (created in task-004-03), [[sys-gas-core]], [[unreal-mcp]], [[cpp-bp-split]], [[blueprints-index]].
- expected_output: in `/Game/RogueRigs/Blueprints/Player/`: `BP_Miner` (parent `ARRMiner`; skeletal mesh, anim class and capsule like `BP_TwinStickCharacter`), `BP_RRPlayerController` with new input assets in `/Game/RogueRigs/Input/`: `IA_Move`, `IA_MouseAim`, `IA_StickAim` (Axis2D) and `IMC_Miner` mapping them per the input convention in `sys-player` (WASD with Swizzle and Negate, ZQSD also bound for AZERTY, gamepad left stick for move, mouse XY for mouse aim, right stick for stick aim); if MCP cannot create them, duplicate the TwinStick ones into that folder and fix their modifiers, never edit the TwinStick originals, `BP_RRGameMode` (default pawn `BP_Miner`, controller `BP_RRPlayerController`). `L_ArenaM1` game mode override and project `GlobalDefaultGameMode` = `BP_RRGameMode`. One doc node per Blueprint. Save the map and its external actors (save all dirty packages).
- done_when: in PIE the pawn is `BP_Miner_C` and spawns near (8100, 8100); `MaxWalkSpeed` reads 600 and the ASC reports `MoveSpeed` 600, `MaxHealth` 100, `MiningSpeed` 1 (console `AbilitySystem.DebugAttribute` or property reads); owner checks movement and aim by hand; files changed on disk (git status); lint clean.
- editor_state: open
- token_ceiling: 50000
- milestone: M1
- result: BP_Miner (Quinn mesh, ABP_Unarmed, capsule 88/34), BP_RRPlayerController, BP_RRGameMode; input assets duplicated into `/Game/RogueRigs/Input/` (no MCP tool creates them) with IMC_Miner (WASD + ZQSD, sticks, Mouse2D). L_ArenaM1 and project default game mode = BP_RRGameMode. PIE: BP_Miner_C at (8100, 8100, 90), MaxWalkSpeed 600, Health 100/100, MoveSpeed 600, MiningSpeed 1, PickupRadius 300. Doc nodes bp-miner, bp-rr-player-controller, bp-rr-game-mode. MCP quirks for [[unreal-mcp]] passed to task-005-02. About 86k tokens.

## Relations
- [[task-004-miner-gas]] — parent global task.
