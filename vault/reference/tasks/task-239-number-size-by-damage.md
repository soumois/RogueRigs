---
id: task-239-number-size-by-damage
description: "Global task: The size of a damage number grows with the damage of the hit, continuous from x0.8 to x1.4; a crit keeps its x1.6 on top."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Damage number size by damage

- request: Damage numbers video: size = damage (point 6), defaults accepted: x0.8 to x1.4, maximum reached at twice the base hit of the starting weapon (owner, 2026-10-19)
- goal: The size of a damage number grows with the damage of the hit, continuous from x0.8 to x1.4; a crit keeps its x1.6 on top.
- acceptance: A 1-damage hit and a hit at DamageForMaxSize give sizes x0.8 and x1.4 of NumberSize in the digit array; crit multiplies by 1.6; build passes; lint clean. Stacking footprint uses the new size; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-19).
- confirmed_by_owner: yes (2026-10-19)
- milestone: M2
- next_step: task-239-01-number-size-by-damage
- result: open

## Subtasks
1. task-239-01-number-size-by-damage — cpp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-239-01-number-size-by-damage]] — step 1, cpp.
