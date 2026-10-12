---
id: wbp-weapons
description: "WBP_Weapons, HUD panel with the four weapon slot columns (name, ammo, blocks, reload, LOCKED) and the turret line, driven by the miner weapon slot event; open for its tree and graphs."
parent: blueprints-index
kind: doc
---
# WBP_Weapons

- asset_path: /Game/RogueRigs/UI/WBP_Weapons
- parent_class: URRHUDPanelWidget (/Script/RogueRigs.RRHUDPanelWidget, [[sys-hud-events]])
- purpose: shows the weapon slots and the turret charges; a sub-widget of [[wbp-hud]], which only places it (variable Weapons, HitTestInvisible, anchors 0.5/1 to 0.5/1, alignment 0.5/1, auto size, offset left 0, top -54: the row is centred on the screen and its bottom edge is 54 px above the screen bottom; owner wish 2026-10-11).
- components: VerticalBox Root > TurretBox (VerticalBox, Collapsed until a turret launcher is owned, centred, above the row; was at the left in WBP_HUD) = TurretText (Size 18, teal 0/0.85/1) + SizeBox 200 x 10 > TurretBar (teal); then SlotsRow (HorizontalBox) of four SizeBox Col0..3 (width 160) > ColBox<i> (VerticalBox) = WeaponSlot<i> (TextBlock 18, centred, shows the weapon name or LOCKED) + SizeBox 112 x 10 (centred) > Overlay > AmmoBar<i> (white-gold 1/0.9/0.5), ReloadBar<i> (red 1/0.1/0.1), BlockRow<i> (HorizontalBox centred) of 8 ProgressBars Blk<i>_0..7 (background 12 x 10, padding 1 left and right). The text, bars, rows, TurretBox, TurretText and TurretBar are variables; bars and rows start Collapsed.
- variables: TurretSlotPlusOne (int, slot index + 1 of the launcher shown in TurretBox, 0 = none).
- events: hook BindToMiner(Miner, PreviousMiner): UnbindWeapons(Previous); valid miner: assigns OnWeaponSlotChanged to OnWeaponSlotChanged_Event_0 and calls ShowSlot(i, GetWeaponSlotHUD(i)) for i 0..3 (bind-time read); null miner: ShowSlot with an empty struct (all LOCKED). OnWeaponSlotChanged_Event_0(SlotIndex, Slot) calls ShowSlot. It also covers ammo, reload and turret charge changes (the miner aggregates ARRWeapon::OnAmmoChanged, the reload events and OnTurretChargesChanged), so no other event is bound. Destruct calls UnbindWeapons(GetMiner).
- functions: ShowSlot(Index, Slot): starts the 0.016 s looping timer ProgressStep when the slot reloads or a launcher recharges; shows TurretBox with "Turrets: n / max" for an owned launcher (hides it when that slot stops being one); calls ShowSlotWidgets for the column of Index. ShowSlotWidgets(Slot, AmmoBar, ReloadBar, BlockRow, SlotText): not owned: LOCKED, all hidden; owned: name, reload bar while reloading (not launcher), AmmoBar when the magazine is above 7, else the block row (count = MaxCharges for a launcher or MagazineSize, filled = Charges or Rounds, the block at index Charges red at RechargeProgress while recharging). ProgressSlot(Slot, ReloadBar, BlockRow) returns bRunning: sets the reload bar percent while reloading and the recharging block and TurretBar percent while a launcher recharges. ProgressStep: calls ProgressSlot for the four slots (GetWeaponSlotHUD) and clears its own timer when none runs. UnbindWeapons(M): clears the timer, unbinds all OnWeaponSlotChanged listeners of a valid miner (named to avoid resolving to other widget functions).
- tick: none. Only the ProgressStep timer (0.016 s), started by a reload or recharge start and stopped by itself when none runs; replaces the old ~185-call per-frame update of WBP_HUD.
- logic_in_bp: display only.
- assets: none.
- owner_agent: agent-bp
- verified: compiled with warnings as errors, saved, graphs read back, anchors and offsets read back in WBP_HUD (see [[wbp-hud]]). Not seen on screen or in PIE; binding, timer start and stop and the turret line at run time are unverified. No C++ source looks up these widgets by name. Behaviour changes: the turret line is now centred above the row (was left), and shows when a launcher is owned (was after its first charge event).

## Relations
- [[blueprints-index]] — parent index.
- [[wbp-hud]] — places it as a child instance.
- [[sys-hud-events]] — the base class, OnWeaponSlotChanged and GetWeaponSlotHUD.
