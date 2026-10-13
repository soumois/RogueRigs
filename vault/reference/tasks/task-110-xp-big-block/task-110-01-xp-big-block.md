---
id: task-110-01-xp-big-block
description: "When many XP cubes fall in one place they become one bigger block of another colour worth the same total XP."
parent: task-110-xp-big-block
kind: task
status: todo
---
# Big XP block (C++ rule)

- goal: When many XP cubes fall in one place they become one bigger block of another colour worth the same total XP.
- agent: cpp
- depends_on: task-109-01-weapon-slot-icons
- inputs: xp-level-up (vault/reference/gdd/run/xp-level-up.md), sys-pickups (vault/reference/tech/systems/sys-pickups.md), sys-actor-pool (vault/reference/tech/systems/sys-actor-pool.md), bp-pickup-xp (vault/reference/tech/blueprints/bp-pickup-xp.md), color-code (vault/reference/gdd/art-audio/color-code.md), perf-trace-real-game (vault/reference/tech/performance/perf-trace-real-game.md), sys-automation-tests (vault/reference/tech/systems/sys-automation-tests.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: owner 2026-10-12 (also because the pool can delay XP creation): when many XP cubes would fall at one place, drop bigger blocks instead. Rule (provisional values as data): XP cube requests are collected per frame per 200 uu bucket in URRPickupSubsystem (or in ARREnemy::DropXP if the subsystem path is not the right place: read sys-pickups and the code); when a bucket gets more than 8 cubes in a frame, groups of 8 cubes become ONE big block worth the sum of their XP (the total XP is exactly preserved: integer or float sums as the code does), the remainder stays as small cubes; the big block is another pooled pickup class (XPBigPickupClass, an EditDefaultsOnly class on the subsystem/data, default null = feature off, set by task-110-02) with the same magnet, pickup radius and collect rules as a cube (same ARRPickup class family; a field XPValue carries the sum). Keep the pool behaviour (MaxFrames 3, release on collect, reset hooks, the root template scale rule of task-095) and the heal/ore behaviour. Tests: 8 or fewer cubes unchanged; 20 cubes in one bucket = 2 big + 4 small with the same total XP; two buckets far apart are not merged; total XP collected equals total XP dropped; the feature off when the class is null. Docs sys-pickups, sys-actor-pool, sys-automation-tests. Editor closed.
- done_when: build exits 0 with the editor closed; all RogueRigs.* tests pass headless; docs updated; lint clean; committed.
- editor_state: closed
- token_ceiling: 60000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-110-xp-big-block]] — parent global task.
