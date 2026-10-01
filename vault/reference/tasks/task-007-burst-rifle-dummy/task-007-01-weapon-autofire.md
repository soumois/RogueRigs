---
id: task-007-01-weapon-autofire
description: "Write URRWeaponAttributeSet, ARRWeapon with its own ASC and CSV stats, URRAutoFireComponent burst timing, and the miner equipping a starting weapon."
parent: task-007-burst-rifle-dummy
kind: task
status: todo
---
# Weapon and auto-fire

- goal: a weapon that decides when to fire bursts toward the miner's aim.
- agent: cpp
- depends_on: task-006-dig
- inputs: [[sys-weapons]], [[sys-auto-fire]], [[sys-gas-core]], [[burst-rifle]], [[sys-player]], [[naming]], [[cpp-standard]], [[gas-api-notes]].
- expected_output: `Source/RogueRigs/Weapons/`: `URRWeaponAttributeSet` (`Damage`, `FireRate`, `ReloadSpeed`, `CritChance`, `CritDamage`, `MagazineSize`, `Ammo`; clamps ≥ 0, `Ammo` ≤ `MagazineSize`); a weapon CSV `Content/RogueRigs/Data/Tables/WeaponStats.csv` row `BurstRifle`: Damage 10, FireRate 2, BurstCount 3, BurstDuty 0.32, CritChance 0.05, CritDamage 1.5, MagazineSize 30, ReloadTime 1.5, ReloadSpeed 1, ProjectileSpeed 3000, Range 2000, SpreadDeg 2 (constants that are not attributes stay CSV values on the weapon); `ARRWeapon` (abstract) with its own `URRAbilitySystemComponent`, acquisition order of [[sys-weapons]] minus upgrades and registry, virtual `FireOnce()` (empty here; projectile in 007-02), aim from the owning miner's `GetAimDirection()`; `URRAutoFireComponent` with the exact burst timing, magazine and reload of [[sys-auto-fire]] (ammo use and refill through the `Ammo` attribute; GE classes optional, report the choice). `ARRMiner`: editable `StartingWeapons` (array of weapon classes), spawned and attached at `BeginPlay` (socket name editable), destroyed at `EndPlay`. Docs [[sys-weapons]], [[sys-auto-fire]], [[sys-player]] updated.
- done_when: build exits 0 with the editor closed; no new warnings in touched files; docs match; lint clean.
- editor_state: closed
- token_ceiling: 80000
- milestone: M1
- result: open

## Relations
- [[task-007-burst-rifle-dummy]] — parent global task.
