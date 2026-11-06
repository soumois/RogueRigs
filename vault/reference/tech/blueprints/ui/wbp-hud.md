---
id: wbp-hud
description: "WBP_HUD, widget Blueprint of URRHUDWidget: the single HUD root that only places the eight sub-widgets; placement table and z-order; open to move a HUD panel."
parent: blueprints-ui-index
kind: doc
---
# WBP_HUD

- asset_path: /Game/RogueRigs/UI/WBP_HUD
- parent_class: URRHUDWidget (/Script/RogueRigs.RRHUDWidget)
- purpose: single root of the in-run HUD; it only places the sub-widgets. Each sub-widget holds its own logic and binds the C++ events ([[sys-hud]], [[sys-hud-events]]). The C++ class initialises only this widget and calls the panel binds once on the children.
- contents (task-094-09): CanvasPanel Root and 8 sub-widget instances (all variables; OreCounter added in task-183-04). No variable, no function, no Tick; the event graph holds one event, OnEndScreenShown (task-211-02, see end screen below); the empty Construct and PreConstruct events were deleted. The C++ events OnHealthChanged, OnXPChanged, OnLevelUp, OnPlayerDamaged, OnAlertChanged, OnMinimapTextureReady and the others are not implemented here (harmless; the sub-widgets handle them).
- placement (reference screen 1920 x 1080; slot order = child index):

| child (class) | anchors | offsets (left, top, right=width, bottom=height) | align | z | visibility |
|---|---|---|---|---|---|
| HealthStatus ([[wbp-health-status]]) | 0,0 to 1,1 | 0,0,0,0 | 0,0 | 0 | HitTestInvisible |
| XPStatus ([[wbp-xp-status]]) | 0,0 to 1,1 | 0,0,0,0 | 0,0 | 0 | HitTestInvisible |
| Weapons ([[wbp-weapons]]) | 0.5,1 | 0,-78, auto size (task-461; was -54) | 0.5,1 | 0 | HitTestInvisible |
| Subtitles ([[wbp-subtitles]]) | 0.5,1 | 0,-290,900,80 (task-461; was -240, before that -190: clears the bottom panel and the turret line) | 0.5,1 | 0 | HitTestInvisible |
| UpgradeChoice ([[wbp-upgrade-choice]]) | 0,0 to 1,1 | 0,0,0,0 | 0,0 | 10 | SelfHitTestInvisible |
| AlertObjectives ([[wbp-alert-objectives]]) | 0,0 to 1,1 | 0,0,0,0 | 0,0 | 8 (was 20 in task-094-07) | SelfHitTestInvisible |
| MinimapLocator ([[wbp-minimap-locator]]) | 0,0 to 1,1 | 0,0,0,0 | 0,0 | 6 | HitTestInvisible |
| OreCounter ([[wbp-ore-counter]]) | 0,0 to 1,1 | 0,0,0,0 | 0,0 | 6 | HitTestInvisible |

- z-order rule: the cards screen (10) paints above the alert bar and the objective texts (8), which are above the minimap and locator (6) and the always-on panels (0). The end screen is inside WBP_AlertObjectives, so it is above everything except UpgradeChoice; this is correct because the run end closes any upgrade screen first ([[sys-hud-events]]), so the two never show together. A z-order of a canvas child orders the whole sub-widget, so an inner z cannot get above a sibling with a higher z.
- layout check (task-461, computed from the set values at 1920 x 1080, not seen on screen): bottom panel (HealthPanel of [[wbp-health-status]]) x 580..1340, y 846..1038, health bar y 872..896, weapons row x 642..1278, y 912..1002 inside it; turret line, when shown, y about 804..840 above the panel; subtitles y 710..790; XP frame (in [[wbp-xp-status]]) x 103..1913, y 1033..1070 over the panel bottom edge, Lv cap x 8..116, y 1030..1074. Draw order HealthStatus, XPStatus, Weapons: the XP frame covers the panel bottom rim, the slot wells are above the panel. All bottom-anchored, so the block keeps its place at other resolutions and ratios.
- layout check (task-457, computed, not seen on screen): radar frame x 16..540, y 16..313 (disc x 72..272, y 63..263 in its ring; ore icon and count in its plate, x 312..491, y 78..159); nothing floats beside the radar. Uses the top-left space task-452 freed; alert bar from x 660 unaffected.
- layout check (task-452-03, computed from read-back values at 1920 x 1080, not seen on screen): weapons row y 936..1026 (x 642..1278); health panel (inside HealthStatus, [[wbp-health-status]]) x 634..1286, y 888..932, 4 above the row; turret line, when shown, y about 852..888 just above the panel (TurretBox bottom padding 52, [[wbp-weapons]]); subtitles y 760..840; nothing left at the top left under the minimap. All bottom-centre anchored, so they keep their relative place at other resolutions and ratios.
- layout check (task-094-09, superseded for health, turret and subtitles by task-452; computed from the read-back values, not seen on screen): weapons row (task-106-01: cells 150 x 90, gap 12) x 642..1278, y 936..1026 with the turret line above it (y about 900..936); subtitles x 510..1410, y 810..890 (bottom edge 190 above the screen bottom, moved up by task-106-01); XP bar y 1044..1060 (x 90..1900), level text y 1038..; alert bar x 660..1260, y 48..84; objective texts y 140..236; minimap x 20..220, y 20..220; ore counter icon x 236..292, y 30..86 with the number from x 300 (right of the minimap, below the alert bar left edge at 660, above the health bar at 232); health bar x 20..300, y 232..260, text from 264. No overlap in the stack; the horde and event icons overlap the alert bar edge by design.
- end screen (task-211-02): event OnEndScreenShown(bRunWon, bGameOver) (C++ BlueprintImplementableEvent of URRHUDWidget, fired once per world on run won, shuttle missed and death, [[sys-hud]]) sets Collapsed on MinimapLocator, OreCounter, Weapons, XPStatus, Subtitles and UpgradeChoice, and on HealthStatus unless bGameOver. Why in WBP_HUD and not in each panel: these children need no logic of their own, one event of six setters is the smallest change. Exceptions: AlertObjectives stays (it draws the end screen) and hides its own non-end content itself in [[wbp-alert-objectives]]; on death HealthStatus stays because its GameOverText (GAME OVER) lives there, and it hides its own bar, text and vignette ([[wbp-health-status]]). Collapsed is never restored: the restart opens a new world with a fresh HUD.
- exposed_defaults: none.
- logic_in_bp: none.
- history: the former contents (cards, alerts, weapons, health, XP, minimap, locator, subtitles) moved to the sub-widget nodes in tasks 094-03 to 094-08; read the sub-widget node for any former behaviour.
- owner_agent: agent-bp
- verified: task-211-02: event graph read back (six setters, branch on bGameOver for HealthStatus), compiled with no log warning, saved; not run in PIE, not seen on screen.

## Relations
- [[blueprints-index]] — parent index.
- [[sys-hud]] — the C++ base class and events.
- [[wbp-health-status]] — child widget: health, game over, damage vignette.
- [[wbp-xp-status]] — child widget: XP bar and level.
- [[wbp-weapons]] — child widget: weapon slots with placeholder icons and padlock, ammo, reload, turret line.
- [[wbp-upgrade-choice]] — child widget: level-up cards and reroll.
- [[wbp-alert-objectives]] — child widget: alert bar, icons, horde banner, objective, shuttle, end screen.
- [[wbp-minimap-locator]] — child widget: minimap, markers, locator arrow and distance.
- [[wbp-ore-counter]] — child widget: carried ore icon and number right of the minimap.
- [[wbp-subtitles]] — child widget: subtitles.
- [[bp-rr-player-controller]] — creates it through HUDWidgetClass.
- [[hud-refactor-audit]] — the refactor audit and final structure.
