---
id: task-386-21-force-move-cvar
description: "Cpp: debug cvar forcing MoveX and MoveY in URRMinerAnimInstance so each blend space sample can be captured"
parent: task-386-miner-mixamo-animation
kind: task
status: done
---
# Miner animated with the owner Mixamo clips: debug override of MoveX and MoveY

- goal: Add a cheat cvar rr.Debug.MinerAnimForce (string 'x y', empty = off) overriding MoveX and MoveY and GroundSpeed (forced length times MaxWalkSpeed) in URRMinerAnimInstance, to capture each of the four samples and the mix; debug only, off by default.
- agent: cpp
- depends_on: 
- inputs: URRMinerAnimInstance (Source/RogueRigs/Player/RRMinerAnimInstance.h and .cpp), task-386-16 commit f6dccd5c
- expected_output: report
- done_when: closed
- editor_state: task-386-20-run-clips-arm-check
- token_ceiling: 0.4 M units
- model: opus
- tokens: input 16, cache write 24720, cache read 137931, output 4041
- usage: turns 8, context start 12k peak 25k, resumes 0, 2 min, units 0.06 M
- runs: 1
- milestone: M2
- started: 2026-10-30T17:02:54Z
- waiting_for: none
- result: rr.Debug.MinerAnimForce 'x y' overrides MoveX/MoveY/GroundSpeed; build ok; commit 03ab6579

## Relations
- [[task-386-miner-mixamo-animation]] - parent global task.
