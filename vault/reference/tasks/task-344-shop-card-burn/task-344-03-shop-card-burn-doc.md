---
id: task-344-03-shop-card-burn-doc
description: "Doc: update wbp-shop-card and m-card-burn."
parent: task-344-shop-card-burn
kind: task
status: done
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
- model: sonnet
- tokens: input 10, cache write 21218, cache read 75725, output 3227
- usage: turns 5, context start 10k peak 25k, resumes 0, 1 min, units 0.05 M
- runs: 1
- milestone: M2
- started: 2026-10-28T15:39:46Z
- waiting_for: none
- result: wbp-shop-card (BurnOverlay, vars, StartBurn/BurnStep/EndBurn, Tick branch, EndPurchaseFx contract, verified_burn) and m-card-burn (shop card as second user) updated; commit 6d56b0f1; lint OK; burn easing and fade shape left open (binary asset)

## Relations
- [[task-344-shop-card-burn]] — parent global task.
