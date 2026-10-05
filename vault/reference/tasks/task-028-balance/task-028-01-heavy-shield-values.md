---
id: task-028-01-heavy-shield-values
description: "Heavy slam radius 400 uu; shield chaser (30 shield before health, shown as a shell); upgrade value table raised; tests."
parent: task-028-balance
kind: task
status: todo
---
# Heavy, shield chaser, upgrade values

- goal: balance and enemy changes in C++ and data.
- agent: cpp
- depends_on: task-027-03-tests-integration
- inputs: [[enemy-heavy]], [[enemy-chaser]], [[upgrades]], [[sys-enemies]], [[sys-upgrades]], [[sys-damage]], [[cpp-standard]], [[sys-automation-tests]], [[drgs-analysis]]
- expected_output: heavy: slam radius 400 uu, trigger distance scaled (0.7 × radius), telegraph unchanged; shield chaser: chaser variant with a 30-point shield absorbing damage before health, shield break event (OnShieldBroken) and shield fraction getter, same speed, xp 2, spawned in chaser packs at 25 % of chasers after 1:00; upgrade values: % stats 10/15/25/30/40, max health 20/30/45/60/80, crit chance 3/5/8/11/15 points, crit damage 0.1/0.2/0.3/0.4/0.5, luck unchanged, targeted ×1.5 unchanged; docs sys-enemies children, sys-upgrades; tests updated.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless (new ones included); docs updated; lint clean.
- editor_state: closed
- token_ceiling: 110000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-028-balance]] — parent global task.
