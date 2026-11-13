---
id: task-531-01-fix-impact-spark-test
description: "Cpp: find whether test ImpactSparkAsset or the impact effect pool code is wrong, fix it at its source, pass in -nullrhi and -RenderOffScreen."
parent: task-531-impact-spark-test-fails
kind: task
status: todo
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
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-531-impact-spark-test-fails]] — parent global task.
