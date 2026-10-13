---
id: wbp-hud
description: "WBP_HUD, widget Blueprint of URRHUDWidget: the single HUD root that only places the seven sub-widgets; placement table and z-order; open to move a HUD panel."
parent: blueprints-ui-index
kind: doc
---
# WBP_HUD

- asset_path: /Game/RogueRigs/UI/WBP_HUD
- parent_class: URRHUDWidget (/Script/RogueRigs.RRHUDWidget)
- purpose: single root of the in-run HUD; it only places the sub-widgets. Each sub-widget holds its own logic and binds the C++ events ([[sys-hud]], [[sys-hud-events]]). The C++ class initialises only this widget and calls the panel binds once on the children.
- contents (task-094-09): CanvasPanel Root and 7 sub-widget instances (all variables). No variable, no function, no graph node: the event graph is empty (the empty Construct and PreConstruct events were deleted), no Tick. The C++ events OnHealthChanged, OnXPChanged, OnLevelUp, OnPlayerDamaged, OnAlertChanged, OnMinimapTextureReady and the others are not implemented here (harmless; the sub-widgets handle them).
- placement (reference screen 1920 x 1080; slot order = child index):

| child (class) | anchors | offsets (left, top, right=width, bottom=height) | align | z | visibility |
|---|---|---|---|---|---|
| HealthStatus ([[wbp-health-status]]) | 0,0 to 1,1 | 0,0,0,0 | 0,0 | 0 | HitTestInvisible |
| XPStatus ([[wbp-xp-status]]) | 0,0 to 1,1 | 0,0,0,0 | 0,0 | 0 | HitTestInvisible |
| Weapons ([[wbp-weapons]]) | 0.5,1 | 0,-54, auto size | 0.5,1 | 0 | HitTestInvisible |
| Subtitles ([[wbp-subtitles]]) | 0.5,1 | 0,-190,900,80 | 0.5,1 | 0 | HitTestInvisible |
| UpgradeChoice ([[wbp-upgrade-choice]]) | 0,0 to 1,1 | 0,0,0,0 | 0,0 | 10 | SelfHitTestInvisible |
| AlertObjectives ([[wbp-alert-objectives]]) | 0,0 to 1,1 | 0,0,0,0 | 0,0 | 8 (was 20 in task-094-07) | SelfHitTestInvisible |
| MinimapLocator ([[wbp-minimap-locator]]) | 0,0 to 1,1 | 0,0,0,0 | 0,0 | 6 | HitTestInvisible |

- z-order rule: the cards screen (10) paints above the alert bar and the objective texts (8), which are above the minimap and locator (6) and the always-on panels (0). The end screen is inside WBP_AlertObjectives, so it is above everything except UpgradeChoice; this is correct because the run end closes any upgrade screen first ([[sys-hud-events]]), so the two never show together. A z-order of a canvas child orders the whole sub-widget, so an inner z cannot get above a sibling with a higher z.
- layout check (task-094-09, computed from the read-back values, not seen on screen): weapons row (task-106-01: cells 150 x 90, gap 12) x 642..1278, y 936..1026 with the turret line above it (y about 900..936); subtitles x 510..1410, y 810..890 (bottom edge 190 above the screen bottom, moved up by task-106-01); XP bar y 1044..1060 (x 90..1900), level text y 1038..; alert bar x 660..1260, y 48..84; objective texts y 140..236; minimap x 20..220, y 20..220; health bar x 20..300, y 232..260, text from 264. No overlap in the stack; the horde and event icons overlap the alert bar edge by design.
- exposed_defaults: none.
- logic_in_bp: none.
- history: the former contents (cards, alerts, weapons, health, XP, minimap, locator, subtitles) moved to the sub-widget nodes in tasks 094-03 to 094-08; read the sub-widget node for any former behaviour.
- owner_agent: agent-bp

## Relations
- [[blueprints-index]] — parent index.
- [[sys-hud]] — the C++ base class and events.
- [[wbp-health-status]] — child widget: health, game over, damage vignette.
- [[wbp-xp-status]] — child widget: XP bar, level, LEVEL UP flash.
- [[wbp-weapons]] — child widget: weapon slots with placeholder icons and padlock, ammo, reload, turret line.
- [[wbp-upgrade-choice]] — child widget: level-up cards and reroll.
- [[wbp-alert-objectives]] — child widget: alert bar, icons, horde banner, objective, shuttle, end screen.
- [[wbp-minimap-locator]] — child widget: minimap, markers, locator arrow and distance.
- [[wbp-subtitles]] — child widget: subtitles.
- [[bp-rr-player-controller]] — creates it through HUDWidgetClass.
- [[hud-refactor-audit]] — the refactor audit and final structure.
