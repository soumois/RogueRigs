---
id: task-531-01-fix-impact-spark-test
description: "Cpp: find whether test ImpactSparkAsset or the impact effect pool code is wrong, fix it at its source, pass in -nullrhi and -RenderOffScreen."
parent: task-531-impact-spark-test-fails
kind: task
status: done
---
# Test RogueRigs.Weapons.ImpactSparkAsset en échec: cpp

- goal: Find whether RogueRigs.Weapons.ImpactSparkAsset or the impact effect pool / Niagara code is wrong, fix it at its source, make the test pass in -nullrhi and -RenderOffScreen.
- agent: cpp
- depends_on: none
- inputs: burst-rifle (vault/reference/gdd/weapons/burst-rifle.md), sys-weapons (vault/reference/tech/systems/combat/sys-weapons.md), task-465-shot-impact-burst (vault/reference/tasks/task-465-shot-impact-burst.md), Source/RogueRigs/Tests/RRWeaponTests.cpp
- expected_output: see done_when
- done_when: A1 to A6 of the global hold: the test passes in both modes with the three named asserts; the cause and its evidence are in the subtask report; no weakened assert; other RogueRigs.Weapons tests show no new failure; game and editor builds compile; no change to what the player sees.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-531
- usage: included in the executor run of task-531
- runs: 1
- milestone: M2
- started: 2026-11-13T18:03:48Z
- waiting_for: none
- result: Done (716bdb8e). Cause in the test, not in the pool or Niagara code: since task-465 BP rifle shot (bp-projectile) overrides ImpactSparkSystem with NS_ShotImpactBurstGreen, while the test still loaded NS_ShotImpactSpark (the C++ default of ARRProjectile). Evidence: the -nullrhi log before the fix also failed 'rifle shot plays it' (rifle projectile CDO system != NS_ShotImpactSpark); prewarm, request and play were counted on the burst pool, so the spark pool read 0. Fix: test reads the rifle's shipped NS_ShotImpactBurstGreen for every pool assert, and keeps a check that NS_ShotImpactSpark is still the C++ default (native turret bullet); no assert removed, no skip added. Checks: ImpactSparkAsset Success under -nullrhi and -RenderOffScreen (played, back in the pool after 14 frames); RogueRigs.Weapons 16/16 Success under -nullrhi; RogueRigsEditor and RogueRigs game builds succeed. No runtime code changed, so nothing the player sees changes.

## Relations
- [[task-531-impact-spark-test-fails]] — parent global task.
