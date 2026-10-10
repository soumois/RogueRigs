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
- vignette warm-up (task-055-01): Slate culls a draw whose opacity is below 1e-4, so a vignette at RenderOpacity 0 never rendered M_HUDVignette before the first hit; its shaders and pipeline state were then not ready on that frame (in an editor `-game` run the material was still compiling, "Preparing Shaders") and the first flash was lost. The widget now looks up `DamageVignetteWidgetName` (WBP_HUD DamageVignette) at `NativeConstruct` (or on the first tick) and, after the Blueprint tick, raises its RenderOpacity to `VignetteWarmOpacity` (0.001: under half an 8-bit step, invisible) when lower, so the material renders from the start. Packaged Development check: first hit (rr.Debug.DamageMiner 30 at 6 s), shot 0.1 s later shows the red edge vignette (opacity 0.29 logged); a hit at frame 44 of a slow start (13 fps) showed none, so a long frame can still decay the flash (2 per s, Blueprint) before it is drawn. "No vignette" therefore reads 0.001, `GetVignetteWarmOpacity()`. No Blueprint change.
- alert lines (task-079-01, [[sys-text-content]]): each alert event has a Kessler-Voss line picked just before it is raised (no repeat of the previous line of that event): `horde_incoming` in `HandleHordeIncoming` before `OnHordeIncoming` (a banner pushed on binding keeps its line, picks one only when none exists); `console_dropped`, `bonus_chest_dropped`, `extraction_ready` in `URRRunFlowSubsystem` before their broadcast; `shuttle_countdown` once per countdown start (`LandShuttle`), not per countdown frame. Blueprint handlers read it with pure `GetAlertLine(EventId)` (empty = keep the functional text). No layout change: nothing displays it yet.
- data: `AlertEventPositionTolerance`, `DamageVignetteWidgetName`, `VignetteWarmOpacity` (EditDefaultsOnly, 0 disables) on the widget.
- depends_on: [[sys-hud]], [[sys-spawner-director]], [[sys-objectives]], [[sys-gas-core]].
- implements: pressure feedback of [[level-flow]] (provisional, task-026).
- gotchas: one spawner per level is assumed. Without the director (or no spawner) the list holds only console and shuttle. The horde estimate ignores the cheat alert multiplier. Tests: `RogueRigs.HUD.Pressure*`, and `RogueRigs.Integration.Pressure*` with the real WBP_HUD ([[sys-automation-tests]]).

## Relations
- [[sys-hud]] — parent: the HUD widget these events belong to.
- [[sys-spawner-director]] — the horde events and queries it reads.
- [[sys-objectives]] — alert, console and extraction state it reads.
- [[sys-automation-tests]] — `HUD.Pressure*` tests.
- [[sys-text-content]] — the alert lines read by `GetAlertLine`.
