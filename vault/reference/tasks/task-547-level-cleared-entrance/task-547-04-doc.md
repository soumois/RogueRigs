---
id: task-547-04-doc
description: "Doc: Document the shared entrance effect"
parent: task-547-level-cleared-entrance
kind: task
status: done
---
# Level cleared screen appears with the death screen entrance effect: doc

- goal: Document the shared entrance effect
- agent: doc
- depends_on: task-547-03-capture
- inputs: death-screen (vault/reference/gdd/run/death-screen.md), intermission (vault/reference/gdd/run/intermission.md)
- expected_output: see done_when
- done_when: sys node of the shared entrance and both screen bp nodes updated; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-547
- usage: included in the executor run of task-547
- runs: 1
- milestone: M2
- started: 2026-11-14T04:18:04Z
- waiting_for: none
- result: New sys-screen-entrance (systems-ui-index); updated sys-run-lost, sys-run-transition, wbp-run-lost (entrance graph removed, leftovers Elapsed/Animating noted), wbp-intermission (backdrop 0.75, entrance parts). Lint clean. Intermission gdd already carries the 2026-11-13 appearance line (edited at confirmation).

## Relations
- [[task-547-level-cleared-entrance]] — parent global task.
