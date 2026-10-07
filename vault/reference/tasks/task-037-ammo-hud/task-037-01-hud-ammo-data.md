---
id: task-037-01-hud-ammo-data
description: "HUD data for the weapon slots: rounds left, magazine size, reload progress per weapon; turret charges and recharge progress; slot owned or locked."
parent: task-037-ammo-hud
kind: task
status: done
---
# Ammo data for the HUD

- goal: the HUD can show ammo, reload and turret recharge per slot.
- agent: cpp
- depends_on: none
- inputs: [[readability-rules]], [[sys-hud]], [[sys-weapons]], [[sys-turret]], [[sys-automation-tests]], [[cpp-standard]]
- expected_output: Blueprint-readable getters on URRHUDWidget (or the weapons) per slot index: owned or locked, rounds left, magazine size, reloading flag, reload progress 0..1; turret launcher charges, max charges, recharge progress 0..1; exact names listed for task-037-02; update only existing tests that break; docs sys-hud.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 50000
- model: opus
- tokens: input 30, cache write 56197, cache read 591551, output 11179
- runs: 1
- milestone: M2
- result: URRHUDWidget GetWeaponSlotCount, GetWeaponSlots, GetWeaponSlot(SlotIndex) returning FRRWeaponSlotHUD (bOwned, Weapon, bIsTurretLauncher, Rounds, MagazineSize, bReloading, ReloadProgress, Charges, MaxCharges, bRecharging, RechargeProgress); slots in acquisition order, locked after the owned ones; static MakeWeaponSlot. No test calls them (no test added, owner rule). Build 0, 195/195. Commit a532e44.

## Relations
- [[task-037-ammo-hud]] — parent global task.
