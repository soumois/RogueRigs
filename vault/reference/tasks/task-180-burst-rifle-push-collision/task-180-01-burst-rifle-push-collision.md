---
id: task-180-01-burst-rifle-push-collision
description: "Stop burst rifle shots from pushing robots through collision: the burst rifle has no knockback, so a hit must not move a robot."
parent: task-180-burst-rifle-push-collision
kind: task
status: todo
---
# Burst rifle: shots push robots by collision

- goal: Stop burst rifle shots from pushing robots through collision: the burst rifle has no knockback, so a hit must not move a robot.
- agent: cpp
- depends_on: none
- inputs: burst-rifle (vault/reference/gdd/weapons/burst-rifle.md), sys-projectiles (vault/reference/tech/systems/combat/sys-projectiles.md), sys-enemies-base (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-base.md), sys-enemies-movement (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-movement.md), sys-weapons (vault/reference/tech/systems/combat/sys-weapons.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), sys-tests-weapons (vault/reference/tech/systems/tools/sys-automation-tests/sys-tests-weapons.md)
- expected_output: The cause found with a -game run (robot velocity and position before and after burst rifle hits, KnockbackSpeed of the burst projectile, the collision responses of the projectile and of the robot root and hit capsules, any physics or movement impulse) and fixed in C++ so a burst rifle hit never moves a robot, while the shotgun knockback (KnockbackSpeed) stays as it is; sys-projectiles (and sys-enemies-base if touched) updated
- done_when: Cause proven by values from a run; after the fix a rerun shows no robot displacement from burst rifle hits and the shotgun still pushes; build passes with the editor closed; existing weapon and enemy tests still pass except the known EyeMesh one; docs updated; committed by path
- editor_state: closed
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-180-burst-rifle-push-collision]] — parent global task.
