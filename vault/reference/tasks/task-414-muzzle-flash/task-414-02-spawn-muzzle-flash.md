---
id: task-414-02-spawn-muzzle-flash
description: "Cpp: ARRWeapon spawns NS_MuzzleFlash attached to the shown hand weapon Muzzle on every miner shot (rifle bullet, shotgun volley)"
parent: task-414-muzzle-flash
kind: task
status: in-progress
---
# Muzzle flash on every miner shot: cpp

- goal: Per the global Questions as decided (Q3 shots, Q5 wiring). In Source/RogueRigs/Weapons/: an `EditDefaultsOnly` UNiagaraSystem property on ARRWeapon (e.g. MuzzleFlashSystem) defaulted in C++ to /Game/RogueRigs/FX/NS_MuzzleFlash by path (RRDrone.cpp pattern); on every shot fired by a weapon owned by the miner (each rifle bullet; one per shotgun volley, not per pellet) spawn it attached to the shown hand weapon Muzzle component (same Muzzle as GetShotPlacement), rotated to the shot direction, auto-destroy (or through the existing URRNiagaraPoolSubsystem if it fits; agent states which); skip it when Niagara does not render (-nullrhi, commandlets) as RRDrone does; turret overrides and robot shots spawn none. Header comments updated. Add no test; update only expectations a change breaks. Never touch or stage any .uasset.
- agent: cpp
- depends_on: task-414-01-ns-muzzle-flash
- inputs: task-414-muzzle-flash (vault/reference/tasks/task-414-muzzle-flash.md), sys-weapons (vault/reference/tech/systems/combat/sys-weapons.md), sys-weapons-shotgun (vault/reference/tech/systems/combat/sys-weapons/sys-weapons-shotgun.md), bp-burst-rifle (vault/reference/tech/blueprints/player/bp-burst-rifle.md), agent-cpp (vault/reference/agents/agent-cpp.md), Source/RogueRigs/Weapons/RRWeapon.h, Source/RogueRigs/Weapons/RRWeapon.cpp, Source/RogueRigs/Weapons/RRShotgun.cpp, Source/RogueRigs/Weapons/RRTurret.cpp, Source/RogueRigs/Drone/RRDrone.cpp, Source/RogueRigs/Pool/RRNiagaraPoolSubsystem.h
- expected_output: Edited weapon C++ files; one commit `task-414-02-spawn-muzzle-flash: <summary>` by path; a change summary for the doc step (property name and default path, spawn point and attach, per-shot rule, pool or not, nullrhi guard, commit hash, changed paths).
- done_when: Development Editor build succeeds with no new warning in the edited files; RogueRigs.Weapons, RogueRigs.Shotgun and turret tests pass headless; code review shows one flash per rifle bullet and per shotgun volley attached to the hand weapon Muzzle, none for turret and robot shots, nullrhi guard, default loaded by path; no .uasset staged; committed by path.
- editor_state: closed
- token_ceiling: 1.2 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-31T22:20:31Z
- waiting_for: none
- result: open

## Relations
- [[task-414-muzzle-flash]] — parent global task.
