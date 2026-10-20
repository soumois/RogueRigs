---
id: task-250-01-fix-heal-vein-test
description: "RogueRigs.Ore.HealVein passes again: find why only 3 of 4 heal pickups are collected (health 55 instead of 70) and fix the cause, in the game if it breaks the heal ore rule, in the test if only its expectation is outdated."
parent: task-250-fix-heal-vein-test
kind: task
status: done
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
- model: opus
- tokens: input 56, cache write 101386, cache read 2005884, output 12828
- runs: 1
- milestone: M1
- result: Cause: since task-235-01 each heal pickup pops from its own block (up to 2 cells from the miner) with the 80-300 uu pop, so a pickup can rest 303-394 uu away, beyond the 300 uu pickup radius; the test assumed a still miner collects all 4. The game follows heal-ore.md; fix in the test only (RROreDepositTests.cpp): uncollected pickups must exist, be at rest and lie beyond the radius, then the miner walks to each and collects it; final 4 collected and 70 HP unchanged. HealVein passes seeds 1-11; regression Ore, Dig, FineContactDigOpensPassage, RunFlow 26 of 26. Owner decision left: shorten PopMaxDistance on BP_PickupHeal if a whole vein must be collected without moving.

## Relations
- [[task-250-fix-heal-vein-test]] — parent global task.
