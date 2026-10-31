---
id: task-400-01-weapon-hand-socket
description: "Cpp: per-weapon hand socket on ARRWeapon, used by ARRMiner when attaching"
parent: task-400-burst-rifle-in-hand
kind: task
status: done
---
# Burst rifle 3D mesh in the miner right hand: cpp

- goal: ARRWeapon gets an editable FName hand socket (EditDefaultsOnly, None = keep the miner WeaponAttachSocketName or capsule WeaponAttachOffset); ARRMiner attaches a weapon to that mesh socket when set. Shot origin untouched.
- agent: cpp
- depends_on: none
- inputs: sys-weapons (vault/reference/tech/systems/combat/sys-weapons.md), sys-player (vault/reference/tech/systems/core/sys-player.md), sys-player-classes (vault/reference/tech/systems/core/sys-player/sys-player-classes.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md), `Source/RogueRigs/Weapons/RRWeapon.h`, `Source/RogueRigs/Player/RRMiner.h` (WeaponAttachSocketName, WeaponAttachOffset), `Source/RogueRigs/Tests/RRTestAccess.h`
- expected_output: C++ change with header comments, build passes, existing RogueRigs.* tests that touch weapon attachment still pass (only expectations broken by this change updated, no new test); change summary (property name, attach rule) for the doc step; commit `task-400-01-weapon-hand-socket: ...`.
- done_when: Editor target builds; the new property exists on ARRWeapon with None default; ARRMiner uses it when set and the old path otherwise; weapon tests green; committed by path.
- editor_state: closed
- token_ceiling: 1.2 M units
- model: opus
- tokens: input 36, cache write 57625, cache read 727519, output 12203
- usage: turns 18, context start 12k peak 63k, resumes 0, 6 min, units 0.21 M
- runs: 1
- milestone: M2
- started: 2026-10-30T23:54:46Z
- waiting_for: none
- result: ARRWeapon::HandSocketName (FName, None default) + GetHandSocketName; ARRMiner::AddWeapon snaps to it when the mesh has the socket, else old path (warning if missing). Build ok, Weapons 12 and Shotgun 9 green; 2 Integration tests fail (JuiceKillDropsStreamingCubes XP pickup collision, LevelUpUpgradeLoop RerollButton), not proven pre-existing. Commit f8d051dc

## Relations
- [[task-400-burst-rifle-in-hand]] — parent global task.
