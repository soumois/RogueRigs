---
id: task-042-01-hack-time
description: "Console hack time 20 → 16 s."
parent: task-042-hack-time
kind: task
status: in-progress
---
# Hack time −20 %

- goal: Console hack time 20 → 16 s.
- agent: cpp
- depends_on: none
- inputs: objective-console (vault/reference/gdd/run/objective-console.md), sys-objectives (vault/reference/tech/systems/sys-objectives.md), sys-automation-tests (vault/reference/tech/systems/sys-automation-tests.md)
- expected_output: console hack duration 20 → 16 s (C++ default or data, check BP_ObjectiveConsole does not override it on disk); update only existing tests that break; docs sys-objectives.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 30000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-042-hack-time]] — parent global task.
