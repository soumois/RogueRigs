---
id: task-016-03-bp-objectives
description: "Console and shuttle visuals, marked-rock outline, hack gauge, alert bar and countdown in the HUD, success and run-lost screens, PIE."
parent: task-016-console-extraction
kind: task
status: done
---
# Objectives Blueprints and HUD

- goal: the objectives are visible and the loop is playable.
- agent: bp
- depends_on: task-016-02-shuttle
- inputs: [[objective-console]], [[extraction]], [[color-code]], [[readability-rules]], [[wbp-hud]], [[bp-drone]], [[unreal-mcp]], [[cpp-bp-split]].
- expected_output: grey-box console and shuttle Blueprints (white + teal), pulsing white outline on marked rocks, world gauge on the console, HUD alert bar and countdown, end screens with a "Rejouer" button calling restart; new Blueprint docs and [[wbp-hud]] updated. PIE (alert sped up with the cheat): console drops, rocks cleared, hack, shuttle lands, extraction success; and a loss by timeout; no log error.
- done_when: PIE checks done with no error; docs updated; lint clean.
- editor_state: open
- token_ceiling: 120000
- model: sonnet
- tokens: 204072
- runs: 4
- milestone: M1
- result: BP_ObjectiveConsole (teal glow grows with hack), BP_ExtractionShuttle (glow pulses each countdown second), M_GreyBoxGlow + MI_GB_Console/Shuttle, M_GreyBoxMarked (pulsing white emissive from per-instance custom data 0) as parent of the rock and ore instances; BP_RRGameMode RunFlowConfig console and shuttle classes; WBP_HUD alert bar ALERTE (pause), objective text and gauge, Navette countdown, EXTRACTION RÉUSSIE / RUN PERDUE with Rejouer. PIE via cheat properties: console drop and HUD text, 1 marked rock flagged, hack complete resumes alert at 0.40, shuttle lands with 45 s, extraction success screen, Rejouer reloads the map, loss with 5 s override; no runtime log error. Not seen on screen: outline pulse, glows, HUD layout; pause on the end screen not directly read. Docs bp-objective-console, bp-extraction-shuttle (new), wbp-hud, bp-rr-game-mode, bp-arena-grid, blueprints-index. Commits 9cd7b9a, 3ac962e, e0c007e; about 200k tokens over four runs.

## Relations
- [[task-016-console-extraction]] — parent global task.
