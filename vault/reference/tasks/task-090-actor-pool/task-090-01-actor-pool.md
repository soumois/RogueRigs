---
id: task-090-01-actor-pool
description: "A generic actor pool subsystem reuses actors and spreads the creation of missing ones over a capped number of frames."
parent: task-090-actor-pool
kind: task
status: in-progress
---
# Generic actor pool with staggered creation

- goal: A generic actor pool subsystem reuses actors and spreads the creation of missing ones over a capped number of frames.
- agent: cpp
- depends_on: none
- inputs: cpp-standard (vault/reference/tech/conventions/cpp-standard.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), sys-pickups (vault/reference/tech/systems/sys-pickups.md), sys-enemies-base (vault/reference/tech/systems/sys-enemies/sys-enemies-base.md), sys-spawner (vault/reference/tech/systems/sys-spawner.md), sys-automation-tests (vault/reference/tech/systems/sys-automation-tests.md), perf-trace-real-game (vault/reference/tech/performance/perf-trace-real-game.md)
- expected_output: a world subsystem (for example URRActorPoolSubsystem) used by every system that spawns many actors: Acquire(Class, Transform, Params, MaxFrames) returns or later delivers (callback or handle) an actor: a free pooled instance is reset and reused at once; when none is free, the costly creation is spread over at most MaxFrames frames (creations per frame = ceil(pending / MaxFrames), never more than needed) and the actor is delivered when ready; Release(Actor) returns it to the pool (hidden, collision and tick off, removed from gameplay queries) and calls a reset hook on the actor (an interface or a BlueprintNativeEvent such as OnPoolReset / OnPoolAcquired; the owner of each class implements it in its own task). Optional prewarm(Class, Count) spread over frames, default none. Counters for tests and for the stress logs (pool size, free, acquired, created this frame, pending, longest wait in frames). A pooled actor must never be counted as alive by systems that count live actors (document how callers distinguish). Keep gameplay unchanged: this task only adds the class, its tests (reuse, reset hook called, spread respects MaxFrames, release/re-acquire, counters) and its doc node sys-actor-pool in tech/systems; no caller is migrated here (tasks 091 and 092 do). Editor closed.
- done_when: build exits 0 with the editor closed; all RogueRigs.* tests pass headless; sys-actor-pool written and linked from systems-index; lint clean; committed.
- editor_state: closed
- token_ceiling: 80000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-090-actor-pool]] — parent global task.
