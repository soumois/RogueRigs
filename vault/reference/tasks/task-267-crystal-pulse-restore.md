---
id: task-267-crystal-pulse-restore
description: "Global task: The deposit crystals pulse again, and the heal crystals get their hashed pulse back."
parent: tasks-done-index
kind: task
status: done
---
# Crystal pulse restore

- request: the crystals no longer pulse (reported 2026-10-20) (owner, 2026-10-21)
- goal: The deposit crystals pulse again, and the heal crystals get their hashed pulse back.
- acceptance: cause of the frozen crystal pulse identified, Duskite and heal deposit crystals pulse with hashed phase and rate again; docs and lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-21).
- confirmed_by_owner: yes (2026-10-21)
- milestone: M1
- next_step: none
- result: Done. Cause (engine source, 5.8): a scalar parameter with Use Custom Primitive Data compiles to the CPD slot and never uses its default; a primitive that writes nothing gets 36 zeroed floats, so PulseRate reads 0 and Time x 0 freezes the pulse: measured 0.0000 on the Duskite and heal deposit ISMs and on BP_PickupXP; ore pickups write 0.8972 and still pulse; task-262 values not the cause. XP nut frozen too (regression of 257-01). C++: deposit pulse data moved to per-instance indices 13 and 14 with 15 floats on every deposit ISM (heal rock keeps 0 at 4..12, shake input 0.0000 on 12 of 12; 371 instances hold their hash); cells keep 4 and 5; PulseDesync check updated; 26 of 26 other tests pass. Pending bp (267-02): M_DepositCrystalGlow reads 13 and 14 and drops its CPD parameters; M_XPNutGlow: If(PulseRate > 0, PulseRate, 1). Not seen on screen.

## Subtasks
1. task-267-01-crystal-pulse-restore — cpp
2. task-267-02-crystal-pulse-restore — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-267-01-crystal-pulse-restore]] — step 1, cpp.
- [[task-267-02-crystal-pulse-restore]] — step 2, bp.
