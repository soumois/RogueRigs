---
id: task-250-fix-heal-vein-test
description: "Global task: RogueRigs.Ore.HealVein passes again: find why only 3 of 4 heal pickups are collected (health 55 instead of 70) and fix the cause, in the game if it breaks the heal ore rule, in the test if only its expectation is outdated."
parent: tasks-done-index
kind: task
status: done
---
# Fix RogueRigs.Ore.HealVein

- request: make a task for RogueRigs.Ore.HealVein, which fails (owner, 2026-10-20)
- goal: RogueRigs.Ore.HealVein passes again: find why only 3 of 4 heal pickups are collected (health 55 instead of 70) and fix the cause, in the game if it breaks the heal ore rule, in the test if only its expectation is outdated.
- acceptance: RogueRigs.Ore.HealVein passes in a headless run; the other RogueRigs.Ore and RogueRigs.Dig tests still pass; cause and fix written in the sys docs; lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-20).
- confirmed_by_owner: yes (2026-10-20)
- milestone: M1
- next_step: none
- result: Done. Cause: since task-235-01 each heal pickup pops from its own block (up to 2 cells from the miner) with the 80-300 uu pop, so a pickup can rest 303-394 uu away, beyond the 300 uu pickup radius; the test assumed a still miner collects all 4. The game follows heal-ore.md; fix in the test only (RROreDepositTests.cpp): uncollected pickups must exist, be at rest and lie beyond the radius, then the miner walks to each and collects it; final 4 collected and 70 HP unchanged. HealVein passes seeds 1-11; regression Ore, Dig, FineContactDigOpensPassage, RunFlow 26 of 26. Owner decision left: shorten PopMaxDistance on BP_PickupHeal if a whole vein must be collected without moving.

## Subtasks
1. task-250-01-fix-heal-vein-test — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-250-01-fix-heal-vein-test]] — step 1, cpp.
