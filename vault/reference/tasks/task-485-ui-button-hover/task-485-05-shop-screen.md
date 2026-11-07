---
id: task-485-05-shop-screen
description: "Bp: Same hover on WBP_ShopScreen Reroll and Continue."
parent: task-485-ui-button-hover
kind: task
status: todo
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
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-485-ui-button-hover]] — parent global task.
