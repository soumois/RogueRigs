---
id: task-412-01-shot-origin-muzzle-capsule
description: "Cpp: ARRProjectile capsule collision root; rifle and pellet origin at the Muzzle, capsule sized per shot from floor + 20; miner-to-Muzzle sweep; tests"
parent: task-412-shots-from-rifle-muzzle
kind: task
status: todo
---
# Shots leave from the rifle muzzle: cpp

- goal: Per the global Questions as decided (defaults: Q1 bottom floor + 20, radius 14; Q2 parallel; Q3 sweep kept; Q4 shotgun too). (a) Source/RogueRigs/Projectiles/RRProjectile.h/.cpp: the collision root becomes a UCapsuleComponent with the same collision setup, overlap and stop handlers; default half height = radius, so ARRShotgunPellet, ARRTurretBullet and ARREnemyProjectile (radius 10) behave exactly as today; replace GetCollisionSphere users; add a way to set the capsule half height and the vertical offset of the collision relative to the shot mesh per shot before FinishSpawning (SpawnProjectile parameter or setter), so the ShotMesh appears at the Muzzle while the capsule spans from floor + clearance to at least the Muzzle height (symmetric capsule centred on the Muzzle or offset, the agent picks the simplest and states it). (b) Source/RogueRigs/Weapons/: with an owning miner holding a weapon on its hand socket, the origin of rifle bullets and shotgun pellets is that weapon's Muzzle world location read at fire time; capsule bottom = floor under the miner (current FloorZ computation) + editable clearance 20; if the Muzzle is below clearance + radius above the floor, fall back to a sphere at floor + ShotHeight under the Muzzle with a warning; without a hand-socket weapon, today's point and sphere; without a miner, the own Muzzle (unchanged). (c) Q3: before spawning, one sweep of the same capsule on the projectile collision from the miner axis to the origin: a blocking hit stops the shot there (barrel damage as OnProjectileStop does), robots on the segment are hit as normal projectile hits counting toward pierce (reuse the projectile hit path). Turret overrides unchanged. Header comments updated (Muzzle no longer visual only). Update only the expectations of existing automation tests the change breaks (RogueRigs.Weapons.MinerStartingWeapon shot position and height, projectile sphere radius checks if any); add no test. Never touch or stage any .uasset.
- agent: cpp
- depends_on: none
- inputs: task-412-shots-from-rifle-muzzle (vault/reference/tasks/task-412-shots-from-rifle-muzzle.md), sys-projectiles (vault/reference/tech/systems/combat/sys-projectiles.md), sys-weapons (vault/reference/tech/systems/combat/sys-weapons.md), sys-weapons-shotgun (vault/reference/tech/systems/combat/sys-weapons/sys-weapons-shotgun.md), sys-player-classes (vault/reference/tech/systems/core/sys-player/sys-player-classes.md), bp-burst-rifle (vault/reference/tech/blueprints/player/bp-burst-rifle.md), bp-projectile (vault/reference/tech/blueprints/player/bp-projectile.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md), agent-cpp (vault/reference/agents/agent-cpp.md), Source/RogueRigs/Projectiles/RRProjectile.h, Source/RogueRigs/Projectiles/RRProjectile.cpp, Source/RogueRigs/Projectiles/RREnemyProjectile.cpp, Source/RogueRigs/Weapons/RRWeapon.h, Source/RogueRigs/Weapons/RRWeapon.cpp, Source/RogueRigs/Weapons/RRShotgun.cpp, Source/RogueRigs/Weapons/RRTurret.cpp, Source/RogueRigs/Tests/RRWeaponTests.cpp, Source/RogueRigs/Tests/RRTestAccess.h
- expected_output: Edited projectile and weapon C++ files and the broken test expectations; one commit `task-412-01-shot-origin-muzzle-capsule: <summary>` by path; a change summary for the bp and doc steps (component rename or class change and its effect on Blueprints, capsule sizing rule, Q3 method, shotgun, fallbacks, tests touched, commit hash, changed paths).
- done_when: Development Editor build succeeds with no new warning in the edited files; RogueRigs.Weapons, RogueRigs.Shotgun and the turret and shooter projectile tests pass headless with only broken expectations updated; code review shows: capsule root with half height = radius by default, rifle and pellet origin = hand-socket weapon Muzzle at fire time, capsule bottom floor + clearance and top at least the Muzzle height, fallback sphere with warning, Q3 sweep stops on rock and hits robots, turret bullets and robot shots unchanged; no .uasset staged; committed by path.
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
