---
id: sys-auto-fire
description: "URRAutoFireComponent: accumulator deciding when weapons, turrets and enemy shooters fire; burst math from FireRate, BurstCount, BurstDuty."
parent: systems-combat-index
kind: doc
---
# Auto-fire

- responsibility: when to fire. The weapon decides what to fire (`FireOnce()`).
- main_classes: `URRAutoFireComponent` (`Source/RogueRigs/Weapons/`), one instance per weapon, turret or enemy shooter (approved). Replaces a `WaitDelay` ability loop.

## Burst timing (approved)
- `FireRate` = bursts per second, attribute on the weapon's ASC.
- Period = 1 / `FireRate`.
- Gap between two bullets = `BurstDuty` × period / (`BurstCount` − 1).
- Pause after the burst = period × (1 − `BurstDuty`).
- `BurstDuty` (share of the period used by the burst, 0 to 1) replaced `BurstInterval`, so the gap between bullets shrinks when fire rate rises.
- Example, [[burst-rifle]]: `FireRate` 1.25, `BurstCount` 3, `BurstDuty` 0.2 → bullets 0.08 s apart, 0.64 s pause. Upgraded to `FireRate` 3: 0.033 s and 0.267 s.
- [[shotgun]]: `BurstCount` 1, `BurstDuty` ignored.
- If the magazine empties mid-burst, the burst stops and the burst counter resets to 0.

## Implementation
- `Configure(ASC, BurstCount, BurstDuty, ReloadTime)` (ASC must own `URRWeaponAttributeSet`), `SetShotHandler` (owner's `FireOnce`), `StartFiring` / `StopFiring` (pause; cooldown and reload resume), `RefillAmmo`. `FireRate`, `Ammo`, `MagazineSize`, `ReloadSpeed` are read live on the ASC.
- Accumulator: `TickComponent` calls `AdvanceTime(Delta)` (public, for fixed-step tests); every shot and reload end due in the frame is processed in order and leftover time is carried, so timing is frame-rate independent. Intervals use the `FireRate` read at the previous shot. Events within 1 µs past the frame end fire in that frame.
- First shot as soon as firing starts. `BurstCount` 1 = one shot per period.
- Magazine: each shot applies `ConsumeAmmoEffect`. When `Ammo` < 1 the burst stops, the counter resets and a reload of `ReloadTime` / `ReloadSpeed` starts at once, replacing the pause; at its end `RefillAmmoEffect` fills the magazine and the next burst starts immediately. Unset effect class = direct base write. `ReloadSpeed` <= 0: reload never ends; `FireRate` <= 0: holds.
- Editable: `ConsumeAmmoEffect` (`URRGE_ConsumeAmmo`), `RefillAmmoEffect` (`URRGE_RefillAmmo`), `bUseMagazine` (false = infinite ammo, enemy shooters), `MaxEventsPerAdvance` (64, safety cap, overflow logged).
- Events: `OnShotFired(ShotIndexInBurst)`, `OnReloadStarted(Duration)`, `OnReloadFinished`. Getters: `IsFiring`, `IsReloading`, `GetShotsFiredInBurst`, `GetTotalShotsFired`, `GetTimeToNextShot`, `GetReloadTimeRemaining`, `GetReloadDuration`.

- data: `BurstCount`, `BurstDuty`, `ReloadTime` in the weapon's CSV row ([[sys-weapons]]).
- depends_on: [[sys-weapons]].
- implements: pillar P2 auto-fire, [[enemy-shooter]] fire cadence.
- gotchas: a reload replaces the pause after the burst that emptied the magazine, it is not added to it (provisional implementation choice). A reload refills to the current `MagazineSize`.

## Relations
- [[systems-index]] — parent index.
- [[sys-automation-tests]] — RogueRigs.Weapons.BurstTiming and MagazineReload prove these rules.
