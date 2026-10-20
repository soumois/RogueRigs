---
id: task-250-01-fix-heal-vein-test
description: "RogueRigs.Ore.HealVein passes again: find why only 3 of 4 heal pickups are collected (health 55 instead of 70) and fix the cause, in the game if it breaks the heal ore rule, in the test if only its expectation is outdated."
parent: task-250-fix-heal-vein-test
kind: task
status: todo
---
# Fix RogueRigs.Ore.HealVein

- goal: RogueRigs.Ore.HealVein passes again: find why only 3 of 4 heal pickups are collected (health 55 instead of 70) and fix the cause, in the game if it breaks the heal ore rule, in the test if only its expectation is outdated.
- agent: cpp
- depends_on: none
- inputs: heal-ore (vault/reference/gdd/terrain/heal-ore.md), ore-blocks (vault/reference/gdd/terrain/ore-blocks.md), sys-grid-deposits (vault/reference/tech/systems/world/sys-grid/sys-grid-deposits.md), sys-pickups (vault/reference/tech/systems/world/sys-pickups.md), sys-pickups-lifecycle (vault/reference/tech/systems/world/sys-pickups/sys-pickups-lifecycle.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md), sys-tests-dig-ore (vault/reference/tech/systems/tools/sys-automation-tests/sys-tests-dig-ore.md)
- expected_output: Diagnosis recorded in the sys docs, then the fix: game code in Source/RogueRigs if a heal pickup is really lost (GDD: every heal pickup of a vein must be collectable, even when it pops inside the pickup radius), or the expectation in Source/RogueRigs/Tests/RROreDepositTests.cpp if it is outdated (loot over the dig since task-235); the cause is checked with and without the drone changes of tasks 247-249 (known to fail before them).
- done_when: RogueRigs.Ore.HealVein passes in a headless run; the other RogueRigs.Ore and RogueRigs.Dig tests still pass; cause and fix written in the sys docs; lint clean.
- editor_state: closed (C++ build)
- token_ceiling: 150k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-250-fix-heal-vein-test]] — parent global task.
