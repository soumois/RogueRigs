---
id: task-019-packaging-fix
description: "Global task: add GameFeatureData to the asset manager settings so BuildCookRun cook, stage and pak exits 0."
parent: tasks-index
kind: task
status: done
---
# Packaging fix

- request: "Tu peux lancer 1 et 2. Aussi, peut être faire un check up ( et refactoring si nécessaire) du code c++ pour voir si le code est assez généralisé et lisible pour un être humain" (1 = packaging fix, 2 = shooter); values and plan confirmed "Oui" (2026-10-04).
- goal: a full Win64 Development BuildCookRun exits 0 and the staged game runs.
- acceptance: (1) BuildCookRun build+cook+stage+pak exits 0; (2) the staged exe starts L_ArenaM1 without error; (3) build-toolchain documents the command; committed.
- gdd_check: No GDD impact. Error found in task-018-04 (UAT exit 25: GameFeatureData missing from the asset manager primary asset types).
- confirmed_by_owner: yes (2026-10-04, all subtasks pre-approved)
- milestone: M1
- next_step: none
- result: All acceptance checks passed: BuildCookRun exit 0, staged run clean, documented.

## Subtasks
1. task-019-01-asset-manager — cpp: asset manager config, full BuildCookRun, staged run check

## Relations
- [[tasks-index]] — parent index.
- [[task-019-01-asset-manager]] — step 1, cpp.
