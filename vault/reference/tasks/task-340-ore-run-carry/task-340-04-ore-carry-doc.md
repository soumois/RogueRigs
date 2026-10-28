---
id: task-340-04-ore-carry-doc
description: "Doc: update sys-objectives-ore-counter, sys-run, wbp-ore-counter."
parent: task-340-ore-run-carry
kind: task
status: todo
---
# Ore kept for the whole run, reset at run end: update sys-objectives-ore-counter, sys-run, wbp-ore-counter

- goal: Update the doc nodes from the commits and change summaries of task-340-01 to 03; decided values in the global `request` and `gdd_check`.
- agent: doc
- depends_on: task-340-01-ore-run-reset, task-340-02-ore-counter-total, task-340-03-ore-carry-capture
- inputs: _authoring (vault/_authoring.md), sys-objectives-ore-counter (vault/reference/tech/systems/world/sys-objectives/sys-objectives-ore-counter.md) (to update), sys-run (vault/reference/tech/systems/world/sys-run.md) (to update), wbp-ore-counter (vault/reference/tech/blueprints/ui/wbp-ore-counter.md) (to update), templates `vault/templates/system.md`, `vault/templates/blueprint.md`
- expected_output: the three doc nodes updated; one commit.
- done_when: the nodes match the commits (bank reset point, counter total, capture result); no value invented; lint clean; committed by path.
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
- [[task-340-ore-run-carry]] — parent global task.
