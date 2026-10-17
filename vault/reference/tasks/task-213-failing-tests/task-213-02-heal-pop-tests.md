---
id: task-213-02-heal-pop-tests
description: "cpp: Fix RogueRigs.Integration.JuiceHealRing"
parent: task-213-failing-tests
kind: task
status: done
---
# heal-pop-tests

- goal: Fix RogueRigs.Integration.JuiceHealRing ("heal chunk lands within one 67 uu cell (PopMaxDistance)", RRJuiceIntegrationTests.cpp:601) and make RogueRigs.Ore.HealVein deterministic (seeded random pop in the test); both share the heal pickup pop.
- agent: cpp
- depends_on: task-213-01-death-parts-test
- inputs: sys-tests-juice-damage-numbers (vault/reference/tech/systems/tools/sys-automation-tests/sys-tests-juice-damage-numbers.md), sys-tests-dig-ore (vault/reference/tech/systems/tools/sys-automation-tests/sys-tests-dig-ore.md), sys-pickups (vault/reference/tech/systems/world/sys-pickups.md), sys-pickups-lifecycle (vault/reference/tech/systems/world/sys-pickups/sys-pickups-lifecycle.md), heal-ore (vault/reference/gdd/terrain/heal-ore.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md), agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: C++ test fixes (or code if the code is wrong), sys-tests docs updated with the cause.
- done_when: JuiceHealRing passes; HealVein passes 10 runs out of 10; the full suite passes; docs updated; committed.
- editor_state: closed
- token_ceiling: 50000
- model: opus
- tokens: input 52, cache write 55758, cache read 1046749, output 2757
- runs: 1
- milestone: M2
- result: JuiceHealRing pop bound read from the BP_PickupHeal class default (80-300 uu, task-195-02 owner decision); flakiness: random pop beyond the 300 uu pickup radius, HealVein, JuiceHealRing and JuiceKillDropsStreamingCubes seeded with FMath::RandInit(5) in the tests; gameplay unchanged (07dd7817, 9f015d76, 767bccf9). HealVein 10/10 alone; full RogueRigs suite exit 0, 262 success.

## Relations
- [[task-213-failing-tests]] — parent global task.
