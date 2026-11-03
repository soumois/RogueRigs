---
id: main-menu
description: "Main menu at game start: background image content and camera, layout, the three buttons (New Run, Options placeholder, Exit), start-up map. Open for menu work."
parent: run-index
kind: spec
status: provisional
---
# Main menu

All values provisional, owner 2026-10-28 ("ok" to Q1 to Q11 of [[task-337-main-menu-background]]).

## Background image
- One static 16:9 image, key art style ([[art-style-guide]], [[color-code]]), made by the art agent through Kie.ai GPT Image 2 image-to-image from `vault/assets/keyart.jpg`, `rigs-lineup.jpg` and `enemies-lineup.jpg`; 1K (cap kept), stretched to the screen.
- Content: the miner and his drone at the cliff edge of the arena asteroid, facing the void (indigo, violet, magenta nebula, stars, floating asteroids, a distant planet as in the key art; image only, the in-game void stays without planet); violet Duskite crystals in the rocks; a few corporate robots (yellow chasers, red eyes) coming over the rocks behind; left third calmer and darker for the menu column.
- Camera: three-quarter view about 35 degrees, lower than the game camera; [[art-style-guide]] A12 stays the rule for gameplay scene art.
- No text, no UI in the image.

## Layout
- Full-screen image with a dark gradient behind a left column: title "ROGUE RIGS" (drawn by the widget, never in the image; no game logo yet), then the buttons "New Run", "Options", "Exit".
- Labels in English, exactly these words, hardcoded in the widget (not in screen_texts.json).
- Font and colours as [[wbp-loading]]: Roboto Bold, white with outline, orange (1, 0.35, 0.1) on hover and focus.
- No animation, no music.

## Behaviour
- New Run: resets the run and opens L_ArenaM1 at level 1 ([[run-structure]]) behind the loading screen of the level changes ([[wbp-loading]], "DROPPING IN", "Level 1"): drawn over the menu before the map opens, kept in the arena until level 1 is ready (gameplay held as between levels), then a 0.5 s fade in from black. The map loading has no measured progress, so the loading animation of [[intermission]] replaces the bar (owner 2026-10-30, provisional, task-379).
- Options: shown greyed out, does nothing (built later).
- Exit: quits the game.
- Mouse cursor shown; keyboard focus on New Run (Enter starts a run).
- Start-up: a launch outside the editor (-game, packaged) opens the menu map L_MainMenu; the editor and PIE keep L_ArenaM1 and start a run directly.
- Run won (owner 2026-10-30, provisional, task-390 Q3, Q4): the button of the run-won end screen reads "Main Menu"; it resets the run (ore back to 0) and opens L_MainMenu directly (no loading screen, no fade), in the packaged game, -game and PIE alike. The run-lost screen keeps "Play again" (restart at level 1).
- Not in scope: a "back to menu" button on the run-lost screen or in pause.

## Relations
- [[run-index]] — parent index.
- [[run-structure]] — the run New Run starts.
- [[art-style-guide]] — rules the background follows.
- [[wbp-loading]] — font and colour reference.
- [[task-337-main-menu-background]] — the task that makes the image.
- [[task-338-main-menu-screen]] — the task that builds the screen.
