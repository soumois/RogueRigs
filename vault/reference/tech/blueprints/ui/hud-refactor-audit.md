---
id: hud-refactor-audit
description: "Audit of WBP_HUD (task-094-01): Tick graph cost, proposed sub-widget list, centred weapon row; open before splitting the HUD or planning its C++ events."
parent: blueprints-ui-index
kind: note
---
# HUD refactor audit (task-094-01)

Source: WBP_HUD as saved (graphs through MCP read_graph_dsl, 116 widgets, slots read with get_properties), URRHUDWidget (`RRHUDWidget.h/.cpp`), [[wbp-hud]]. No asset or C++ was changed. Not read: Tick cost in ms (not measured; the 0.4 ms comes from [[perf-trace-real-game]]), card sub-graphs beyond SetCard0 and StyleCard0 (identical per index), anything on screen.

## What WBP_HUD does today
- 116 widgets in one Root canvas (48 direct children), 18 graphs: EventGraph, 5 per-tick functions, card functions, PlaceIcon, PlaceMarker, flavor helpers.
- Display events are already C++-driven (BlueprintImplementableEvents fed by Bind functions). The waste is the BP `EventTick`, which re-pushes values that already have events.
- Per-tick work in the BP `EventTick`, in order:
  1. 3x GetOwningPlayerPawn + CastToRRMiner, GetWeapons.
  2. UpdateVignette: GetHealthBar percent, hold and decay maths, SetRenderOpacity.
  3. UpdateLocator_0: GetObjectiveLocator (pure, evaluated 2 to 3 times), 2 SetVisibility; off screen also 3 transforms, sin, GetActorLocation, distance, SetText, colour switch (about 14 calls).
  4. UpdateAmmoHUD: 4x UpdateSlotAmmo. Locked slot: 4 calls. Owned slot: GetWeaponSlot, 2 SetPercent, 3 SetVisibility, then 8 blocks x (GetChildAt, Cast, SetVisibility, SetPercent, SetFillColor) = about 46 calls. Up to about 185 calls for 4 slots.
  5. UpdateMinimap: GetMinimapState x2, 2 material parameter sets, 3 PlaceMarker (GetMinimapOffset, SetVisibility, SetRenderTranslation).
  6. TurretBar SetPercent (GetTurretRechargeProgress).
  7. 4x SetText of the slot names (4 array Length, 4 Get, GetDisplayName), rewritten every tick although UpdateSlotAmmo already writes LOCKED.
- Total: about 250 node executions per tick with all slots owned, 30 to 60 with locked slots (counted from the DSL, not measured). Nearly all write unchanged values.
- C++ `NativeTick` also runs each frame: RefreshAlertBarEvents (recomputes the icon list), KeepVignetteWarm, UpdateMinimap (Minimap->Tick). Not in the Blueprint figure.

## Proposed sub-widgets
All children of WBP_HUD (which only places them); each binds C++ events itself (model [[wbp-subtitles]]).
1. WBP_HealthStatus: health bar and text, game over text, damage vignette (hit flash, low-health tint).
2. WBP_XPStatus: XP bar, level text, LEVEL UP flash.
3. WBP_Weapons: four slot columns (name, ammo bar, blocks, reload bar, LOCKED), turret charges line; owns the only per-frame value (reload and recharge progress, only while one runs).
4. WBP_UpgradeChoice: UpgradeScreen (cards, reroll), full screen, z 10. [[wbp-hud]] says a separate widget could not be called from the HUD graph through MCP; with the sub-widget binding its own C++ events this may no longer apply: verify.
5. WBP_AlertObjectives: alert bar and label, event icons, horde banner, objective and hack texts, shuttle text, end screen (z 20).
6. WBP_MinimapLocator: minimap, markers, locator arrow and distance; per frame only while the minimap or a target exists.
7. WBP_Subtitles: done.

Detail per feature: [[hud-refactor-audit-core]], [[hud-refactor-audit-run]]. New C++ events: [[hud-refactor-missing-events]].

## Weapon info row, centred
Today: four slot texts (160 x 24, alignment x 0.5, anchor bottom-left 0/1, left 76 + 170 i, top -78), bars and blocks at top -54 (112 x 10) on the same centres. The row spans x -4 to 666 from the left edge, so it sits bottom-left and slot 0's box starts 4 px off screen.
Proposal: in WBP_Weapons a HorizontalBox of four slot columns (each a 160 wide SizeBox with a VerticalBox: name, then bar or blocks, centred), the sub-widget anchored bottom centre (0.5/1), alignment (0.5, 1), auto size, bottom edge 54 above the screen bottom (name row at about -78 as now). The subtitles (bottom centre, 900 x 80, bottom 50) would then overlap it: raise the subtitles or the row; to decide with the owner (open). The turret line (left 20, top -124 and -100) moves into the sub-widget above the row.

## Final structure (task-094-09)
- WBP_HUD: Root canvas and 7 sub-widget instances only (HealthStatus, XPStatus, Weapons, Subtitles z 0; MinimapLocator z 6; AlertObjectives z 8; UpgradeChoice z 10); no variable, function, graph node or Tick. Table in [[wbp-hud]]. Moved: health, game over and vignette to [[wbp-health-status]]; XP and LEVEL UP to [[wbp-xp-status]]; weapon row and turret line to [[wbp-weapons]] (bottom centre, bottom edge 54); cards to [[wbp-upgrade-choice]]; alert bar, icons, banner, objectives, shuttle and end screen to [[wbp-alert-objectives]]; minimap, markers and locator to [[wbp-minimap-locator]]; subtitles above the weapons row (bottom edge 140).
- Still ticking: the reload and recharge progress timer in Weapons (only while one runs), the vignette fade timer in HealthStatus, the arrow pulse timer in MinimapLocator (only while the arrow is shown off screen). The always-on minimap timer and the texture polling were removed by task-103-02 ([[wbp-minimap-locator]]: the panel binds OnMinimapTextureReady, OnMinimapViewChanged and OnLocatorViewChanged through BindToHUD). C++ NativeTick in URRHUDWidget: emptied by task-103-01 (no override): the vignette warm-up is one-shot, the minimap and the locator follow the miner's and its camera's TransformUpdated through the delegates `OnMinimapTextureReady`, `OnMinimapViewChanged`, `OnLocatorViewChanged` ([[sys-hud-events]]). Blueprint side pending (task-103-02): WBP_MinimapLocator binds them in `BindToHUD` and drops its 0.016 s timer; then only the reload / recharge progress, the vignette fade while it runs and the arrow pulse stay per frame.
- C++ lookups (task-094-10, done): every lookup by name of C++, debug commands and tests goes through `URRHUDWidget::FindHUDWidget` / `FindWidgetInTree` (searches the panels) and `FindUserWidgetWithProperty` for Blueprint variables ([[sys-hud]]); the unimplemented display events on WBP_HUD are kept and documented there. Suite after the fix: 236 of 237 pass; the FadeStep zero dt and the XP divide by zero were fixed in task-104-01 (PressureVignetteOnHit passes in the task-103-01 run).
- Z-order: AlertObjectives lowered from 20 to 8 so the cards (10) are above the alert bar; the end screen lives inside AlertObjectives, so it is above everything except the cards, which never coexist with it (the run end closes the cards first).
- Weapons (bottom 54) and subtitles (bottom 140) do not overlap, which closes the open point of the proposal above.

## Relations
- [[blueprints-index]] — parent index.
- [[hud-refactor-audit-core]] — table: health, game over, vignette, XP, weapons, turret.
- [[hud-refactor-audit-run]] — table: cards, alerts, objectives, minimap, locator.
- [[hud-refactor-missing-events]] — events to add in C++ (task 094-02).
- [[wbp-hud]] — the audited widget.
- [[sys-hud]] — the C++ base class.
- [[perf-trace-real-game]] — the 0.4 ms Blueprint tick measurement.
