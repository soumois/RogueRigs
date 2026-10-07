---
id: task-050-01-flaky-stacking-test
description: "DamageNumbers.StackOverDisplayed made reliable: find why it fails intermittently and fix the cause."
parent: task-050-flaky-stacking-test
kind: task
status: in-progress
---
# Flaky damage-number stacking test

- goal: DamageNumbers.StackOverDisplayed made reliable: find why it fails intermittently and fix the cause.
- agent: cpp
- depends_on: none
- inputs: sys-damage-numbers (vault/reference/tech/systems/sys-damage-numbers.md), sys-automation-tests (vault/reference/tech/systems/sys-automation-tests.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md)
- expected_output: RogueRigs.DamageNumbers.StackOverDisplayed (Source/RogueRigs/Tests/RRJuiceIntegrationTests.cpp:256, "numbers opened over displayed ones were stacked") fails intermittently (task-045, task-049): find the root cause (timing, random positions, frame delta, ordering, GC, shared state between tests) and fix it at the cause, in the test setup or in URRDamageNumberSubsystem if the stacking itself is wrong; do not loosen the check without a measured reason; prove stability by running that test alone 10 times in a row, then one full run; no new tests; docs sys-damage-numbers or sys-automation-tests if behaviour or the test changes.
- done_when: the test passes 10 runs in a row; build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 50000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-050-flaky-stacking-test]] — parent global task.
