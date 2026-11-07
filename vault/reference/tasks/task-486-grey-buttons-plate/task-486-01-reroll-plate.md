---
id: task-486-01-reroll-plate
description: "Bp: Level-up Reroll button of WBP_UpgradeChoice gets the cream plate look of ui-button-hover, with the standard plate hover."
parent: task-486-grey-buttons-plate
kind: task
status: in-progress
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
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-07T03:29:08Z
- waiting_for: none
- result: open

## Relations
- [[task-486-grey-buttons-plate]] — parent global task.
