---
id: sys-hud-xp-bar
description: "URRXPStatusWidget: C++ XP bar fill and gain ghost (ghost at once, fill catches up, level-up pass); open to change the XP bar."
parent: sys-hud
kind: doc
---
# HUD XP bar

- responsibility: drive the XP bar of the XP status panel from the miner progression events (task-456-01): on each gain a pale ghost fill shows the new XP at once and the cyan fill catches up after a short delay, the mirror of the health bar damage chunk ([[sys-hud-health-bar]]). The level text stays in the Blueprint ([[wbp-xp-status]]).
- main_classes: `URRXPStatusWidget : URRHUDPanelWidget` (`Source/RogueRigs/UI/RRXPStatusWidget.h`, Abstract), parent of WBP_XPStatus (reparented by task-456-02). `BindWidgetOptional` `XPBar` (UProgressBar, fill) and `XPGhostBar` (UProgressBar, drawn behind `XPBar`, whose background must be transparent). Pure `GetFillFraction`, `GetGhostFraction`, `GetTargetFraction`; C++ `IsLevelUpPassPending`, `IsXPBarUpdateRunning`, `IsXPBarSettled`, `AdvanceXPBar(dt)` (public for the tests). Test subclass `URRTestXPStatusWidget` (`Tests/RRTestHUDWidget.h`).
- entry_points: `NativeBindToMiner` binds the progression `OnXPChanged` and `OnLevelUp` (`AddUniqueDynamic`), removes them from the previous miner, and shows the current XP snapped (no ghost at bind; null miner = empty bar). `NativeConstruct` rebinds and draws.
- rules: Target = XP / need clamped 0..1 (0 when the need is 0). Gain (`OnXPChanged`, same level): Ghost = Target at once, Fill unchanged (never above the ghost), delay = `CatchUpDelay` (each gain restarts it); after the delay Fill += (Ghost - Fill) * min(1, `CatchUpRate` * dt), snapped under `CatchUpSnapThreshold`. Level-up: `OnLevelUp` (broadcast before `OnXPChanged`) marks the next XP event; that event sets Ghost = 1 and starts a pass; once Fill reaches 1 the bar empties (Fill 0), Ghost = the remainder of the new level and the delay restarts, so the remainder shows the same way. Several levels in one gain, or a gain during a pass, make one pass to the end (no loop per level, never stuck). A level change without `OnLevelUp` (run carry-over `RestoreProgress`) snaps.
- per-frame update: no tick. An XP event arms `UpdateXPBar` with `SetTimerForNextTick` (world delta seconds); it re-arms until settled (no pass, Fill == Ghost, no delay left), then stops.
- data: `EditDefaultsOnly`, provisional (owner 2026-11-05, Q32 of [[task-456-xp-gain-ghost]]): `CatchUpDelay` 0.4 s and `CatchUpRate` 7 per s (same as the health chunk), `CatchUpSnapThreshold` 0.001, `GhostColor` #c8f6ff sRGB very pale cyan (`FLinearColor(FColor(0xC8, 0xF6, 0xFF))`, linear about 0.578 / 0.922 / 1); the retained mockup shows no ghost, so this colour is a reading of "cyan très clair". The fill colour (0 / 0.85 / 1) stays in the Blueprint.
- depends_on: [[sys-hud-events]] (panel base), [[sys-progression]] (XP events and getters).
- implements: [[xp-level-up]] (gain ghost).
- tests: `RogueRigs.HUD.XPGhost` (`Tests/RRHUDXPGhostTests.cpp`): bind snapped; gain ghost at once, fill waits 0.3 s, a second gain restarts the wait, fill reaches the ghost; level-up ghost 1, bar empties, remainder after the wait; several levels at once settle on the remainder.
- gotchas: world timers do not run while the game is paused, so a level-up pass started before the card screen plays after it closes. Not seen on screen (Q26: the owner checks the look in game).

## Relations
- [[sys-hud]] — parent: the HUD root that binds the panels.
- [[wbp-xp-status]] — the Blueprint deriving from it.
- [[sys-hud-health-bar]] — the damage chunk this ghost mirrors.
- [[sys-progression]] — the XP events it binds.
- [[sys-hud-events]] — the panel base.
