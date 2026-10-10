---
id: task-042-hack-time
description: "Global task: Console hack time 20 → 16 s."
parent: tasks-done-index
kind: task
status: done
---
# Hack time −20 %

- request: "Réduire de 20% le temps de piratage des commandes." (owner, 2026-10-07)
- goal: Console hack time 20 → 16 s.
- acceptance: build exits 0 with the editor closed; all `RogueRigs*` tests pass headless; docs updated; lint clean.; committed.
- gdd_check: GDD updated by the Orchestrator (owner 2026-10-07, fifth test). No open value.
- confirmed_by_owner: yes (2026-10-07)
- milestone: M2
- next_step: none
- result: Done. ARRConsole HackDuration 20 → 16 s (C++ only, BP_ObjectiveConsole does not override); Console.HackRules retimed (528 frames, measured 529). Build 0, 195/195. Commit c16814d.

## Subtasks
1. task-042-01-hack-time — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-042-01-hack-time]] — step 1, cpp.
