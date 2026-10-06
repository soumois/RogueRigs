---
id: sys-hud-pressure
description: "HUD pressure events as built: damage vignette, horde banner, alert-bar icons (console, hordes, shuttle) and the horde icon position rule; open for the BP layer."
parent: sys-hud
kind: doc
---
# HUD pressure events

- responsibility: feed the pressure feedback of [[drgs-analysis]] (task-026-02): red damage vignette, "HORDE EN APPROCHE !" banner, icons of upcoming events on the alert bar. Visuals live in the HUD Blueprint ([[cpp-bp-split]]).
- main_classes: `URRHUDWidget` (`Source/RogueRigs/UI/RRHUDWidget.h`), `ERRAlertBarEventType` {Console, Horde, Extraction}, `FRRAlertBarEvent` {`Type`, `Position` 0..1 (alert scale), `bDone`, `Index` (horde index, -1 otherwise)}. The three events are `BlueprintNativeEvent`s (empty native default; implement them like the other display events), category `HUD|Pressure`:
  - `OnPlayerDamaged(float Amount, float HealthFraction)`: the miner's Health went down; `HealthFraction` = Health / MaxHealth after the hit (0 = dead). Not called for a heal, nor for the clamp of a MaxHealth decrease.
  - `OnHordeIncoming(float SecondsUntil)`: forwarded from `ARREnemySpawner::OnHordeIncoming` (banner, front in `SecondsUntil` s, 4 s by default, [[sys-spawner-director]]); pushed on binding during a banner.
  - `OnAlertEventsChanged(TArray<FRRAlertBarEvent> Events)`: order console, each horde by index, shuttle; on binding and whenever the list changes. Pure `GetAlertBarEvents()` returns the last list.
- position rule: console = `ConsoleAlertThreshold` (0.4), done when hacked (or extraction ready). Shuttle = 1, done at extraction ready. Horde, alert trigger: its `TriggerAlert`. Horde, run-time trigger (current): while pending, `Alert + SecondsUntilFront / AlertFillSeconds` clamped to [Alert, 1] (`EstimateBarPosition`): the base fill rate without kills or pause, so the icon is constant along the base fill, moves forward when kills speed the fill, slides back toward the fill while the bar is paused (console), and is never behind the fill. Done (front started, state Spawning or Done): frozen at the alert of that moment.
- entry_points: `BindToSpawner(ARREnemySpawner*)` (`ARRPlayerController::BeginPlay` binds the first spawner of the world; null unbinds; `NativeDestruct` unbinds); `RefreshAlertBarEvents()` runs on every bound run-flow and horde event and in `NativeTick`; a list is resent only when count, type, index or done flag changes, or a position moves more than `AlertEventPositionTolerance` (EditDefaultsOnly, 0.002).
- data: `AlertEventPositionTolerance` on the widget.
- depends_on: [[sys-hud]], [[sys-spawner-director]], [[sys-objectives]], [[sys-gas-core]].
- implements: pressure feedback of [[level-flow]] (provisional, task-026).
- gotchas: one spawner per level is assumed. Without the director (or no spawner) the list holds only console and shuttle. The horde estimate ignores the cheat alert multiplier. Tests: `RogueRigs.HUD.Pressure*`, and `RogueRigs.Integration.Pressure*` with the real WBP_HUD ([[sys-automation-tests]]).

## Relations
- [[sys-hud]] — parent: the HUD widget these events belong to.
- [[sys-spawner-director]] — the horde events and queries it reads.
- [[sys-objectives]] — alert, console and extraction state it reads.
- [[sys-automation-tests]] — `HUD.Pressure*` tests.
