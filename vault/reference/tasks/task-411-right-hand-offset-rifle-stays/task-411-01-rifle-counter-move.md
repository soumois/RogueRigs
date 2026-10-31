---
id: task-411-01-rifle-counter-move
description: "Cpp: URRMinerAnimInstance: RifleMesh and Muzzle counter-moved by the inverse right hand offset, left hand IK refreshed"
parent: task-411-right-hand-offset-rifle-stays
kind: task
status: todo
---
# Right hand offset moves the hand on the rifle, the rifle stays in place: cpp

- goal: In Source/RogueRigs/Player/RRMinerAnimInstance.cpp/.h, extend RefreshRightHandOffset (task-410-01): for the weapon attached on its hand socket (same test as RefreshLeftHandIK), set the relative transform of its mesh carrying LeftHandGrip to Blueprint relative transform * O^-1 and Muzzle to Blueprint relative transform * O^-1 (O = FTransform(RightHandOffsetRotation, RightHandOffsetLocation), UE order A * B applies A then B; Blueprint values read from the weapon class default object components, or saved on first apply), so the rifle world transform is unchanged while the hand moves by D = S^-1 * O * S; then call RefreshLeftHandIK for that weapon; call the same counter-move from HandleWeaponAdded (and NativeBeginPlay for weapons already held) after RefreshLeftHandIK order is respected (counter-move first, then RefreshLeftHandIK); offset 0 restores the Blueprint values; add the applied RifleMesh and Muzzle relative transforms to the existing log line; fix the cvar help text (the hand moves on the rifle, the rifle stays in place, it follows the hand in animations). Muzzle: add a minimal const getter on ARRWeapon only if needed. If the Q1 clamp is chosen: clamp location length to 15 cm with a warning. No ABP, asset, AddWeapon, WeaponSocket_R or shot origin change.
- agent: cpp
- depends_on: none
- inputs: task-411-right-hand-offset-rifle-stays (vault/reference/tasks/task-411-right-hand-offset-rifle-stays.md), task-410-right-hand-offset-live (vault/reference/tasks/task-410-right-hand-offset-live.md), sys-player-classes (vault/reference/tech/systems/core/sys-player/sys-player-classes.md), bp-burst-rifle (vault/reference/tech/blueprints/player/bp-burst-rifle.md), bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), agent-cpp (vault/reference/agents/agent-cpp.md), Source/RogueRigs/Player/RRMinerAnimInstance.h, Source/RogueRigs/Player/RRMinerAnimInstance.cpp, Source/RogueRigs/Weapons/RRWeapon.h
- expected_output: Edited RRMinerAnimInstance.h/.cpp (RRWeapon.h only for a Muzzle getter), comments stating R' = R * O^-1 and why it keeps the rifle world transform; one commit `task-411-01-rifle-counter-move: <summary>`; a change summary for the doc step (rule, call order, log line, commit hash, changed paths).
- done_when: Development Editor build succeeds with no new warning in the edited files; code review shows: counter-move applied on every offset apply and at weapon add, only for the weapon on its hand socket, from the Blueprint values (no accumulation over repeated applies), Muzzle moved the same way, RefreshLeftHandIK called after the counter-move, offset 0 restores the Blueprint values, log line extended, help text fixed; existing automation tests still compile (no test added); committed by path.
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
- [[task-411-right-hand-offset-rifle-stays]] — parent global task.
