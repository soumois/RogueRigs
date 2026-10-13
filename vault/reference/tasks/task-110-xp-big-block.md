---
id: task-110-xp-big-block
description: "Global task: When many XP cubes fall in one place they become one bigger block of another colour worth the same total XP."
parent: tasks-done-index
kind: task
status: done
---
# Big XP block (C++ rule)

- request: "rapport de test du 2026-10-12" (owner); decisions confirmed 2026-10-12 (rules of the zones unchanged: only the rendering changes) (owner, 2026-10-13)
- goal: When many XP cubes fall in one place they become one bigger block of another colour worth the same total XP.
- acceptance: build exits 0 with the editor closed; all RogueRigs.* tests pass headless; docs updated; lint clean; committed; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-13).
- confirmed_by_owner: yes (2026-10-13)
- milestone: M2
- next_step: none
- result: Done. BP_PickupXPBig (duplicate of BP_PickupXP, root scale 0.54, MI_PickupXPBig on M_GreyBoxGlow: white, GlowColor 0.75/1/1, GlowAmount 4) and XPBigPickupClass set on BP_PickupXP (feature on); trail inherits the scale; headless rerun to confirm; not seen. Commits 6e1eff5, 7247abb.

## Subtasks
1. task-110-01-xp-big-block — cpp
2. task-110-02-xp-big-block — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-110-01-xp-big-block]] — step 1, cpp.
- [[task-110-02-xp-big-block]] — step 2, bp.
