---
id: task-050-01-flaky-stacking-test
description: "DamageNumbers.StackOverDisplayed made reliable: find why it fails intermittently and fix the cause."
parent: task-050-flaky-stacking-test
kind: task
status: done
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
- model: opus
- tokens: input 54, cache write 53588, cache read 986682, output 13926
- runs: 1
- milestone: M2
- result: Cause in the test, not the game: random ±25 uu side offsets (clock-seeded) sometimes put robot B's number more than 55.8 uu from A's column (2-digit plus 1-digit half widths), so it rightly stayed unstacked (about 1.4 % of runs, confirmed by logs and a simulation). Fix: B hit 5 uu from A instead of 20 (0 failures over 500k draws); checks unchanged. 10/10 runs alone, full run 195/195. Commit 719af2d.

## Relations
- [[task-050-flaky-stacking-test]] — parent global task.
