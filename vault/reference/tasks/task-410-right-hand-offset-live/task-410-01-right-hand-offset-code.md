---
id: task-410-01-right-hand-offset-code
description: "Cpp: URRMinerAnimInstance: RightHandOffset properties, cvar rr.Debug.RightHandOffset, ABP inputs, rifle counter-move, left hand IK refresh"
parent: task-410-right-hand-offset-live
kind: task
status: todo
---
# Live control of the right hand offset on the rifle grip: cpp

- goal: In Source/RogueRigs/Player/RRMinerAnimInstance.h/.cpp: add EditDefaultsOnly BlueprintReadOnly `RightHandOffsetLocation` (FVector, cm) and `RightHandOffsetRotation` (FRotator, deg), category `Animation|RightHand`, default 0, in the WeaponSocket_R frame, rotation pivot at the socket point; add the cheat cvar `rr.Debug.RightHandOffset` ("X Y Z" or "X Y Z Pitch Yaw Roll", empty = class defaults) read like rr.Debug.MinerUpperPose (player-controlled miner only, applied only when the value changes); on each change and at weapon add compute BlueprintReadOnly ABP inputs: `RightHandIKEffector` (FVector, location in RightHand bone space where the hand bone must go so that the socket point moves by the offset, pivot correction for the rotation included, mind the socket scale 0.01 versus bone scale about 100), `RightHandRotationDelta` (FRotator, bone-space rotation to add on RightHand) and `RightHandOffsetAlpha` (1 when the offset is non-zero, else 0); while a weapon is attached on its hand socket, set its mesh carrying LeftHandGrip (RifleMesh) relative transform = inverse offset composed with its Blueprint relative transform (scale 1.25 kept) and the Muzzle likewise from its Blueprint value, so the rifle world transform is unchanged; then call RefreshLeftHandIK; offset 0 or empty restores the Blueprint values; bad input logs a warning and changes nothing; each apply logs one LogRogueRigs line giving the RightHandOffsetLocation and RightHandOffsetRotation to type into ABP_Miner Class Defaults. No per-frame work beyond the existing cvar read pattern; no change to shot origin, AddWeapon, WeaponSocket_R, assets or the ABP.
- agent: cpp
- depends_on: none
- inputs: task-410-right-hand-offset-live (vault/reference/tasks/task-410-right-hand-offset-live.md), sys-player-classes (vault/reference/tech/systems/core/sys-player/sys-player-classes.md), bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), bp-burst-rifle (vault/reference/tech/blueprints/player/bp-burst-rifle.md), sys-weapons (vault/reference/tech/systems/combat/sys-weapons.md), agent-cpp (vault/reference/agents/agent-cpp.md), Source/RogueRigs/Player/RRMinerAnimInstance.h, Source/RogueRigs/Player/RRMinerAnimInstance.cpp, Source/RogueRigs/Weapons/RRWeapon.h
- expected_output: Edited RRMinerAnimInstance.h/.cpp (RRWeapon.h only for a minimal Muzzle getter if none exists), header comments for the properties, inputs and cvar; one commit `task-410-01-right-hand-offset-code: <summary>`; a change summary for the bp and doc steps: exact property and input names, their spaces and units, the alpha rule, the counter-move rule, the log line, commit hash, changed paths.
- done_when: Development Editor build succeeds with no new warning in the edited files; code review shows: properties and cvar as specified (ECVF_Cheat, help text with syntax, units, axes, empty reset); apply only on change and at weapon add, player miner only; the counter-move is the exact inverse of the hand delta in the socket frame (a comment states the math; a log line at apply prints the rifle world location and rotation before and after, equal within 0.01 cm and 0.01 deg when the hand nodes are ignored); RefreshLeftHandIK called after each apply; offset 0 restores Blueprint RifleMesh and Muzzle; existing automation tests still compile (no test added); committed by path.
- editor_state: closed
- token_ceiling: 1.2 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-410-right-hand-offset-live]] — parent global task.
