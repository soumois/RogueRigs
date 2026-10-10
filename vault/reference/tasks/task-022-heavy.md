---
id: task-022-heavy
description: "Global task: heavy robot with a telegraphed ground slam, high knockback resistance, corridor-fitting capsule; joins the spawner mix with a cap of 5."
parent: tasks-done-index
kind: task
status: done
---
# Heavy robot

- request: "pourrais aussi implémenter le gros robot ?"; values and plan confirmed "Je valide le plan" (2026-10-04).
- goal: heavies (300 HP, slow, 2× visual) walk every corridor, telegraph 0.8 s, slam 25 damage in 220 uu, resist knockback 0.8, are 8 % of spawns with at most 5 alive.
- acceptance: (1) build exits 0; all tests pass, new ones included; (2) slam timing, radius, damage, no terrain damage, knockback resistance, one-cell corridor traversal (tests); (3) spawner mix 65/12/15/8 and cap 5 on the placed map instance (test); (4) readable in PIE; (5) docs sys-enemies children, new Blueprint doc; lint clean; committed.
- gdd_check: Read [[enemy-heavy]], [[enemies-index]], [[arena-grid]], [[destruction-rules]], [[color-code]], [[xp-level-up]]. Conflict "twice the size" vs one-cell corridors resolved by the owner: 2× visual, capsule radius ≤ 80 uu. Open values set by the owner (2026-10-04, provisional). Weak point stays in the wishlist.
- confirmed_by_owner: yes (2026-10-04, all subtasks pre-approved)
- milestone: M3
- next_step: none
- result: All acceptance checks passed: 122/122 tests (slam timing, radius, damage, no terrain damage, knockback 0.8, corridor crossing, placed spawner 65/12/15/8 with cap 5); BP_Heavy readable in PIE after the ShockRing fix; docs sys-enemies-heavy, bp-heavy, bp-enemy-spawner. For the owner to judge in play: heavy pressure, slam readability; heavies often die before reaching the miner.

## Subtasks
1. task-022-01-heavy-cpp — cpp: heavy class, slam and telegraph, CSV row, corridor capsule, spawner entry and cap, tests
2. task-022-02-bp-heavy — bp: BP_Heavy grey-box, telegraph and shockwave visuals, spawner weights, PIE
3. task-022-03-tests-integration — cpp: integration tests with the final Blueprints

## Relations
- [[tasks-done-index]] — parent index.
- [[task-022-01-heavy-cpp]] — step 1, cpp.
- [[task-022-02-bp-heavy]] — step 2, bp.
- [[task-022-03-tests-integration]] — step 3, cpp.
