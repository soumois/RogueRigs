---
id: task-007-02-projectile-damage
description: "Write ARRProjectile (stops on rock, carries the damage spec), a Projectile collision channel, URRGE_Damage with a crit ExecCalc, native state tags."
parent: task-007-burst-rifle-dummy
kind: task
status: todo
---
# Projectile and damage

- goal: bullets that fly, stop on rock, and hurt anything with a health attribute set through GAS.
- agent: cpp
- depends_on: task-007-01-weapon-autofire
- inputs: [[sys-projectiles]], [[sys-damage]], [[sys-weapons]], [[sys-gas-core]], [[destruction-rules]], [[naming]], [[gas-api-notes]], [[gas-engine-facts]], [[cpp-standard]].
- expected_output: native tags `State.Dead`, `State.Invulnerable` ([[naming]]). `URRGE_Damage` (instant) with an ExecCalc that captures the source weapon's `Damage`, `CritChance`, `CritDamage`, rolls the crit, writes `IncomingDamage` on the target, and makes the crit flag readable by listeners (e.g. a dynamic tag on the spec or the effect context; report the choice); refuses targets with `State.Dead` or `State.Invulnerable`. `ARRProjectile` (abstract): sphere collision on a new `Projectile` object channel (config change allowed in `Config/DefaultEngine.ini` for that channel only, CRLF kept) that overlaps pawns and blocks world static and dynamic; projectile movement without sub-stepping, speed and lifetime from the weapon (`Range / ProjectileSpeed`); carries the spec handle; on overlap with an actor that has an ASC, applies the spec once and is destroyed; on block (rock), destroyed. `ARRWeapon` gets the burst rifle `FireOnce()` (spawn at a muzzle point, spread ±`SpreadDeg` in yaw) via an editable projectile class. Docs [[sys-projectiles]], [[sys-damage]], [[sys-weapons]] updated.
- done_when: build exits 0 with the editor closed; no new warnings in touched files; docs match; lint clean.
- editor_state: closed
- token_ceiling: 80000
- milestone: M1
- result: open

## Relations
- [[task-007-burst-rifle-dummy]] — parent global task.
