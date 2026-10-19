---
id: task-235-ore-loot-over-dig
description: "Global task: deposits release their loot one by one, evenly spread over the dig time, not all at destruction."
parent: tasks-done-index
kind: task
status: done
---
# Loot spread over the dig

- request: "on ne recoit les ressources que lorsque le bloc est totalement détruit, il faut étaler sur le temps de minage. Ex : Si sur la vie, il faut 2 sec de minage et qu'il y a 4 loot de vie sur le minage, il y a un lot qui arrive toutes les 0.5s" (owner, 2026-10-19)
- goal: Ore and heal deposits release their loot one by one during the dig, evenly spread over the dig time, instead of all at destruction.
- acceptance: build exits 0 editor closed; RogueRigs.* tests pass except the 2 known pre-existing failures; a -game run logs the pickup times during a deposit dig; docs updated; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-19).
- confirmed_by_owner: yes (2026-10-19)
- milestone: M2
- next_step: none
- result: Done. Commits 336ed33c (code, tests), 3479ac88 (docs). Deposit pickup k pops at dig progress k/N (N = blocks x DestroyDropCount = 4), from its own block; explosion pops the rest at once; progress kept. -game: heal vein pops at 0.38 s intervals, Duskite at 0.375 s (1.5 s dig, mining speed 1). Build 0, tests 262/264 (2 known; Ore.HealVein timing checks pass, still 3 of 4 collected as before).

## Subtasks
1. task-235-01-ore-loot-over-dig — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-235-01-ore-loot-over-dig]] — step 1, cpp.
