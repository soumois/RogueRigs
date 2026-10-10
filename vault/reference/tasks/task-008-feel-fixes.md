---
id: task-008-feel-fixes
description: "Global task: owner PIE feedback: fixed shot height, rifle no longer blocks movement, longer pause between bursts, pickups, shake, readable damage text, bigger dummy."
parent: tasks-done-index
kind: task
status: done
---
# Feel fixes after the first PIE test

- request: owner feedback on 2026-10-02: pickups attract too soon; the gun seems to hamper movement; shake should be wider and a bit slower; the dummy is too small, shots pass over it; the damage number is too light to read; burst speed fine but not enough time between bursts. Owner confirmed the proposed values (damage stays 10) on 2026-10-02.
- goal: shots leave the miner at a fixed height under the block top and hit a floor-standing dummy; the miner moves freely while firing; bursts pause 0.64 s; pickups rest 1 s before attraction; blocks shake 15 uu at 10 Hz; damage numbers readable on the grey-box floor; dummy 160 uu.
- acceptance: (1) build exits 0; all `RogueRigs.*` tests pass headless, including `Weapons.MinerStartingWeapon` (shot spawn height below the block top) and a new test where a shot fired by `BP_Miner` hits a `BP_TargetDummy` standing on the floor; (2) a test shows the miner capsule is not blocked by its own projectiles; (3) values match [[burst-rifle]], [[sys-pickups]], [[sys-grid]], `sys-target-dummy` (removed in task-031-01); (4) docs match; lint clean, every subtask committed; closes [[task-007-burst-rifle-dummy]]; (5) owner checks the feel.
- gdd_check: read [[burst-rifle]], [[destruction-rules]] (nobody shoots over a block → shots below the block top), [[damage-numbers]], [[color-code]], [[sys-pickups]], [[sys-grid]]. Owner decisions 2026-10-02: FireRate 1.25 bursts/s with BurstDuty 0.2 (gap 0.08 s, pause 0.64 s), damage 10; pickup attraction delay 1 s; shake 15 uu at 10 Hz; damage text with a dark outline, sizes 60 / 90; dummy 160 uu; fixed shot height 50 uu.
- confirmed_by_owner: yes
- milestone: M1
- next_step: none
- result: acceptance 1 to 4 passed: 14/14 headless tests, movement bug found and fixed (rifle mesh collision, not projectiles), shots at 50 uu hit a floor dummy, values applied. Closes task-007. Owner confirmed the features work in PIE (2026-10-02); value tuning will come as a later task. About 300k tokens.

## Subtasks
1. task-008-01-shot-height-movement — cpp: fixed shot height, weapon on the body, capsule ignores own projectiles, burst values, tests.
2. task-008-02-feel-values — cpp: pickup delay, shake, readable damage text.
3. task-008-03-bp-miner-dummy — bp: BP_Miner without hand socket, dummy 160 uu on the floor, save.
4. task-008-04-tests-final — cpp: dummy-hit test with the final Blueprints, full headless run.

## Relations
- [[tasks-done-index]] — parent index.
- [[task-008-01-shot-height-movement]] — step 1, cpp.
- [[task-008-02-feel-values]] — step 2, cpp.
- [[task-008-03-bp-miner-dummy]] — step 3, bp.
- [[task-008-04-tests-final]] — step 4, cpp.
