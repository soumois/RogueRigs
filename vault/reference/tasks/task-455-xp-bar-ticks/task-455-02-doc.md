---
id: task-455-02-doc
description: "Doc: Docs describe the ticks."
parent: task-455-xp-bar-ticks
kind: task
status: done
---
# XP bar split in 5 by light ticks: doc

- goal: Docs describe the ticks.
- agent: doc
- depends_on: task-455-01-ticks
- inputs: wbp-xp-status (vault/reference/tech/blueprints/ui/wbp-xp-status.md)
- expected_output: see done_when
- done_when: Anchors listed per umg-layout; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-455
- usage: included in the executor run of task-455
- runs: 1
- milestone: M2
- started: 2026-11-06T00:59:49Z
- waiting_for: none
- result: Commit 6ee90430: wbp-xp-status updated (tree XPBarArea > XPBar + XPTick20/40/60/80, ticks decision Q31, anchors table per umg-layout for each widget, verified line, umg-layout relation). Lint clean.

## Relations
- [[task-455-xp-bar-ticks]] — parent global task.
