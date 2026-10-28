---
id: task-350-01-turret-head-yaw
description: "Before capture of the grey-box turret; ARRTurret keeps its placement yaw and turns a C++ HeadPivot toward the target, Muzzle on the head."
parent: task-350-turret-in-game
kind: task
status: in-progress
---
# Turret C++: fixed base, yawing head pivot, shots from the canon tip

- goal: (0) Before any change, a -game capture of a placed grey-box turret firing at a robot (reference for subtask 04). (1) `ARRTurret` no longer turns the whole actor: the actor keeps the yaw it got at placement (facing the aim), and a C++ `HeadPivot` scene component (constructor subobject under the root) gets the yaw toward the locked robot, snapped as today (owner Q3). (2) The inherited `Muzzle` is attached to `HeadPivot`; bullets and `OnTurretFired` start at the Muzzle's horizontal position at today's `ShotHeight` above the actor (owner Q4), while `IsValidTarget` (range, line of sight) still measures from the turret centre at `ShotHeight` as today. No other rule changes (lock, timing, pierce, lifetime, no collision).
- agent: cpp
- depends_on: none
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), sys-turret (vault/reference/tech/systems/combat/sys-turret.md), turret (vault/reference/gdd/weapons/turret.md), bp-turret (vault/reference/tech/blueprints/player/bp-turret.md), sys-drone-parts (vault/reference/tech/systems/combat/sys-drone/sys-drone-parts.md, precedent of C++ part components), sys-tests-weapon-types (vault/reference/tech/systems/tools/sys-automation-tests/sys-tests-weapon-types.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md), art-prompts-turret (vault/reference/gdd/art-audio/art-prompts-turret.md); decisions: global `request` and `gdd_check` (vault/reference/tasks/task-350-turret-in-game.md); code: `Source/RogueRigs/Weapons/RRTurret.h`, `RRTurret.cpp`, `RRWeapon.h` (Muzzle, ShotHeight), `Source/RogueRigs/Tests/RRTurretTests.cpp`, `RRWeaponsIntegrationTests.cpp`.
- expected_output: before frames in `Saved/TurretInGame/before/` (not committed); `HeadPivot` (VisibleAnywhere, BlueprintReadOnly, so BP_Turret can attach the head mesh under it and set its relative location), Muzzle re-attached to it, a split between the spawn origin and the targeting origin (e.g. `GetTargetingOrigin`), Tick turning `HeadPivot` (relative yaw = target yaw minus actor yaw) instead of the actor; header comments updated; change summary for the doc subtask (public API, defaults, behaviour).
- done_when: before frames exist and their paths are reported; Development Editor build succeeds; `RogueRigs.Turret.*` and `Integration.PlaceTurretInputKillsRobot` pass (only expectations broken by the requested change are updated, and named in the summary); a new or extended check in `RogueRigs.Turret.FireRangeLifetime` or a sibling shows the actor yaw unchanged after a lock and `HeadPivot` world yaw pointing at the target within 1 degree; committed by path as `task-350-01-turret-head-yaw: <summary>`; editor left closed for the next step to open.
- editor_state: closed
- token_ceiling: 1.2 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-28T16:06:37Z
- waiting_for: none
- result: open

## Relations
- [[task-350-turret-in-game]] — parent global task.
