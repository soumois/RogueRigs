---
id: task-530-reglage-build-echoue-compilation
description: "Global task: Réglage (no planner): Réglage : le build échoue à la compilation (UE 5.8). RRWeaponTests.cpp:1687-1689 et RRLevelUpTests.cpp:412-414 appellent FMaterialResource::FinishCompilation() et ..."
parent: tasks-done-index
kind: task
status: done
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
- next_step: none
- started: 2026-11-13T17:45:02Z
- waiting_for: none
- executor_tokens: input 46, cache write 55512, cache read 834651, output 8234
- executor_usage: runs 1, turns 21, context start 17k peak 56k, resumes 0, 8 min, units 0.19 M, USD 0.61
- planning_tokens: none (no planner run attributed)
- planning_usage: none
- result: Staged build fixed: editor-only material compile checks guarded with WITH_EDITOR (UE 5.8), game target checks HasValidGameThreadShaderMap; editor build and BuildCookRun exit 0; build copied to Saved/StagedBuilds/RogueRigsv0.4.0 (not committed). Weapons+LevelUp 24/25: existing failure RogueRigs.Weapons.ImpactSparkAsset (pool prewarm/request), not touched, reported.

## Subtasks
1. task-530-01-reglage-build-echoue-compilation (vault/reference/tasks/task-530-reglage-build-echoue-compilation/task-530-01-reglage-build-echoue-compilation.md) — executor: Apply the setting change of the request and check it at light size.

## Relations
- [[tasks-done-index]] — parent index.
- [[task-530-01-reglage-build-echoue-compilation]] — step 1, executor.
- [[request-families]] — the family path this task follows.
