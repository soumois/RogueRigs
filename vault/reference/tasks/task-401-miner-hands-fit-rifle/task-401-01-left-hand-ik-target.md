---
id: task-401-01-left-hand-ik-target
description: "Cpp: URRMinerAnimInstance: left hand IK target in right hand space and alpha"
parent: task-401-miner-hands-fit-rifle
kind: task
status: done
---
# Miner hands fitted to the burst rifle: cpp

- goal: URRMinerAnimInstance exposes BlueprintReadOnly LeftHandIKTarget (location in the right hand bone space, from WeaponSocket_R and the held weapon mesh socket LeftHandGrip, recomputed only when the held weapon changes, event driven) and LeftHandIKAlpha (1 when the hand-held weapon mesh has LeftHandGrip, else 0).
- agent: cpp
- depends_on: none
- inputs: sys-player-classes (vault/reference/tech/systems/core/sys-player/sys-player-classes.md), sys-weapons (vault/reference/tech/systems/combat/sys-weapons.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md), `Source/RogueRigs/Player/RRMinerAnimInstance.h`, `Source/RogueRigs/Player/RRMinerAnimInstance.cpp`, `Source/RogueRigs/Player/RRMiner.h` (OnWeaponAdded)
- expected_output: C++ change with header comments, build passes, existing tests green (no new test); change summary (property names, socket names, space); commit `task-401-01-left-hand-ik-target: ...`.
- done_when: Editor target builds; both properties exist and are filled from the held weapon; no per-frame socket search; committed by path.
- editor_state: closed
- token_ceiling: 1.2 M units
- model: sonnet
- tokens: input 24, cache write 92564, cache read 880738, output 9111
- usage: turns 12, context start 58k peak 93k, resumes 0, 4 min, units 0.25 M
- runs: 1
- milestone: M2
- started: 2026-10-31T05:33:59Z
- waiting_for: none
- result: LeftHandIKTarget (right hand bone space, location) and LeftHandIKAlpha added to URRMinerAnimInstance, event driven via OnWeaponAdded; build ok; commit 5b6e3fc3. Limitation: no weapon-removed event.

## Relations
- [[task-401-miner-hands-fit-rifle]] — parent global task.
