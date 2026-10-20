---
id: task-257-pulse-desync
description: "Global task: Ore and heal crystals and pickups no longer pulse in sync: each object gets a seeded random pulse phase and a +-20 percent rate."
parent: tasks-to-do-index
kind: task
status: todo
---
# Pulse desync material inputs

- request: random pulse time and a seed on minerals and heals so crystals and heals do not all pulse together (owner, 2026-10-20)
- goal: Ore and heal crystals and pickups no longer pulse in sync: each object gets a seeded random pulse phase and a +-20 percent rate.
- acceptance: ore and heal pickups and deposit crystals pulse with different seeded phases and rates, XP unchanged; docs and lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-20).
- confirmed_by_owner: yes (2026-10-20)
- milestone: M1
- next_step: task-257-01-pulse-desync
- result: open

## Subtasks
1. task-257-01-pulse-desync — bp
2. task-257-02-pulse-desync — cpp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-257-01-pulse-desync]] — step 1, bp.
- [[task-257-02-pulse-desync]] — step 2, cpp.
