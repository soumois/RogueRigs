---
id: task-396-01-shop-short-label
description: "Bp: WBP_ShopScreen: ShopCard0..4 ShortLabel "Short on ore" becomes "Short on Duskite"; read WBP_ShopCard runtime texts"
parent: task-396-duskite-leftover-labels
kind: task
status: todo
---
# Duskite: leftover on-screen "ore" labels: bp

- goal: In WBP_ShopScreen set the ShortLabel override of ShopCard0 to ShopCard4 from "Short on ore" to "Short on Duskite"; nothing else changes. Then read WBP_ShopCard (ShortLabel default, every TextBlock default, the graph that builds the card texts at runtime: format strings, appended literals) and report any player-visible "ore"; change WBP_ShopCard only if such a literal exists there (same rule, "Duskite"), never its number sources or bindings. Editor tooltips and variable names keep "ore".
- agent: bp
- depends_on: task-393-01-miner-material-scale
- inputs: agent-bp (vault/reference/agents/agent-bp.md), wbp-shop-screen (vault/reference/tech/blueprints/ui/wbp-shop-screen.md), wbp-shop-card (vault/reference/tech/blueprints/ui/wbp-shop-card.md), sys-run-shop (vault/reference/tech/systems/world/sys-run/sys-run-shop.md), ore-blocks (vault/reference/gdd/terrain/ore-blocks.md), global request and gdd_check: vault/reference/tasks/task-396-duskite-leftover-labels.md
- expected_output: WBP_ShopScreen saved (and WBP_ShopCard if a literal was found), compiled with warnings as errors; commit `task-396-01-shop-short-label: ...`; change summary with the WBP_ShopCard check result, for the doc subtask.
- done_when: ShortLabel of ShopCard0..4 read back "Short on Duskite"; no "Short on ore" string left in the package; WBP_ShopCard check reported; compiled with warnings as errors, no error; saved; committed by path.
- editor_state: open
- token_ceiling: 1.0 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-396-duskite-leftover-labels]] — parent global task.
