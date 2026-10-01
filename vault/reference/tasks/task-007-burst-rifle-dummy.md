---
id: task-007-burst-rifle-dummy
description: "Global task: the burst rifle (weapon ASC, auto-fire bursts, projectiles, GAS damage with crits) and an immobile target dummy showing hits."
parent: tasks-index
kind: task
status: in-progress
---
# Burst rifle and target dummy

- request: "Implémente le tir (la mitraillette). Implémente un ennemi immobile (une sphère) qui ne bouge pas, pour voir les dégâts dessus (une sorte de punching-ball pour tester les armes et les effets visuels)." Owner confirmed the burst rifle, the proposed values and the debug damage text, and pre-approved every subtask (2026-10-01).
- goal: in PIE the miner auto-fires burst rifle bursts toward the aim; projectiles stop on rock; hitting a target dummy lowers its health through GAS, flashes it white and shows a floating number (bigger, white-gold on crit); the dummy refills 3 s after the last hit and never dies.
- acceptance: (1) build exits 0; (2) `ARRWeapon` with its own ASC and `URRWeaponAttributeSet`, values from `WeaponStats.csv` row `BurstRifle`; `URRAutoFireComponent` follows [[sys-auto-fire]] timing; (3) projectile stops on rock blocks and applies the damage spec on impact; `URRGE_Damage` ExecCalc rolls crits; (4) PIE: projectiles spawn in bursts of 3 about 0.08 s apart, the dummy's health drops by 10 (15 on crit) per hit, and the magazine reloads after 30 shots; (5) docs `sys-weapons`, `sys-auto-fire`, `sys-projectiles`, `sys-damage`, new `sys-target-dummy`, Blueprint docs match; lint clean, every subtask committed; (6) owner checks feel and visuals.
- gdd_check: read [[burst-rifle]], [[sys-weapons]], [[sys-auto-fire]], [[sys-projectiles]], [[sys-damage]], [[damage-numbers]], [[sys-gas-core]], [[color-code]]. Open values set by the owner (2026-10-01) in [[burst-rifle]]; final weapon name stays open (`BurstRifle` used as working name). Damage numbers are M2: an M1 debug text on the dummy only, recorded in [[damage-numbers]]. The target dummy is a test tool, not a GDD entity. Out of scope: shotgun, turret, upgrades, enemy death, i-frames.
- confirmed_by_owner: yes
- milestone: M1
- next_step: task-007-02-projectile-damage
- result: open

## Subtasks
1. task-007-01-weapon-autofire — cpp: weapon attribute set, `ARRWeapon`, weapon CSV, auto-fire component, miner equips it.
2. task-007-02-projectile-damage — cpp: projectile, collision channel, damage GE and ExecCalc, native tags.
3. task-007-03-target-dummy — cpp: `ARRTargetDummy`, hit flash, debug damage text, refill.
4. task-007-04-bp-rifle-dummy — bp: Blueprints, materials, three dummies in the map, PIE check.
5. task-007-05-weapon-tests — cpp: headless automation tests of timing, magazine, projectile and damage (added: MCP cannot drive PIE input).

## Relations
- [[tasks-index]] — parent index.
- [[task-007-01-weapon-autofire]] — step 1, cpp, weapon and auto-fire.
- [[task-007-02-projectile-damage]] — step 2, cpp, projectile and damage.
- [[task-007-03-target-dummy]] — step 3, cpp, target dummy.
- [[task-007-04-bp-rifle-dummy]] — step 4, bp, assets and PIE.
- [[task-007-05-weapon-tests]] — step 5, cpp, automation tests.
