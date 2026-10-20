---
id: task-250-fix-heal-vein-test
description: "Global task: RogueRigs.Ore.HealVein passes again: find why only 3 of 4 heal pickups are collected (health 55 instead of 70) and fix the cause, in the game if it breaks the heal ore rule, in the test if only its expectation is outdated."
parent: tasks-to-do-index
kind: task
status: todo
---
# Fix RogueRigs.Ore.HealVein

- request: make a task for RogueRigs.Ore.HealVein, which fails (owner, 2026-10-20)
- goal: RogueRigs.Ore.HealVein passes again: find why only 3 of 4 heal pickups are collected (health 55 instead of 70) and fix the cause, in the game if it breaks the heal ore rule, in the test if only its expectation is outdated.
- acceptance: RogueRigs.Ore.HealVein passes in a headless run; the other RogueRigs.Ore and RogueRigs.Dig tests still pass; cause and fix written in the sys docs; lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-20).
- confirmed_by_owner: yes (2026-10-20)
- milestone: M1
- next_step: task-250-01-fix-heal-vein-test
- result: open

## Subtasks
1. task-250-01-fix-heal-vein-test — cpp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-250-01-fix-heal-vein-test]] — step 1, cpp.
