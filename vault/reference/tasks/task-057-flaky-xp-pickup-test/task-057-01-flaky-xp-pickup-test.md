---
id: task-057-01-flaky-xp-pickup-test
description: "Progression.XPPickupAddsOne made reliable: find why it fails intermittently and fix the cause."
parent: task-057-flaky-xp-pickup-test
kind: task
status: in-progress
---
# Flaky XP pickup test

- goal: Progression.XPPickupAddsOne made reliable: find why it fails intermittently and fix the cause.
- agent: cpp
- depends_on: none
- inputs: sys-progression (vault/reference/tech/systems/sys-progression.md), sys-pickups (vault/reference/tech/systems/sys-pickups.md), sys-automation-tests (vault/reference/tech/systems/sys-automation-tests.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md)
- expected_output: RogueRigs.Progression.XPPickupAddsOne (RRProgressionTests.cpp, "XP pickup collected once" expected 1, was 0; seen in tasks 030, 031, 054) fails intermittently: find the root cause (pickup motion timing, rest time, magnet, random pop, frame delta, ordering, GC, shared state) and fix it at the cause, in the test or in the game code if the game is wrong; do not loosen the check without a measured reason; prove stability by running that test alone 10 times in a row, then one full run; no new tests; docs sys-automation-tests (and sys-pickups if behaviour changes).
- done_when: the test passes 10 runs in a row; build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 50000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-057-flaky-xp-pickup-test]] — parent global task.
