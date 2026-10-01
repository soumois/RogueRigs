---
id: task-004-03-miner-classes
description: "Write ARRMiner (ASC, move and aim inspired by TwinStickCharacter, no dash, shot or AoE), ARRPlayerController and ARRGameMode."
parent: task-004-miner-gas
kind: task
status: todo
---
# Miner, controller, game mode

- goal: a clean C++ player built for the GDD, reusing only the TwinStick movement, aim and camera ideas.
- agent: cpp
- depends_on: task-004-02-gas-core
- inputs: [[miner]], [[player-stats]], [[sys-gas-core]], [[cpp-bp-split]], [[naming]], [[cpp-standard]]; reference only: `Source/RogueRigs/Variant_TwinStick/TwinStickCharacter.*`, `TwinStickPlayerController.*` (do not modify them).
- expected_output: under `Source/RogueRigs/Player/`: `ARRMiner : ACharacter, IAbilitySystemInterface` with its `URRAbilitySystemComponent` and the three attribute sets, base stats applied at `BeginPlay` from the CSV loader, `MoveSpeed` changes pushed to `CharacterMovement->MaxWalkSpeed`; spring arm and camera with editable pitch (−50), arm length (2200), FOV (75), arm not rotating with the pawn; move (world-space 2D input) and aim (mouse cursor projected on the floor plane, or right stick) rotating the pawn toward the aim; no dash, shot, AoE, pickup or item logic. `ARRPlayerController`: Enhanced Input mapping context and actions as editable properties (assets assigned in Blueprint), mouse cursor shown, input routed to the miner. `ARRGameMode`: default pawn and controller classes set in Blueprint. New doc node `sys-player` in tech/systems (template `system.md`), linked from [[systems-index]].
- done_when: build exits 0 with the editor closed; no new warnings in touched files; `Variant_TwinStick` unchanged (git diff); `sys-player` matches the code; lint clean.
- editor_state: closed
- token_ceiling: 80000
- milestone: M1
- result: open

## Relations
- [[task-004-miner-gas]] — parent global task.
