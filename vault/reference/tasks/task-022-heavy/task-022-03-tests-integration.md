---
id: task-022-03-tests-integration
description: "Integration tests with the final Blueprints: heavy telegraphs and slams the miner, resists the shotgun knockback, crosses a corridor, placed spawner mix and cap."
parent: task-022-heavy
kind: task
status: todo
---
# Heavy integration tests

- goal: the heavy is covered with the real Blueprints.
- agent: cpp
- depends_on: task-022-02-bp-heavy
- inputs: [[sys-automation-tests]], task-022-02 result.
- expected_output: `RogueRigs.Integration.*` tests with BP_Heavy, BP_Miner and the arena: telegraph then slam timing and damage, no slam damage outside 220 uu, shotgun knockback reduced by 0.8, one-cell corridor crossing, placed L_ArenaM1 spawner effective weights 65/12/15/8 and heavy cap 5; full run.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless (new ones included); docs updated; lint clean.
- editor_state: closed
- token_ceiling: 70000
- model: open
- tokens: open
- runs: open
- milestone: M3
- result: open

## Relations
- [[task-022-heavy]] — parent global task.
