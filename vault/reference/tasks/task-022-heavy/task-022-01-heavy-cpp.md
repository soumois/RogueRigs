---
id: task-022-01-heavy-cpp
description: "Heavy class on the robot base: telegraphed ground slam, high knockback resistance, corridor-fitting capsule, CSV row, spawner entry with an alive cap; tests."
parent: task-022-heavy
kind: task
status: in-progress
---
# Heavy C++

- goal: the heavy works in C++.
- agent: cpp
- depends_on: none
- inputs: [[enemy-heavy]], [[enemies-index]], [[sys-enemies]] (children base, chaser, shooter), [[sys-enemy-ai]], [[sys-flow-field]], [[sys-spawner]], [[sys-damage]], [[sys-automation-tests]], [[cpp-standard]].
- expected_output: heavy class on ARREnemy using the shared helpers (states Chase, Telegraph, Slam, Recover, Dead); EnemyStats.csv row (hp 300, speed 200, damage 25, slam radius 220, interval 2.5 s, telegraph 0.8 s, xp 5, knockback resistance 0.8); capsule radius ≤ 80 uu with a test that it crosses a one-cell corridor; slam hits the miner within 220 uu (no terrain damage, no robot damage); BlueprintImplementableEvents on the heavy itself (OnTelegraphStart, OnSlam(Location, Radius), OnRecover) for visuals; spawner support for a per-class alive cap (heavy 5) usable by Blueprint defaults; docs: new child node sys-enemies-heavy, sys-enemies index, sys-spawner. Tests `RogueRigs.Heavy.*`.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless (new ones included); docs updated; lint clean.
- editor_state: closed
- token_ceiling: 120000
- model: open
- tokens: open
- runs: open
- milestone: M3
- result: open

## Relations
- [[task-022-heavy]] — parent global task.
