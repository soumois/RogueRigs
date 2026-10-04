---
id: task-020-code-review
description: "Global task: review the C++ for human readability and generalization, refactor high-value items without behavior change, check Blueprints in PIE."
parent: tasks-index
kind: task
status: in-progress
---
# C++ code review and refactor

- request: "Tu peux lancer 1 et 2. Aussi, peut être faire un check up ( et refactoring si nécessaire) du code c++ pour voir si le code est assez généralisé et lisible pour un être humain" (1 = packaging fix, 2 = shooter); values and plan confirmed "Oui" (2026-10-04).
- goal: the C++ reads well for a human and shares the right abstractions (robots, weapons, pickups, objectives), with no behavior change.
- acceptance: (1) a review node ranks findings by gain and risk; (2) the chosen refactors are done with all tests green before and after; (3) no renamed reflected class or property without a CoreRedirect; (4) all Blueprints load and PIE plays with no error; (5) docs match; lint clean; committed.
- gdd_check: No GDD impact. Rule [[cpp-standard]] and [[cpp-bp-split]] apply.
- confirmed_by_owner: yes (2026-10-04, all subtasks pre-approved)
- milestone: M1
- next_step: task-020-03-refactor-cleanup
- result: open

## Subtasks
1. task-020-01-review — cpp: review report, ranked refactor list
2. task-020-02-refactor — cpp: R1, R2, R4, R5 (robot helpers, cell query, world lookups, projectile filter, weapon aim)
3. task-020-03-refactor-cleanup — cpp: R3, R6, R7 (CSV loader, damage numbers, cheats and test hooks)
4. task-020-04-bp-check — bp: all Blueprints load, PIE smoke test

## Relations
- [[tasks-index]] — parent index.
- [[task-020-01-review]] — step 1, cpp.
- [[task-020-02-refactor]] — step 2, cpp.
- [[task-020-03-refactor-cleanup]] — step 3, cpp.
- [[task-020-04-bp-check]] — step 4, bp.
