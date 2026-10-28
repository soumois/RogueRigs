---
id: task-340-04-ore-carry-doc
description: "Doc: update sys-objectives-ore-counter, sys-run, wbp-ore-counter."
parent: task-340-ore-run-carry
kind: task
status: done
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
- model: sonnet
- tokens: input 10, cache write 39469, cache read 328175, output 3815
- usage: turns 5, context start 60k peak 82k, resumes 0, 1 min, units 0.10 M
- runs: 1
- milestone: M2
- started: 2026-10-28T06:26:51Z
- waiting_for: none
- result: Done, commit 0ddf4a8e. sys-objectives-ore-counter, sys-run, wbp-ore-counter updated; lint OK.

## Relations
- [[task-340-ore-run-carry]] — parent global task.
