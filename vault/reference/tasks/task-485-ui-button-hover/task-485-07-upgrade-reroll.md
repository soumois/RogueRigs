---
id: task-485-07-upgrade-reroll
description: "Bp: Same hover on the WBP_UpgradeChoice Reroll button; the cards keep their own hover."
parent: task-485-ui-button-hover
kind: task
status: in-progress
---
# Hover effect on every button of every screen: bp

- goal: Same hover on the WBP_UpgradeChoice Reroll button; the cards keep their own hover.
- agent: bp
- depends_on: task-485-06-shop-card
- inputs: ui-button-hover (vault/reference/gdd/art-audio/ui-button-hover.md), wbp-upgrade-choice (vault/reference/tech/blueprints/ui/wbp-upgrade-choice.md)
- expected_output: see done_when
- done_when: Reroll shows the hover of ui-button-hover after the deal, none while disabled; card hover, deal, rarity suspense and pick animation unchanged; compiled with warnings as errors, saved.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-07T03:02:52Z
- waiting_for: none
- result: open

## Relations
- [[task-485-ui-button-hover]] — parent global task.
