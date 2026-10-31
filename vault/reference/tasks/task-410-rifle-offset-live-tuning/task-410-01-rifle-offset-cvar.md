---
id: task-410-01-rifle-offset-cvar
description: "Cpp: cvar rr.Debug.RifleOffset "X Y Z": held weapon mesh relative location live, Muzzle moved by the same delta, left hand IK refreshed"
parent: task-410-rifle-offset-live-tuning
kind: task
status: todo
---
# Live control of the rifle offset in the right hand: cpp

- goal: Add the cheat console variable `rr.Debug.RifleOffset` (string "X Y Z" in cm, default empty, ECVF_Cheat) next to the other miner debug cvars in Source/RogueRigs/Player/RRMinerAnimInstance.cpp: for the player-controlled miner only, when the value changes, set the relative location of the hand-held weapon mesh that has the LeftHandGrip socket (RifleMesh of BP_BurstRifle) to (X, Y, Z), keep its rotation and scale, move the weapon Muzzle by the same delta from its Blueprint value (Blueprint values read from the weapon class default object or saved at first apply), then call RefreshLeftHandIK so LeftHandIKTarget follows on the same frame; empty restores the Blueprint RifleMesh location and Muzzle and refreshes the IK; an unparsable value logs a warning and changes nothing; each apply logs one LogRogueRigs line "rr.Debug.RifleOffset: RifleMesh Location (X, Y, Z), Muzzle (mx, my, mz) - type these in BP_BurstRifle to keep them"; a weapon added later gets the current value before its IK refresh. Muzzle access: use an existing accessor or add a minimal const getter on ARRWeapon if none exists. No change to shot origin, AddWeapon, WeaponSocket_R or any asset.
- agent: cpp
- depends_on: none
- inputs: task-410-rifle-offset-live-tuning (vault/reference/tasks/task-410-rifle-offset-live-tuning.md), sys-player-classes (vault/reference/tech/systems/core/sys-player/sys-player-classes.md), bp-burst-rifle (vault/reference/tech/blueprints/player/bp-burst-rifle.md), bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), sys-weapons (vault/reference/tech/systems/combat/sys-weapons.md), agent-cpp (vault/reference/agents/agent-cpp.md), Source/RogueRigs/Player/RRMinerAnimInstance.h, Source/RogueRigs/Player/RRMinerAnimInstance.cpp, Source/RogueRigs/Weapons/RRWeapon.h
- expected_output: Edited RRMinerAnimInstance.h/.cpp (and RRWeapon.h only if a Muzzle getter is needed), header comments for the cvar; one commit `task-410-01-rifle-offset-cvar: <summary>`; a change summary for the doc step (cvar name, syntax, what it moves, Muzzle rule, IK refresh, log line, commit hash, changed paths).
- done_when: Development Editor build succeeds with no new warning in the edited files; the cvar is registered with ECVF_Cheat and a help text giving syntax, units, axes (RifleMesh Location frame) and the empty-value reset; code review shows: applied only on value change and only for the player-controlled miner, RifleMesh relative location set and rotation and scale untouched, Muzzle moved by the same delta, RefreshLeftHandIK called after each apply and after a later weapon add, empty restores Blueprint values, bad input logs a warning, one log line per apply; existing automation tests that compiled before still compile (no test added); committed by path.
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
- [[task-410-rifle-offset-live-tuning]] — parent global task.
