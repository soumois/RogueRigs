---
id: task-056-02-bp-bonus-chest
description: "BP_BonusChest look, its gold alert-bar icon in WBP_HUD and its hack visual; PIE."
parent: task-056-bonus-chest
kind: task
status: done
---
# Bonus chest Blueprint

- goal: the bonus chest is visible and readable in game.
- agent: bp
- depends_on: task-056-01-bonus-chest
- inputs: objective-bonus-chest (vault/reference/gdd/run/objective-bonus-chest.md), color-code (vault/reference/gdd/art-audio/color-code.md), wbp-hud (vault/reference/tech/blueprints/wbp-hud.md), bp-objective-console (vault/reference/tech/blueprints/bp-objective-console.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), sys-cheats (vault/reference/tech/systems/sys-cheats.md)
- expected_output: BP_BonusChest child of ARRBonusChest (grey-box chest, gold accents, hack ring like the console's), set on the game mode or run flow config as task-056-01 says; WBP_HUD alert-bar icon for the chest event (gold, "B" or a chest glyph), faded when done; docs: new bp-bonus-chest node, wbp-hud; PIE: no log error, chest spawns at 60 % alert (alert speed cheat), hack completes, reward events fire. From task-056-01: derive BP_BonusChest from ARRBonusChest (Mesh component), assign BP_RRGameMode RunFlowConfig.BonusChestClass; events OnHackProgress(Progress, bHacking), OnHackStateChanged(NewState), OnRewardGranted(XP, OreChunks, bUpgradeChoiceQueued), OnMarkedRocksChanged (ignore); getters WasRewardGranted, GetRewardXP, GetHackProgress, GetZoneRadius, IsComplete; WBP_HUD OnAlertEventsChanged: add the BonusChest case (ERRAlertBarEventType::BonusChest) placing a gold icon with PlaceIcon (Position × 300, done 0.35 opacity).
- done_when: PIE check with no error; docs updated; lint clean.
- editor_state: open
- token_ceiling: 80000
- model: sonnet
- tokens: input 114, cache write 81653, cache read 3134770, output 27590
- runs: 2
- milestone: M2
- result: BP_BonusChest (child of ARRBonusChest, gold cube, gold hack ring, glow on OnRewardGranted), MI_GB_BonusChest, MI_GroundRingGold; BP_RRGameMode RunFlowConfig.BonusChestClass set (recompile needed after editing); WBP_HUD gold 'B' AlertIconChest with the BonusChest case. PIE: chest dropped at 60 % alert, no log error; hack completion and rewards not exercised (no cheat targets the chest); not seen on screen. Docs bp-bonus-chest (new), wbp-hud, bp-rr-game-mode, blueprints-index. Commits 12bd69e, e7e20f2, a90285c.

## Relations
- [[task-056-bonus-chest]] — parent global task.
