---
id: sys-hud-health-bar
description: "URRHealthStatusWidget: C++ health bar and text panel, smooth drain of the shown fill, event-armed per-frame update; open to change the health bar."
parent: sys-hud
kind: doc
---
# HUD health bar

- responsibility: drive the health bar and text of the health status panel from the miner health events (task-203-01); the shown fill glides to the real health both ways. Vignette and GAME OVER stay in the Blueprint ([[wbp-health-status]]).
- main_classes: `URRHealthStatusWidget : URRHUDPanelWidget` (`Source/RogueRigs/UI/RRHealthStatusWidget.h`, Abstract), future parent of WBP_HealthStatus (reparent: task-203-07). `BindWidgetOptional` `HealthBar` (UProgressBar) and `HealthText` (UTextBlock): the class works on a Blueprint lacking them. Pure `GetShownFraction`, `GetTargetFraction`; `IsBarUpdateRunning` (C++, tests).
- entry_points: native bind `NativeBindToMiner` (new virtual of `URRHUDPanelWidget`, run by `SetMiner` just before the `BindToMiner` hook): removes its handlers from the previous miner, adds them (`AddUniqueDynamic`) to `OnHealthChanged` and `OnDamaged`, shows `GetCurrentHealth` / `GetMaxHealth` snapped (null miner: "0 / 0", fraction 0). A separate native step, not `BindToMiner_Implementation`, because a Blueprint override of the hook replaces the native implementation, so the vignette and game-over graphs keep running without a parent call. `NativeConstruct` rebinds (the Blueprint Destruct unbinds every listener of the miner) and draws.
- rules: text = "round(Current) / round(Max)" at once on each `OnHealthChanged`. Target = Current / Max clamped 0..1 (0 when Max <= 0). Shown += (Target - Shown) * min(1, DrainRate * dt), snapped when the gap is under `DrainSnapThreshold`. Fill colour = `GetBaseFillColor()` (`FillColor`).
- per-frame update: no tick. A health event arms `UpdateBarEffects` with `SetTimerForNextTick` (dt = world delta seconds); it re-arms each frame until `AreBarEffectsSettled`, then stops. Extension points for task-203-02 to 06: state in the class, start in `HandleDamaged` / `HandleHealthChanged`, step in `AdvanceBarEffects`, settle test in `AreBarEffectsSettled`, drawing in `ApplyBarVisuals`, colour in `GetBaseFillColor`.
- data: `EditDefaultsOnly` on the Blueprint subclass, provisional (owner 2026-10-16, reference video): `DrainRate` 10 per s, `DrainSnapThreshold` 0.001, `FillColor` (0.9, 0.05, 0.05, 1).
- depends_on: [[sys-hud-events]] (panel base, miner events), [[sys-player]] (miner).
- implements: [[task-203-health-bar-juice]] (health bar juice, no gdd node).
- gotchas: world timers do not run while the game is paused, so a change during the card screen glides after it closes. Until task-203-07 removes them, the Blueprint ShowHealth also writes the bar percent and text (also bound to OnHealthChanged), which would override the drain. Not seen on screen.

## Relations
- [[sys-hud]] — parent: the HUD root that binds the panels.
- [[sys-hud-events]] — the panel base and miner events it binds.
- [[wbp-health-status]] — the Blueprint that will derive from it.
- [[sys-player]] — the miner whose health it shows.
