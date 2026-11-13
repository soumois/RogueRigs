---
id: task-530-reglage-build-echoue-compilation
description: "Global task: Réglage (no planner): Réglage : le build échoue à la compilation (UE 5.8). RRWeaponTests.cpp:1687-1689 et RRLevelUpTests.cpp:412-414 appellent FMaterialResource::FinishCompilation() et ..."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Réglage: Réglage : le build échoue à la compilation (UE 5.8). RRWeaponTests.cpp:1687-1689 et RRLevelUpTests.cpp:412-414 appellent FMaterialResource::FinishCompilation() et ...

- request: Owner 2026-11-13, verbatim: "Réglage : le build échoue à la compilation (UE 5.8). RRWeaponTests.cpp:1687-1689 et RRLevelUpTests.cpp:412-414 appellent FMaterialResource::FinishCompilation() et GetCompileErrors(), qui n'existent plus. Réécrire ces deux vérifications de compilation des matériaux (M_MuzzleFlash, M_LevelUpWave) avec l'API de 5.8, puis relancer le build staged (BuildCookRun) jusqu'à exit 0.". Family réglage, picked or forced at the entry; node written by task-reglage.js before the executor run, no planner.
- goal: The setting the owner names is changed on the existing system, nothing else.
- acceptance: A1 the change named in the request is in place, checked at light size (verification-levels); A2 no new asset, no new system, no gdd value decided by the executor; A3 lint clean, committed.
- gdd_check: Not done: réglage family, no planner (request-families). The executor stops before editing on a new asset or system, an open value or a design question, and reports for a relaunch through the planner (agent-executor wrong_family).
- verification_level: light
- family: reglage
- confirmed_by_owner: yes (2026-11-13, the request itself, réglage family)
- milestone: M2
- next_step: task-530-01-reglage-build-echoue-compilation
- started: 2026-11-13T17:45:02Z
- waiting_for: none
- result: open

## Subtasks
1. task-530-01-reglage-build-echoue-compilation (vault/reference/tasks/task-530-reglage-build-echoue-compilation/task-530-01-reglage-build-echoue-compilation.md) — executor: Apply the setting change of the request and check it at light size.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-530-01-reglage-build-echoue-compilation]] — step 1, executor.
- [[request-families]] — the family path this task follows.
