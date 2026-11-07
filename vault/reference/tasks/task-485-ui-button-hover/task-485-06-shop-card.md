---
id: task-485-06-shop-card
description: "Bp: Same hover on the Buy button of WBP_ShopCard, the card body unchanged."
parent: task-485-ui-button-hover
kind: task
status: in-progress
---
# Hover effect on every button of every screen: bp

- goal: Same hover on the Buy button of WBP_ShopCard, the card body unchanged.
- agent: bp
- depends_on: task-485-05-shop-screen
- inputs: ui-button-hover (vault/reference/gdd/art-audio/ui-button-hover.md), wbp-shop-card (vault/reference/tech/blueprints/ui/wbp-shop-card.md)
- expected_output: see done_when
- done_when: Buy shows the hover of ui-button-hover on all five cards, a disabled (NEED n MORE) or sold button shows none, purchase juice, burn and sold look still play; compiled with warnings as errors, saved.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-07T03:02:44Z
- waiting_for: none
- result: open

## Relations
- [[task-485-ui-button-hover]] — parent global task.
