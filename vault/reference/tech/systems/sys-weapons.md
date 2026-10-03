---
id: sys-weapons
description: "Weapon actors and their ASC: acquisition order, player registry, upgrade broadcast and replay, magazine and reload GEs, turret attribute copy."
parent: systems-index
kind: doc
---
# Weapons

- responsibility: weapon lifecycle, weapon stats, how upgrades reach weapons and turrets.
- main_classes (in `Source/RogueRigs/Weapons/`): `ARRWeapon` (abstract; own `URRAbilitySystemComponent` + `URRWeaponAttributeSet`, root scene component, `Muzzle` scene component under it (visual reference only), no actor collision (`bActorEnableCollision` off: a held weapon sits inside the miner capsule and would push it), `URRAutoFireComponent` [[sys-auto-fire]]; protected virtual `FireOnce()` decides what is fired: by default the burst rifle shot, overridden by weapons that fire differently); `URRWeaponAttributeSet` (`Damage`, `FireRate`, `ReloadSpeed`, `CritChance`, `CritDamage`, `MagazineSize`, `Ammo`; all >= 0, `Ammo` <= `MagazineSize`, lowered when `MagazineSize` drops); `FRRWeaponStatsRow`; GEs `URRGE_ConsumeAmmo` (instant, `Ammo` -1) and `URRGE_RefillAmmo` (instant, `Ammo` overridden by the target's `MagazineSize`). Weapons: burst rifle, shotgun, turret launcher.

## Decisions
- One ASC per weapon; upgrades routed by `Weapon.Player.*` tags (approved).
- Magazine and reload by GEs `ConsumeAmmo`, `Reloading`, `RefillAmmo` (approved). Implemented: `ConsumeAmmo`, `RefillAmmo`; the reload is a timer in [[sys-auto-fire]]; `Reloading` (duration GE + tag) waits for the tag hierarchy (open).
- Acquisition order (provisional): spawn and attach → `InitAbilityActorInfo(weapon, weapon)` → base values from CSV (`SetNumericAttributeBase`) → replay of global upgrades → registration in the player's registry.
- Global bonuses are broadcast to every weapon ASC, replayed on acquisition, never removed; the list of applied upgrades serves replay and debug (provisional).
- Player-state conditions (death, level transition) are read on the player's ASC; weapon ASCs do not carry those tags (provisional).
- 4-direction fire is a fire pattern inside `FireOnce()` (main shot + 3 weaker ones), not a projectile behavior (provisional).
- Burst rifle `FireOnce()` (implemented in `ARRWeapon`): `MakeDamageSpec()` (editable `DamageEffectClass`, default `URRGE_Damage`, context instigator and causer = the weapon), then one editable `ProjectileClass` spawned deferred at `GetShotOrigin()`, flat, aim yaw + uniform random in ±`SpreadDeg`, speed `ProjectileSpeed`, lifetime `Range / ProjectileSpeed`; Owner = weapon, Instigator = miner ([[sys-projectiles]]). Skipped with a warning if `ProjectileClass` is unset or `ProjectileSpeed`/`Range` <= 0. The Blueprint subclass sets `ProjectileClass`, the mesh and the `Muzzle` position.
- Shot origin (`GetShotOrigin()`): floor under the owning miner (capsule bottom minus the CharacterMovement floor distance when walking) + editable `ShotHeight` (50 uu; must stay below the grid `BlockHeight`, warning at acquisition otherwise), moved along the aim by capsule radius + editable `ShotForwardOffset` (20 uu, measured from the capsule surface). Shots hit floor-standing targets and stop on blocks whatever the weapon mesh pose. Without an owning miner (tests, future turrets): the `Muzzle`.
- Shotgun: `FireOnce()` runs a cone query, keeps enemies with a clear grid line of sight to the miner, and applies the damage spec to each one directly (one critical roll per target); no projectile.
- Reload duration = base `ReloadTime` (CSV) / `ReloadSpeed`.
- Turret launcher: `MagazineSize` = turret charges; placing a turret starts recharging that charge at `ReloadSpeed`, one charge at a time; no cap on active turrets.
- Impact effects are weapon attributes (e.g. `StunChance`, `StunDuration`), changed by ordinary upgrade GEs; an effect is active when its attribute is above 0 (provisional).
- Turrets: at placement, copy every attribute of the turret launcher, impact effects and `Projectile.Behavior.*` tags included (provisional). A turret's specs have the turret as instigator.

- entry_points: `ARRWeapon::Acquire(Miner)`, called by the miner after spawn and attach ([[sys-player]]): `InitAbilityActorInfo(weapon, weapon)`, base values from CSV row `StatsRowName` (editable, set in Blueprint), magazine filled, auto-fire configured (ticks after the miner) and started if `bStartFiringOnAcquire`. Upgrade replay: after the base values, the owning miner's all-weapons upgrades are re-applied ([[sys-upgrades]]). Player registry: not implemented. `GetDisplayName()` (CSV `DisplayName`, or the row name). `GetAimDirection()` = owning miner's aim (weapon forward without miner). `GetShotOrigin()`, `GetShotHeight()`, `GetShotForwardOffset()`. Getters for the CSV constants.
- data: `Content/RogueRigs/Data/Tables/WeaponStats.csv`, read at runtime with `FRRBaseStatsLoader::LoadTableFromCSV`; row `BurstRifle` = [[burst-rifle]] values; column `DisplayName` is the French UI name (`Fusil à rafales`, working name). Attribute columns become base values; `BurstCount`, `BurstDuty`, `ReloadTime`, `ProjectileSpeed`, `Range`, `SpreadDeg` stay constants on the weapon.
- depends_on: [[sys-gas-core]], [[sys-projectiles]], [[sys-player]] (aim).
- implements: [[burst-rifle]], [[shotgun]], [[turret]], [[upgrades]].
- gotchas: `AbilitySystem.DebugAttribute FireRate Damage` shows values above each weapon actor. Turret stats are frozen by attribute copy, not by spec snapshot.

## Relations
- [[systems-index]] — parent index.
- [[sys-upgrades]] — the upgrades applied to weapon ASCs and replayed on acquisition.
