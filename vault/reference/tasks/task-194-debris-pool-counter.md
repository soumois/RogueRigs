---
id: task-194-debris-pool-counter
description: "Global task: find why debris pool plays exceed the cap of 80 since the GPU emitter, and fix it."
parent: tasks-done-index
kind: task
status: done
---
# Robot debris: pool plays above the cap

- request: A puis C: comprendre pourquoi des débris reviennent tout de suite dans le pool et corriger le compteur ou le recyclage (owner, 2026-10-15) (owner, 2026-10-15)
- goal: Find why debris pool plays exceed the cap of 80 (S7: 200 plays, 0 skips) since the GPU emitter, and fix the counter or the recycling.
- acceptance: Cause proven by run values; after the fix an S7 burst 200 run shows debris plays <= 80 and skips for the rest, and a game-mode run still shows debris (plays > 0); build passes with the editor closed; Stress tests pass; committed by path; committed.
- gdd_check: enemy-death read (2026-10-15): no conflict (measure and pool fix only).
- confirmed_by_owner: yes (2026-10-15)
- milestone: M3
- next_step: none
- result: Done. Cause: at first use in editor -game the GPU NS_RobotDebris compiles and activated components wait (inactive, tick on); the pool reclaimed them as idle (S7: 80 effects, 200 counted plays, earlier debris overwritten). Fix in URRNiagaraPoolSubsystem: waiting components not reclaimed (IsWaitingToStart), play counted only if active after Activate, editor-only background compile at prewarm; S7 200 now 80 plays / 120 skips (both bursts), game 200 219 plays; build OK, tests 29/30 (known EyeMesh); cooked build and on-screen look unverified; commits e62af85f, 978026c7

## Subtasks
1. task-194-01-debris-pool-counter — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-194-01-debris-pool-counter]] — step 1, cpp.
