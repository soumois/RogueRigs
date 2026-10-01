---
id: sys-weapons
description: "Weapon actors and their ASC: acquisition order, player registry, upgrade broadcast and replay, magazine and reload GEs, turret attribute copy."
parent: systems-index
kind: doc
---
# Weapons

- responsibility: weapon lifecycle, weapon stats, how upgrades reach weapons and turrets.
- main_classes: `ARRWeapon` (`FireOnce()` decides what is fired), `URRWeaponAttributeSet`, [[sys-auto-fire]] (decides when). Weapons: burst rifle, shotgun, turret launcher.

## Decisions
- One ASC per weapon; upgrades routed by `Weapon.Player.*` tags (approved).
- Magazine and reload by GEs `ConsumeAmmo`, `Reloading`, `RefillAmmo` (approved).
- Acquisition order (provisional): spawn and attach → `InitAbilityActorInfo(weapon, weapon)` → base values from CSV (`SetNumericAttributeBase`) → replay of global upgrades → registration in the player's registry.
- Global bonuses are broadcast to every weapon ASC, replayed on acquisition, never removed; the list of applied upgrades serves replay and debug (provisional).
- Player-state conditions (death, level transition) are read on the player's ASC; weapon ASCs do not carry those tags (provisional).
- 4-direction fire is a fire pattern inside `FireOnce()` (main shot + 3 weaker ones), not a projectile behavior (provisional).
- Shotgun: `FireOnce()` runs a cone query, keeps enemies with a clear grid line of sight to the miner, and applies the damage spec to each one directly (one critical roll per target); no projectile.
- Reload duration = base `ReloadTime` (CSV) / `ReloadSpeed`.
- Turret launcher: `MagazineSize` = turret charges; placing a turret starts recharging that charge at `ReloadSpeed`, one charge at a time; no cap on active turrets.
- Impact effects are weapon attributes (e.g. `StunChance`, `StunDuration`), changed by ordinary upgrade GEs; an effect is active when its attribute is above 0 (provisional).
- Turrets: at placement, copy every attribute of the turret launcher, impact effects and `Projectile.Behavior.*` tags included (provisional). A turret's specs have the turret as instigator.

- entry_points: acquisition, `FireOnce()`, upgrade application.
- data: one CSV row per weapon (fire rate, burst, magazine…).
- depends_on: [[sys-gas-core]], [[sys-projectiles]].
- implements: [[burst-rifle]], [[shotgun]], [[turret]], [[upgrades]].
- gotchas: `AbilitySystem.DebugAttribute FireRate Damage` shows values above each weapon actor. Turret stats are frozen by attribute copy, not by spec snapshot.

## Relations
- [[systems-index]] — parent index.
