---
id: task-195-01-floaty-pop-midair-magnet
description: "C++: pop fall-to-rise time ratio and optional mid-air magnet after a delay from spawn on ARRPickup, defaults unchanged."
parent: task-195-floaty-pop-midair-magnet
kind: task
status: done
---
# Floaty pop and mid-air magnet

- goal: Pickup pop: long floaty arc (slower fall than rise) and magnet allowed mid-air after a delay from spawn, for XP nut, ore and heal pickups, not the chip.
- agent: cpp
- depends_on: none
- inputs: sys-pickups (vault/reference/tech/systems/world/sys-pickups.md), sys-pickups-lifecycle (vault/reference/tech/systems/world/sys-pickups/sys-pickups-lifecycle.md), sys-pickups-xp (vault/reference/tech/systems/world/sys-pickups/sys-pickups-xp.md), sys-actor-pool (vault/reference/tech/systems/world/sys-actor-pool.md), xp-level-up (vault/reference/gdd/run/xp-level-up.md), sys-tests-pools-pickups (vault/reference/tech/systems/tools/sys-automation-tests/sys-tests-pools-pickups.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md), agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: C++ in Source/RogueRigs/Pickups/: on ARRPickup (EditDefaultsOnly, BlueprintReadOnly) a fall-to-rise time ratio for the pop arc (1 = today) and an option letting the magnet latch during the pop once a delay counted from spawn has passed (off by default = today: attraction only after AttractDelay from the end of the pop); no new tick, one more check in the existing subsystem loop; docs sys-pickups-lifecycle and sys-pickups updated with the property names.
- done_when: editor build succeeds with no new warning; existing pickup and pool automation tests pass (only expectations a change breaks are updated, no new test); defaults leave every Blueprint unchanged; docs updated; lint clean; committed by path.
- editor_state: closed
- token_ceiling: open
- model: opus
- tokens: input 56, cache write 89087, cache read 1504761, output 931
- runs: 1
- milestone: M1
- result: ARRPickup PopFallTimeRatio (1 = unchanged; apex at 1/(1+ratio) of the arc, same path, slower fall), bAttractDuringPop and PopAttractDelay (1.2 s from life start) via shared TryLatchMagnet; no tick; build clean; 34/35 tests pass, ReusedPickupEqualsFresh fails on rotation because of task-192 random RestOrientation (not this change); ratio and mid-air latch not exercised; commits 2bbfea1e, cfb07351

## Relations
- [[task-195-floaty-pop-midair-magnet]] — parent global task.
