---
id: task-396-03-doc-duskite-leftovers
description: "Doc: update the shop screen, shop card and end screen docs with the Duskite labels"
parent: task-396-duskite-leftover-labels
kind: task
status: todo
---
# Duskite: leftover on-screen "ore" labels: doc

- goal: Record the new label texts and the WBP_ShopCard check result.
- agent: doc
- depends_on: task-396-01-shop-short-label, task-396-02-end-ore-text-default
- inputs: agent-doc (vault/reference/agents/agent-doc.md), wbp-shop-screen (vault/reference/tech/blueprints/ui/wbp-shop-screen.md), wbp-shop-card (vault/reference/tech/blueprints/ui/wbp-shop-card.md), wbp-alert-objectives (vault/reference/tech/blueprints/ui/wbp-alert-objectives.md), global request and gdd_check: vault/reference/tasks/task-396-duskite-leftover-labels.md
- expected_output: The listed doc nodes updated; commit `task-396-03-doc-duskite-leftovers: ...`.
- done_when: The nodes state "Short on Duskite", "Duskite secured: 0" and the shop card check; lint clean; committed by path.
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
- [[task-396-duskite-leftover-labels]] — parent global task.
