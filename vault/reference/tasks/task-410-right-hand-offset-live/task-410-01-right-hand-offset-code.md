---
id: task-410-01-right-hand-offset-code
description: "Cpp: URRMinerAnimInstance: RightHandOffset properties, cvar rr.Debug.RightHandOffset, ABP inputs (effector, rotation delta, alpha)"
parent: task-410-right-hand-offset-live
kind: task
status: in-progress
---
# Live control of the right hand offset: cpp

- goal: In Source/RogueRigs/Player/RRMinerAnimInstance.h/.cpp: add EditDefaultsOnly BlueprintReadOnly `RightHandOffsetLocation` (FVector, cm) and `RightHandOffsetRotation` (FRotator, deg), category `Animation|RightHand`, default 0, expressed in the WeaponSocket_R frame (X toward the rifle muzzle, Y side, Z rifle top), rotation pivot at the WeaponSocket_R point (the grip); add the cheat cvar `rr.Debug.RightHandOffset` ("X Y Z" or "X Y Z Pitch Yaw Roll", empty = class defaults) read like rr.Debug.MinerUpperPose (player-controlled miner only, applied only when the value changes); at anim begin (from the class defaults) and on each cvar change compute BlueprintReadOnly ABP inputs: `RightHandIKEffector` (FVector, location in RightHand bone space where the hand bone must go so that the socket point moves by the location offset and the hand turns around the socket point; socket local transform from the mesh socket named by the held weapon `GetHandSocketName()` or WeaponSocket_R; mind the socket scale 0.01 versus the bone scale about 100), `RightHandRotationDelta` (FRotator, bone-space rotation to add on RightHand) and `RightHandOffsetAlpha` (1 when the offset is non-zero, else 0). The rifle follows the hand (owner Q1): do not touch RifleMesh, Muzzle, BP_BurstRifle, WeaponSocket_R, AddWeapon or the shot origin; no left hand IK refresh needed (its target is in RightHand bone space). Bad input logs a warning and changes nothing; each apply logs one LogRogueRigs line giving the RightHandOffsetLocation and RightHandOffsetRotation to type into ABP_Miner Class Defaults. No per-frame work beyond the existing cvar read pattern; no asset or ABP change.
- agent: cpp
- depends_on: none
- inputs: task-410-right-hand-offset-live (vault/reference/tasks/task-410-right-hand-offset-live.md), sys-player-classes (vault/reference/tech/systems/core/sys-player/sys-player-classes.md), bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), agent-cpp (vault/reference/agents/agent-cpp.md), Source/RogueRigs/Player/RRMinerAnimInstance.h, Source/RogueRigs/Player/RRMinerAnimInstance.cpp, Source/RogueRigs/Weapons/RRWeapon.h (GetHandSocketName only, read)
- expected_output: Edited RRMinerAnimInstance.h/.cpp with header comments for the properties, inputs and cvar; one commit `task-410-01-right-hand-offset-code: <summary>`; a change summary for the bp and doc steps: exact property and input names, their spaces and units, the pivot math, the alpha rule, the log line, commit hash, changed paths.
- done_when: Development Editor build succeeds with no new warning in the edited files; code review shows: properties and cvar as specified (ECVF_Cheat, help text with syntax, units, axes, empty reset); inputs computed at anim begin and on cvar change only, cvar for the player miner only; a comment states the socket-frame to RightHand-bone-space conversion and the pivot correction; offset 0 gives alpha 0; no change to RifleMesh, Muzzle, weapon or asset code; existing automation tests still compile (no test added); committed by path.
- editor_state: closed
- token_ceiling: 1.2 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-31T20:13:59Z
- waiting_for: none
- result: open

## Relations
- [[task-410-right-hand-offset-live]] — parent global task.
