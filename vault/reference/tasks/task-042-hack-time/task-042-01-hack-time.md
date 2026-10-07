---
id: task-042-01-hack-time
description: "Console hack time 20 → 16 s."
parent: task-042-hack-time
kind: task
status: done
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
- model: opus
- tokens: input 26, cache write 22468, cache read 239041, output 5399
- runs: 1
- milestone: M2
- result: ARRConsole HackDuration 20 → 16 s (C++ only, BP_ObjectiveConsole does not override); Console.HackRules retimed (528 frames, measured 529). Build 0, 195/195. Commit c16814d.

## Relations
- [[task-042-hack-time]] — parent global task.
