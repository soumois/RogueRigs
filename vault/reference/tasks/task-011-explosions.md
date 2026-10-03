---
id: task-011-explosions
description: "Global task: feel tweaks (camera, shake, chaser wind-up), then explosions: queue and chains, barrels in the arena, kamikaze robot, assets, tests."
parent: tasks-index
kind: task
status: in-progress
---
# Explosions: barrel and kamikaze

- request: owner tweaks of 2026-10-02 (camera a bit closer, miner always centered, stronger block shake, faster chaser wind-up, take back BP_ArenaGrid) and the next M1 step: kamikaze, barrel, explosions. Owner validated values and plan ("je valide", 2026-10-02).
- goal: in PIE, barrels in the walls and kamikazes explode (shots, contact, death, chains every 0.08 s), destroy every block in their radius and hurt robots and the miner without one-shotting it; the camera is closer and centered; shake is visible; chasers hit after 0.1 s.
- acceptance: (1) build exits 0; all `RogueRigs.*` headless tests pass, new ones included; (2) arena_m1 has about 16 barrels and passes the validator; (3) a chain barrel → barrel resolves one link per 0.08 s, each source once; (4) values match [[enemy-kamikaze]], [[barrel]], [[explosions]], [[enemy-chaser]]; (5) docs `sys-explosions`, `sys-enemies`, `sys-grid`, `sys-spawner`, `sys-player`, Blueprint docs; lint clean; every subtask committed; (6) owner plays it.
- gdd_check: read [[enemy-kamikaze]], [[barrel]], [[explosions]], [[sys-explosions]], [[ore-blocks]], [[destruction-rules]], [[visual-style]] (miner always centered: matches). Fixed a stale rule (one explosion breaks one ore block) in [[enemy-kamikaze]] and [[ore-blocks]] after the 2026-10-01 decision. Open values set by the owner (2026-10-02): kamikaze, barrel, explosion radius and damage, chain delay 0.08 s, grey-box look, 16 barrels, spawn share 25 % kamikaze, chaser wind-up 0.1 s, camera arm 1700 uu without lag, shake 25 uu at 8 Hz plus 8 uu vertical. TaskCreate is not available in this session: progress is tracked in these nodes only.
- confirmed_by_owner: yes (2026-10-02, all subtasks pre-approved)
- milestone: M1
- next_step: task-011-03-explosion-system
- result: open

## Subtasks
1. task-011-01-feel-tweaks — cpp: camera, shake, wind-up values, tests updated.
2. task-011-02-layout-barrels — level-design: about 16 barrels in arena_m1.
3. task-011-03-explosion-system — cpp: explosion queue, chains, terrain and character damage, FX actor.
4. task-011-04-barrel — cpp: barrel cells with health, damaged by shots, explode when destroyed.
5. task-011-05-kamikaze — cpp: kamikaze robot and weighted spawner classes.
6. task-011-06-bp-explosions — bp: grey-box assets, spawner mix, PIE check.
7. task-011-07-tests-integration — cpp: integration tests of chains, terrain, damage cap.

## Relations
- [[tasks-index]] — parent index.
- [[task-011-01-feel-tweaks]] — step 1, cpp.
- [[task-011-02-layout-barrels]] — step 2, level-design.
- [[task-011-03-explosion-system]] — step 3, cpp.
- [[task-011-04-barrel]] — step 4, cpp.
- [[task-011-05-kamikaze]] — step 5, cpp.
- [[task-011-06-bp-explosions]] — step 6, bp.
- [[task-011-07-tests-integration]] — step 7, cpp.
