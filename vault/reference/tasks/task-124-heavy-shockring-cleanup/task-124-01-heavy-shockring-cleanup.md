---
id: task-124-01-heavy-shockring-cleanup
description: "The hidden ShockRing component of BP_Heavy is deleted: its test lookups are removed in C++, then the component is removed from the Blueprint."
parent: task-124-heavy-shockring-cleanup
kind: task
status: in-progress
---
# Delete the heavy ShockRing component

- goal: The hidden ShockRing component of BP_Heavy is deleted: its test lookups are removed in C++, then the component is removed from the Blueprint.
- agent: cpp
- depends_on: none
- inputs: bp-heavy (vault/reference/tech/blueprints/enemies/bp-heavy.md), sys-enemies-heavy (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-heavy.md), sys-tests-enemies (vault/reference/tech/systems/tools/sys-automation-tests/sys-tests-enemies.md), sys-tests-heavy (vault/reference/tech/systems/tools/sys-automation-tests/sys-tests-heavy.md)
- expected_output: RRHeavyIntegrationTests.cpp no longer looks up or asserts on ShockRing (the ground circle checks stay); no other C++ reference to ShockRing remains.
- done_when: build exits 0 with the editor closed; RogueRigs.Integration.HeavyTelegraphSlamsMiner and RogueRigs.Heavy.* pass headless; grep finds no ShockRing in Source/; sys-tests-heavy updated; lint clean
- editor_state: editor closed (C++ build)
- token_ceiling: 60 turns, commit after each group
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-124-heavy-shockring-cleanup]] — parent global task.
