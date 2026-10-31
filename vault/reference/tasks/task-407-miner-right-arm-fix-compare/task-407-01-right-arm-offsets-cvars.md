---
id: task-407-01-right-arm-offsets-cvars
description: "Cpp: URRMinerAnimInstance: RightElbowOffset, RightHandOffset, debug cvars MinerArmFix and MinerAnimFreeze"
parent: task-407-miner-right-arm-fix-compare
kind: task
status: in-progress
---
# Miner right arm correction on the idle pose, compared on one frame: cpp

- goal: URRMinerAnimInstance exposes RightElbowOffset and RightHandOffset (FVector cm, EditAnywhere, BlueprintReadOnly, category Animation|RightArm, defaults Q6) and two debug cvars: rr.Debug.MinerArmFix "<elbowUp> <handForward>" (overrides the offsets with (0, 0, elbowUp) and (0, handForward, 0) while set; empty = off) and rr.Debug.MinerAnimFreeze (1 stops the miner animation time, for example GlobalAnimRateScale 0 on the mesh, while the graph keeps evaluating so a new offset shows on the same frame; 0 restores).
- agent: cpp
- depends_on: none
- inputs: sys-player-classes (vault/reference/tech/systems/core/sys-player/sys-player-classes.md), bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), tool-capture (vault/reference/tech/pipelines/tool-capture.md), Source/RogueRigs/Player/RRMinerAnimInstance.h, Source/RogueRigs/Player/RRMinerAnimInstance.cpp
- expected_output: Change summary: properties, cvars and their help text, build result, commit hash. No vault doc edit.
- done_when: Properties and cvars as in the goal, help texts listed by `node Tools/capture/commands.js MinerA` / `MinerArm`; RogueRigsEditor builds without warnings from the change; game rules untouched; committed by path `task-407-01-right-arm-offsets-cvars: ...`.
- editor_state: closed
- token_ceiling: 0.8 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-31T07:17:00Z
- waiting_for: none
- result: open

## Relations
- [[task-407-miner-right-arm-fix-compare]] — parent global task.
