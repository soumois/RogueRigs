---
id: task-485-05-shop-screen
description: "Bp: Same hover on WBP_ShopScreen Reroll and Continue."
parent: task-485-ui-button-hover
kind: task
status: done
---
# Hover effect on every button of every screen: bp

- goal: Same hover on WBP_ShopScreen Reroll and Continue.
- agent: bp
- depends_on: task-485-04-intermission
- inputs: ui-button-hover (vault/reference/gdd/art-audio/ui-button-hover.md), wbp-shop-screen (vault/reference/tech/blueprints/ui/wbp-shop-screen.md)
- expected_output: see done_when
- done_when: Reroll and Continue show the hover of ui-button-hover, a greyed Reroll shows none, ore flight and input block unchanged; compiled with warnings as errors, saved.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-485
- usage: included in the executor run of task-485
- runs: 1
- milestone: M2
- started: 2026-11-07T03:02:17Z
- waiting_for: none
- result: Done (be7c1c32). URRShopScreenWidget::NativeOnInitialized attaches RerollButton and ContinueButton; a greyed Reroll (SetIsEnabled false) is skipped by the subsystem. Build succeeded; no asset change. Seen in -game: Reroll (dark teal plate, brighter step subtle) and Continue grow and brighten on hover at the four resolutions (Saved/Captures/task485_b1080/c_shopbtn.png, task485_resB/sheet_sh.png, task485_resB1200). Greyed Reroll hovered not captured (no state with balance below 15 and a shop); ore flight and input block code untouched. Budget ALERT at closing of 04 (114 turns, 2.47 M units vs 90 / 1.5 M): eight widgets, a new material and C++ mechanism, the editor Save Content dialog, MCP through a JSON-RPC script, and multi-resolution capture flows.

## Relations
- [[task-485-ui-button-hover]] — parent global task.
