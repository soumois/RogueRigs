---
id: task-061-minimap
description: "Global task: Round minimap at the top left: seen cells of rock, floor and heal veins, markers for miner, console, chest and shuttle."
parent: tasks-done-index
kind: task
status: done
---
# Minimap

- request: "ajouter la mini carte" (owner, 2026-10-07)
- goal: Round minimap at the top left: seen cells of rock, floor and heal veins, markers for miner, console, chest and shuttle.
- acceptance: build exits 0 with the editor closed; all `RogueRigs*` tests pass headless; docs updated; lint clean.; committed.
- gdd_check: GDD updated by the Orchestrator (owner 2026-10-07, fifth test). No open value.
- confirmed_by_owner: yes (2026-10-07)
- milestone: M2
- next_step: none
- result: Done. M_Minimap (UI translucent, MinerUV, ViewRadiusUV, MinimapTex, circular mask, dark unseen disc); WBP_HUD MinimapImage 200 × 200 at (20, 20), markers miner, console, chest, shuttle placed by GetMinimapOffset each tick, texture bound once on OnMinimapTextureReady through the brush dynamic material; HealthBar and HealthText moved down to 232 and 264. PIE: no runtime error; drawing not seen on screen. Doc wbp-hud. Commits 551716c, 3988940, 9e130c1, 5495033.

## Subtasks
1. task-061-01-minimap — cpp
2. task-061-02-bp-minimap — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-061-01-minimap]] — step 1, cpp.
- [[task-061-02-bp-minimap]] — step 2, bp.
