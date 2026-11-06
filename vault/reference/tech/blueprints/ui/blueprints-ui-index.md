---
id: blueprints-ui-index
description: "WBP_HUD and its eight panels plus the HUD refactor audit."
parent: blueprints-index
kind: note
---
# Blueprints — ui

WBP_HUD and its eight panels plus the HUD refactor audit.

## Children
- [[wbp-hud]] — WBP_HUD, health bar and Game Over text.
- [[wbp-subtitles]] — WBP_Subtitles, the company subtitle line, a sub-widget of WBP_HUD.
- [[wbp-health-status]] — WBP_HealthStatus, health bar and text in a riveted plate just above the weapon slots, Game Over text and damage vignette, a sub-widget of WBP_HUD.
- [[wbp-xp-status]] — WBP_XPStatus, XP bar and level text, a sub-widget of WBP_HUD.
- [[wbp-weapons]] — WBP_Weapons, weapon slots, ammo, reload and turret line, a sub-widget of WBP_HUD.
- [[wbp-upgrade-choice]] — WBP_UpgradeChoice, level-up cards and reroll, a sub-widget of WBP_HUD.
- [[wbp-alert-objectives]] — WBP_AlertObjectives, alert bar and icons, horde banner, objective, shuttle and end screen, a sub-widget of WBP_HUD.
- [[wbp-minimap-locator]] — WBP_MinimapLocator, minimap in its riveted frame, markers and locator arrow with distance, a sub-widget of WBP_HUD.
- [[wbp-ore-counter]] — WBP_OreCounter, ore icon and number in the radar frame plate, a sub-widget of WBP_HUD.
- [[wbp-loading]] — WBP_Loading, full-screen loading screen between levels, not a sub-widget of WBP_HUD.
- [[wbp-intermission]] — WBP_Intermission, recap screen between run levels with a reserved shop area and the Continue button.
- [[wbp-intermission]] — WBP_Intermission, recap screen between run levels with a reserved shop area and the Continue button.
- [[wbp-shop-card]] — WBP_ShopCard, one priced stat card of the intermission shop, self-refreshing by SlotIndex, placed five times by WBP_Intermission.
- [[wbp-shop-screen]] — WBP_ShopScreen, the cards shop on its own opaque black screen after the stats screen (5 WBP_ShopCard, balance, reroll, Continue).
- [[wbp-shop-screen-indicator]] — IndicatorRessource of the shop, the ore counter: tree, anchors of icon and value, icon-font ratio, capture check.
- [[hud-refactor-audit]] — audit of WBP_HUD: tick cost, per-feature tables, sub-widget plan, missing C++ events.
- [[m-card-burn]] — M_CardBurn and MI_CardBurn, the dissolve material with fire edge for discarded cards.
- [[wbp-main-menu]] — WBP_MainMenu, the main menu screen: background, title, New Run, Options, Exit.
- [[wbp-pause-menu]] — WBP_PauseMenu, the pause screen: miner stats, weapon blocks, Resume and Exit, anchors table.

## Relations
- [[blueprints-index]] — parent index.
