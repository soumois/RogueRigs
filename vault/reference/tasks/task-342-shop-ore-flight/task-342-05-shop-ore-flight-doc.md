---
id: task-342-05-shop-ore-flight-doc
description: "Doc: update sys-run-shop, wbp-shop-card, wbp-shop-screen."
parent: task-342-shop-ore-flight
kind: task
status: todo
---
# Shop purchase: ore flies from the balance to the Buy button: update sys-run-shop, wbp-shop-card, wbp-shop-screen

- goal: Update the doc nodes from the commits and change summaries of task-342-01 to 04; decided values in the global `request` and `gdd_check`.
- agent: doc
- depends_on: task-342-01-shop-bought-event, task-342-02-shop-card-hold, task-342-03-shop-ore-flight, task-342-04-shop-ore-flight-capture
- inputs: _authoring (vault/_authoring.md), sys-run-shop (vault/reference/tech/systems/world/sys-run/sys-run-shop.md) (to update), wbp-shop-card (vault/reference/tech/blueprints/ui/wbp-shop-card.md) (to update), wbp-shop-screen (vault/reference/tech/blueprints/ui/wbp-shop-screen.md) (to update), templates `vault/templates/system.md`, `vault/templates/blueprint.md`
- expected_output: the three doc nodes updated; one commit.
- done_when: nodes match the commits (OnSlotBought, EndPurchaseFx contract, flight values); no value invented; lint clean; committed by path.
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
- [[task-342-shop-ore-flight]] — parent global task.
