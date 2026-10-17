---
id: task-213-failing-tests
description: "Global task: Failing and flaky automation tests."
parent: tasks-done-index
kind: task
status: done
---
# Failing and flaky automation tests

- request: "fait le 1, puis le 4, puis 7,8,9." (point 7: ShotHitsMovingRobotAndDeathShowsParts, JuiceHealRing, flaky HealVein), then "le reste est ok" (owner, 2026-10-17)
- goal: the two failing tests and the flaky HealVein test pass reliably, fixing the test when it contradicts an owner decision and the code otherwise.
- acceptance: RogueRigs.Enemies.ShotHitsMovingRobotAndDeathShowsParts, RogueRigs.Integration.JuiceHealRing and RogueRigs.Ore.HealVein pass, HealVein 10 runs out of 10; the full RogueRigs suite passes; docs updated; lint clean; committed.
- gdd_check: Owner 2026-10-17 defaults accepted: when a test contradicts a later owner decision (e.g. heal pop distance 80 to 300 from task-195-02) the test is updated, otherwise the code is fixed; HealVein becomes deterministic by seeding the random stream in the test, gameplay unchanged.
- confirmed_by_owner: yes (2026-10-17)
- milestone: M2
- next_step: none
- result: Done. JuiceHealRing pop bound read from the BP_PickupHeal class default (80-300 uu, task-195-02 owner decision); flakiness: random pop beyond the 300 uu pickup radius, HealVein, JuiceHealRing and JuiceKillDropsStreamingCubes seeded with FMath::RandInit(5) in the tests; gameplay unchanged (07dd7817, 9f015d76, 767bccf9). HealVein 10/10 alone; full RogueRigs suite exit 0, 262 success.

## Subtasks
1. task-213-01-death-parts-test — cpp
2. task-213-02-heal-pop-tests — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-213-01-death-parts-test]] — step 1, cpp.
- [[task-213-02-heal-pop-tests]] — step 2, cpp.
