---
id: task-343-03-shop-buy-juice-doc
description: "Doc: update wbp-shop-card."
parent: task-343-shop-buy-juice
kind: task
status: todo
---
# Shop purchase: juicy Buy button click: update wbp-shop-card

- goal: Update wbp-shop-card from the commits and change summaries of task-343-01 and 02; decided values in the global `request` and `gdd_check`.
- agent: doc
- depends_on: task-343-01-shop-buy-juice, task-343-02-shop-buy-juice-capture
- inputs: _authoring (vault/_authoring.md), wbp-shop-card (vault/reference/tech/blueprints/ui/wbp-shop-card.md) (to update), template `vault/templates/blueprint.md`
- expected_output: wbp-shop-card updated; one commit.
- done_when: node matches the commits; no value invented; lint clean; committed by path.
- editor_state: any
- token_ceiling: 0.3 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-343-shop-buy-juice]] — parent global task.
