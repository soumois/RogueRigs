---
id: task-408-01-remove-arm-offsets-upper-pose-cvar
description: "Cpp: undo task-407 (C++ offsets, MinerArmFix, ABP_Miner restored to b03e520d), keep MinerAnimFreeze, add rr.Debug.MinerUpperPose"
parent: task-408-miner-right-arm-in-animation
kind: task
status: done
---
# Miner right arm corrected in the animation, variants compared on one frame: cpp

- goal: The task-407 ABP correction and C++ offsets are gone and a debug cvar swaps the upper body sequence live (Q4, Q5).
- agent: cpp
- depends_on: none
- inputs: sys-player-classes (vault/reference/tech/systems/core/sys-player/sys-player-classes.md), bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), task-407-01-right-arm-offsets-cvars (vault/reference/tasks/task-407-miner-right-arm-fix-compare/task-407-01-right-arm-offsets-cvars.md), Source/RogueRigs/Player/RRMinerAnimInstance.h, Source/RogueRigs/Player/RRMinerAnimInstance.cpp, Commits: 32c5c0ce (407-01 C++ to undo except rr.Debug.MinerAnimFreeze), fc07fb0d (407-02 ABP_Miner change to undo), b03e520d (task-406 ABP_Miner to restore)
- expected_output: Change summary: removed members and cvar, new cvar and help text, ABP_Miner restore, build result, commit hash. No vault doc edit.
- done_when: Editor closed (check the process first; if open, stop and report). `git diff b03e520d -- Content/RogueRigs/Characters/Miner/ABP_Miner.uasset` is empty after `git checkout b03e520d -- Content/RogueRigs/Characters/Miner/ABP_Miner.uasset`; RightElbowOffset, RightHandOffset, AppliedRightElbowOffset, AppliedRightHandOffset and rr.Debug.MinerArmFix no longer in Source; rr.Debug.MinerAnimFreeze unchanged; rr.Debug.MinerUpperPose "<name>" loads /Game/RogueRigs/Characters/Miner/Anims/<name> as UAnimSequenceBase and sets UpperBodySequence on the player miner anim instance, empty restores the class default, a missing asset logs a warning; `node Tools/capture/commands.js Miner` lists MinerAnimFreeze and MinerUpperPose, not MinerArmFix; RogueRigsEditor builds without warnings from the change; Source files and ABP_Miner.uasset committed by path `task-408-01-remove-arm-offsets-upper-pose-cvar: ...`.
- editor_state: closed
- token_ceiling: 0.6 M units
- model: opus
- tokens: input 28, cache write 58311, cache read 506394, output 11105
- usage: turns 14, context start 12k peak 58k, resumes 0, 2 min, units 0.18 M
- runs: 1
- milestone: M2
- started: 2026-10-31T07:41:29Z
- waiting_for: none
- result: Done, commit 4e0cce39: 407 offsets and MinerArmFix removed, ABP_Miner restored to b03e520d, MinerAnimFreeze kept (help text reworded), rr.Debug.MinerUpperPose added; build ok; not run in game

## Relations
- [[task-408-miner-right-arm-in-animation]] — parent global task.
