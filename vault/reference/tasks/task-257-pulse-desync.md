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
- result: Done. M_XPNutGlow gets PulsePhase (turns, added inside the sine) and PulseRate (scales PulseFrequency): pickups via Custom Primitive Data index 1 (phase, neutral 0) and 2 (rate, neutral 1), deposit crystal instances via per-instance data index 4 (phase) and 5 (rate), NumCustomDataFloats must be at least 6; neutral inputs give the old expression exactly; one material, no switch; compiles. Look and runtime read unverified.

## Subtasks
1. task-257-01-pulse-desync — bp
2. task-257-02-pulse-desync — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-257-01-pulse-desync]] — step 1, bp.
- [[task-257-02-pulse-desync]] — step 2, cpp.
