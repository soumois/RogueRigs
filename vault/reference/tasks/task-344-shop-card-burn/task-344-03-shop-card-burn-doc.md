---
id: task-344-03-shop-card-burn-doc
description: "Doc: update wbp-shop-card and m-card-burn."
parent: task-344-shop-card-burn
kind: task
status: in-progress
---
# Shop purchase: the bought card burns like a level-up pick: update wbp-shop-card and m-card-burn

- goal: Update wbp-shop-card (and m-card-burn users) from the commits and change summaries of task-344-01 and 02; decided values in the global `request` and `gdd_check`.
- agent: doc
- depends_on: task-344-01-shop-card-burn, task-344-02-shop-card-burn-capture
- inputs: _authoring (vault/_authoring.md), wbp-shop-card (vault/reference/tech/blueprints/ui/wbp-shop-card.md) (to update), m-card-burn (vault/reference/tech/blueprints/ui/m-card-burn.md) (to update: add the shop card as a user), template `vault/templates/blueprint.md`
- expected_output: the two nodes updated; one commit.
- done_when: nodes match the commits; no value invented; lint clean; committed by path.
- editor_state: any
- token_ceiling: 0.3 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-28T15:39:46Z
- waiting_for: none
- result: open

## Relations
- [[task-344-shop-card-burn]] — parent global task.
