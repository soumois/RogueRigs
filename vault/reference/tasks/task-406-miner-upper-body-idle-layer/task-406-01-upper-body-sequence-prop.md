---
id: task-406-01-upper-body-sequence-prop
description: "Cpp: URRMinerAnimInstance: UpperBodySequence property, the one swappable source of the upper body"
parent: task-406-miner-upper-body-idle-layer
kind: task
status: done
---
# Miner upper body from the idle rifle pose over the locomotion legs: cpp

- goal: URRMinerAnimInstance exposes UpperBodySequence (UAnimSequenceBase*, EditDefaultsOnly, BlueprintReadOnly, category Animation|UpperBody, header comment: source of the upper body layer in ABP_Miner, swap it for another weapon pose), no default asset in C++ (set in ABP_Miner class defaults), no other logic (Q2).
- agent: cpp
- depends_on: none
- inputs: sys-player-classes (vault/reference/tech/systems/core/sys-player/sys-player-classes.md), bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), Source/RogueRigs/Player/RRMinerAnimInstance.h, Source/RogueRigs/Player/RRMinerAnimInstance.cpp
- expected_output: Change summary: header and property added, build result, commit hash. No vault doc edit (the doc step does it).
- done_when: UpperBodySequence declared as above with its header comment; RogueRigsEditor builds without warnings from the change; committed by path `task-406-01-upper-body-sequence-prop: ...`.
- editor_state: closed
- token_ceiling: 0.5 M units
- model: opus
- tokens: input 28, cache write 48823, cache read 232191, output 6204
- usage: turns 14, context start 12k peak 26k, resumes 0, 2 min, units 0.12 M
- runs: 2
- milestone: M2
- started: 2026-10-31T07:06:43Z
- waiting_for: none
- result: UpperBodySequence added to URRMinerAnimInstance (TObjectPtr<UAnimSequenceBase>, Animation|UpperBody); build ok; commit 49a881c3; first run blocked by Live Coding (runs 2)

## Relations
- [[task-406-miner-upper-body-idle-layer]] — parent global task.
