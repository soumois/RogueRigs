---
id: task-235-ore-loot-over-dig
description: "Global task: deposits release their loot one by one, evenly spread over the dig time, not all at destruction."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Loot spread over the dig

- request: "on ne recoit les ressources que lorsque le bloc est totalement détruit, il faut étaler sur le temps de minage. Ex : Si sur la vie, il faut 2 sec de minage et qu'il y a 4 loot de vie sur le minage, il y a un lot qui arrive toutes les 0.5s" (owner, 2026-10-19)
- goal: Ore and heal deposits release their loot one by one during the dig, evenly spread over the dig time, instead of all at destruction.
- acceptance: build exits 0 editor closed; RogueRigs.* tests pass except the 2 known pre-existing failures; a -game run logs the pickup times during a deposit dig; docs updated; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-19).
- confirmed_by_owner: yes (2026-10-19)
- milestone: M2
- next_step: task-235-01-ore-loot-over-dig
- result: open

## Subtasks
1. task-235-01-ore-loot-over-dig — cpp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-235-01-ore-loot-over-dig]] — step 1, cpp.
