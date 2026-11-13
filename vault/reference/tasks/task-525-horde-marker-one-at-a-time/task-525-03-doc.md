---
id: task-525-03-doc
description: "Doc: alert bar doc: single horde marker"
parent: task-525-horde-marker-one-at-a-time
kind: task
status: done
---
# Un seul point d'exclamation de horde à la fois: doc

- goal: alert bar doc: single horde marker
- agent: doc
- depends_on: task-525-02-capture
- inputs: level-flow (vault/reference/gdd/run/level-flow.md)
- expected_output: see done_when
- done_when: A5 doc part holds; lint clean; commit.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-525
- usage: included in the executor run of task-525
- runs: 1
- milestone: M2
- started: 2026-11-13T14:09:36Z
- waiting_for: none
- result: commit 3131460f: sys-hud-pressure gains the horde list rule (one horde, the first not Done; gone once Done, next listed; none after the last; no WBP change, ShowAlertIcons places hordes by Index) and links wbp-alert-objectives and level-flow. level-flow M1 already holds the owner rule (written at planning, unchanged). Lint clean.

## Relations
- [[task-525-horde-marker-one-at-a-time]] — parent global task.
