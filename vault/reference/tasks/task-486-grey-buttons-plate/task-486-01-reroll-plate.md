---
id: task-486-01-reroll-plate
description: "Bp: Level-up Reroll button of WBP_UpgradeChoice gets the cream plate look of ui-button-hover, with the standard plate hover."
parent: task-486-grey-buttons-plate
kind: task
status: done
---
# Cream plate look for the two grey buttons (level-up Reroll, run-won Main Menu): bp

- goal: Level-up Reroll button of WBP_UpgradeChoice gets the cream plate look of ui-button-hover, with the standard plate hover.
- agent: bp
- depends_on: none
- inputs: ui-button-hover (vault/reference/gdd/art-audio/ui-button-hover.md), wbp-upgrade-choice (vault/reference/tech/blueprints/ui/wbp-upgrade-choice.md), wbp-run-lost (vault/reference/tech/blueprints/ui/wbp-run-lost.md), wbp-shop-screen (vault/reference/tech/blueprints/ui/wbp-shop-screen.md), sys-button-hover (vault/reference/tech/systems/ui/sys-button-hover.md), umg-layout (vault/reference/tech/conventions/umg-layout.md), Saved/Captures/task485_resA/c_lu.png
- expected_output: see done_when
- done_when: Reroll draws as the cream plate with "Reroll", ore icon and cost readable in dark brown; disabled it is greyed with no hover; enabled it uses the plate hover (brighter, 1.05) instead of the x1.25 grey tint; deal, pick, card hover and reroll click unchanged; compiled with warnings as errors and saved.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-486
- usage: included in the executor run of task-486
- runs: 1
- milestone: M2
- started: 2026-11-07T03:29:08Z
- waiting_for: none
- result: Done (98a84716). RerollButton wrapped in SizeBox RerollSize 440x110 (VerticalBox slot top 4, centred); WidgetStyle copied from WBP_RunLost MainMenuButton (T_Shop_ButtonContinue Box, margin 0.155, tints 1/1.3/0.7/0.35); RerollText and RerollCost Roboto Bold 28 dark brown (0.05,0.04,0.03), RerollOre 38x38; Panel canvas offset top 18.5 so cards and title stay put. Compiled (warnings as errors) and saved. Seen -game 1080 (Saved/Captures/task486_chk1/sheet.png): cream plate readable, hover brighter and 1.05 (plate material mode, rr.Debug.ButtonHover), disabled (0 Duskite) greyed and no hover, rr.Cheat.Reroll updates cost 15 to 20, cards and title unmoved.

## Relations
- [[task-486-grey-buttons-plate]] — parent global task.
