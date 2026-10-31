---
id: task-412-01-shot-origin-muzzle
description: "Cpp: ARRWeapon::GetShotOrigin from the held rifle Muzzle X, Y at floor + 50, segment check miner to Muzzle, shotgun uses it; tests expectations"
parent: task-412-shots-from-rifle-muzzle
kind: task
status: todo
---
# Shots leave from the rifle muzzle: cpp

- goal: Per the global Questions as decided (defaults: Q1 Muzzle X, Y at floor + ShotHeight; Q2 parallel to the aim; Q3 segment check; Q4 shotgun too). In Source/RogueRigs/Weapons/: when the weapon has an owning miner, the shot origin is the X, Y of the Muzzle of the miner's weapon attached on its hand socket (the burst rifle; read at fire time, after the anim counter-move), Z = floor under the miner (current FloorZ computation) + ShotHeight; when no hand-socket weapon is held, today's point (capsule radius + ShotForwardOffset ahead); without a miner, the own Muzzle (unchanged). Q3: before spawning, trace on the Projectile channel (the channel the projectiles collide on) from the miner axis at shot height to that origin: a rock block hit stops the shot at the block (no projectile beyond it, impact as a projectile hitting rock); robots on the segment receive the hit as a normal projectile hit counting toward pierce (reuse ARRProjectile hit handling, e.g. spawn at the miner axis and advance, or apply the hits then spawn at the Muzzle with the remaining pierce: the agent picks the simplest correct way and states it). ARRShotgun uses the same origin and check for each pellet. Turret overrides unchanged. Keep the ShotHeight below BlockHeight warning. Update header comments (RRWeapon.h class comment, GetShotOrigin, Muzzle no longer visual only). Update only the expectations of existing automation tests the change breaks (RogueRigs.Weapons.MinerStartingWeapon shot position: now at the Muzzle X, Y; shotgun origin tests if broken); add no test. Do not touch any .uasset (the owner has uncommitted BP_BurstRifle and ABP_Miner edits: never stage them).
- agent: cpp
- depends_on: none
- inputs: task-412-shots-from-rifle-muzzle (vault/reference/tasks/task-412-shots-from-rifle-muzzle.md), sys-weapons (vault/reference/tech/systems/combat/sys-weapons.md), sys-weapons-shotgun (vault/reference/tech/systems/combat/sys-weapons/sys-weapons-shotgun.md), sys-projectiles (vault/reference/tech/systems/combat/sys-projectiles.md), sys-player-classes (vault/reference/tech/systems/core/sys-player/sys-player-classes.md), bp-burst-rifle (vault/reference/tech/blueprints/player/bp-burst-rifle.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md), agent-cpp (vault/reference/agents/agent-cpp.md), Source/RogueRigs/Weapons/RRWeapon.h, Source/RogueRigs/Weapons/RRWeapon.cpp, Source/RogueRigs/Weapons/RRShotgun.cpp, Source/RogueRigs/Weapons/RRTurret.cpp, Source/RogueRigs/Player/RRMiner.h, Source/RogueRigs/Tests/RRWeaponTests.cpp
- expected_output: Edited weapon (and projectile if needed) C++ files and the broken test expectations; one commit `task-412-01-shot-origin-muzzle: <summary>` by path; a change summary for the doc step (origin rule, segment check method, shotgun, fallbacks, tests touched, commit hash, changed paths).
- done_when: Development Editor build succeeds with no new warning in the edited files; the RogueRigs.Weapons and RogueRigs.Shotgun automation tests pass (run headless) with only broken expectations updated; code review shows: rifle origin = held weapon Muzzle X, Y read at fire time at floor + ShotHeight, fallback to the old point without a hand-socket weapon, own Muzzle without a miner, segment check stops on rock and hits robots, shotgun uses the same origin, turrets unchanged; no .uasset staged; committed by path.
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
- [[task-412-shots-from-rifle-muzzle]] — parent global task.
