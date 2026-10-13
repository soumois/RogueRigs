---
id: hud-refactor-audit-core
description: "HUD audit table part 1: health, game over, damage vignette, XP, level, weapon slots, ammo, reload, turret, per feature (widgets, driver, tick reads, events, placement)."
parent: hud-refactor-audit
kind: note
---
# HUD audit, part 1
Fields: widgets | updated by | Tick reads | event that can drive it | per-frame needed | placement (anchor, left/top/width/height, z) | sub-widget. Placement is on the Root canvas, anchor top-left unless said.

- Health bar and text: HealthBar, HealthText | OnHealthChanged(Current, Max) | none (UpdateVignette reads the bar percent) | exists in C++ only as an ASC attribute callback; `FRROnDamageReceived` is native, not bindable from Blueprint; missing: ARRMiner OnHealthChanged | no | bar 20,232,280x28; text 20,264,280x30 | WBP_HealthStatus.
- Game over: GameOverText | OnGameOver | none | exists: ARRGameMode::OnGameOver | no | anchor 0.5/0.5, auto, hidden | WBP_HealthStatus.
- Damage vignette: DamageVignette (Image, M_HUDVignette), vars HitFlash, LowHealthAlpha, HitHold* | OnPlayerDamaged(Amount, HealthFraction) sets HitFlash; UpdateVignette(DeltaTime) | yes: HitFlash decay, hold timer, HealthBar percent, SetRenderOpacity | missing: ARRMiner OnDamaged(Amount, HealthFraction) | only during the decay (about 0.5 s after a hit); the low-health tint follows HealthFraction by event | anchors 0,0-1,1, offsets 0, z 0 (first child), HitTestInvisible; C++ floor opacity 0.001 (KeepVignetteWarm) | WBP_HealthStatus.
- XP bar and level: XPBar, LevelText | OnXPChanged(Current, Needed, Level) | none | exists: URRProgressionComponent::OnXPChanged | no | bar anchors 0,1-1,1, left 90, top -36, right 20, h 16; text 20,-42 auto | WBP_XPStatus.
- Level-up flash: LevelUpText | OnLevelUp(NewLevel), Delay 1 s | none | exists: URRProgressionComponent::OnLevelUp | no | anchor 0.5/0.5, 0,-120, hidden | WBP_XPStatus.
- Weapon slot names: WeaponSlot0..3 | EventTick (names) and UpdateSlotAmmo (LOCKED) | yes: GetWeapons, array Length and Get, GetDisplayName x4 | exists: ARRMiner::OnWeaponAdded; missing: OnWeaponSlotChanged | no | anchor 0,1, left 76+170i, top -78, 160x24, alignment x 0.5 | WBP_Weapons.
- Ammo bar, ammo blocks, locked slots: AmmoBar<i> (112x10), BlockRow<i> with Blk<i>_0..7 | UpdateSlotAmmo(Index, ...) via UpdateAmmoHUD | yes: GetWeaponSlot(i) x4 (bOwned, Rounds, MagazineSize, Charges, MaxCharges, bIsTurretLauncher, bRecharging) | exists on URRAutoFireComponent: OnShotFired(ShotIndexInBurst), OnReloadFinished; missing: ARRWeapon OnAmmoChanged | no | anchor 0,1; bars left 76+170i, top -54; rows auto size, alignment x 0.5 | WBP_Weapons.
- Reload bar: ReloadBar<i> (112x10, red) | UpdateSlotAmmo | yes: ReloadProgress, bReloading | exists: URRAutoFireComponent OnReloadStarted(ReloadDuration), OnReloadFinished | yes, while a reload runs (progress moves each frame) | same slot as AmmoBar | WBP_Weapons.
- Turret charges: TurretText, TurretBar (200x10) | OnTurretChargesChanged(Charges, MaxCharges, RechargeDuration); the bar percent in EventTick | yes: GetTurretRechargeProgress | exists: ARRTurretLauncher::OnTurretChargesChanged | yes while recharging | anchor 0,1: text 20,-124 auto; bar 20,-100, 200x10; both hidden | WBP_Weapons.
- Launcher blocks (charges, recharge in the leftmost empty block): the same Blk widgets | UpdateSlotAmmo | yes: RechargeProgress | as turret charges | yes while recharging | as ammo | WBP_Weapons.

## Relations
- [[hud-refactor-audit]] — parent, summary and sub-widget list.
- [[hud-refactor-audit-run]] — part 2.
- [[hud-refactor-missing-events]] — the "missing" items.
