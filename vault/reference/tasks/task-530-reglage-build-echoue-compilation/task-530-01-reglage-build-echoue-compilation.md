---
id: task-530-01-reglage-build-echoue-compilation
description: "Executor: Apply the setting change of the request and check it at light size."
parent: task-530-reglage-build-echoue-compilation
kind: task
status: done
---
# Réglage: Réglage : le build échoue à la compilation (UE 5.8). RRWeaponTests.cpp:1687-1689 et RRLevelUpTests.cpp:412-414 appellent FMaterialResource::FinishCompilation() et ...: executor

- goal: Apply the setting change of the request and check it at light size.
- agent: executor
- depends_on: none
- inputs: request-families (vault/reference/agents/request-families.md), verification-levels (vault/reference/agents/verification-levels.md)
- expected_output: see done_when
- done_when: A1 to A3 of the global pass.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-530
- usage: included in the executor run of task-530
- runs: 1
- milestone: M2
- started: 2026-11-13T17:45:02Z
- waiting_for: none
- result: Done (commit e1445b2c). Cause: in UE 5.8 FMaterial::FinishCompilation and GetCompileErrors are WITH_EDITOR only, so the game target of BuildCookRun failed (C2039). RRWeaponTests.cpp (M_MuzzleFlash) and RRLevelUpTests.cpp (M_LevelUpWave): editor keeps FinishCompilation + 0 compile errors under #if WITH_EDITOR; game target checks HasValidGameThreadShaderMap. Editor build exit 0. Tests RogueRigs.Weapons + RogueRigs.LevelUp (-RenderOffScreen and -nullrhi): 24 of 25 pass incl. MuzzleFlashAsset and WaveAsset; Weapons.ImpactSparkAsset fails at RRWeaponTests.cpp l.1620-1636 (pool prewarm and spark request = 0), code not touched here, fails under -nullrhi too: existing defect, not fixed (outside the request). Staged BuildCookRun exit 0 (BUILD SUCCESSFUL); copied per owner to Saved/StagedBuilds/RogueRigsv0.4.0 (original Saved/StagedBuilds/Windows kept, git-ignored, not committed).

## Relations
- [[task-530-reglage-build-echoue-compilation]] — parent global task.
