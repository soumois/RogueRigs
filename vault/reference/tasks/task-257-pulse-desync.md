---
id: task-257-pulse-desync
description: "Global task: Ore and heal crystals and pickups no longer pulse in sync: each object gets a seeded random pulse phase and a +-20 percent rate."
parent: tasks-done-index
kind: task
status: done
---
# Pulse desync material inputs

- request: random pulse time and a seed on minerals and heals so crystals and heals do not all pulse together (owner, 2026-10-20)
- goal: Ore and heal crystals and pickups no longer pulse in sync: each object gets a seeded random pulse phase and a +-20 percent rate.
- acceptance: ore and heal pickups and deposit crystals pulse with different seeded phases and rates, XP unchanged; docs and lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-20).
- confirmed_by_owner: yes (2026-10-20)
- milestone: M1
- next_step: none
- result: Done. Seeded pulse desync: FRRPulseDesync hash (PulseSeed 1, phase 0-1 turns, rate 0.8-1.2) written at pickup life start for Ore and Heal (CPD 1 and 2, reset to neutral at pool release, XP untouched) and for every deposit crystal instance (per-instance data 4 and 5, 6 floats, deposit id); build OK, 27 of 27 tests pass incl. HealVein and the new PulseDesync check; docs and GDD lines updated. Look and runtime read of the material unverified.

## Subtasks
1. task-257-01-pulse-desync — bp
2. task-257-02-pulse-desync — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-257-01-pulse-desync]] — step 1, bp.
- [[task-257-02-pulse-desync]] — step 2, cpp.
