---
id: task-050-flaky-stacking-test
description: "Global task: DamageNumbers.StackOverDisplayed made reliable: find why it fails intermittently and fix the cause."
parent: tasks-index
kind: task
status: in-progress
---
# Flaky damage-number stacking test

- request: "Résous le bug concernant le test qui rate régulièrement l’empilement des chiffres (crée une tâche)" (owner, 2026-10-07)
- goal: DamageNumbers.StackOverDisplayed made reliable: find why it fails intermittently and fix the cause.
- acceptance: the test passes 10 runs in a row; build exits 0 with the editor closed; all `RogueRigs*` tests pass headless; docs updated; lint clean.; committed.
- gdd_check: GDD updated by the Orchestrator (owner 2026-10-07, fifth test). No open value.
- confirmed_by_owner: yes (2026-10-07)
- milestone: M2
- next_step: task-050-01-flaky-stacking-test
- result: open

## Subtasks
1. task-050-01-flaky-stacking-test — cpp

## Relations
- [[tasks-index]] — parent index.
- [[task-050-01-flaky-stacking-test]] — step 1, cpp.
