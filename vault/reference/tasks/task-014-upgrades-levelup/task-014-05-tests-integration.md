---
id: task-014-05-tests-integration
description: "Integration tests with the final Blueprints: kill robots, level up, paused offer appears, reroll, pick, stat changed, resume."
parent: task-014-upgrades-levelup
kind: task
status: in-progress
---
# Upgrade integration tests

- goal: the full level-up loop is covered by headless tests.
- agent: cpp
- depends_on: task-014-04-bp-upgrade-ui
- inputs: [[sys-automation-tests]], [[sys-upgrades]], [[sys-progression]], task-014-04 result.
- expected_output: `RogueRigs.Integration.*` test(s): with the real miner and spawner Blueprints, gain a level, check the heal, wait the delay, check pause and 3 valid cards, reroll, choose a damage card, check the rifle damage rose and the game resumed; full test run.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless (new ones included); docs updated; lint clean.
- editor_state: closed
- token_ceiling: 70000
- milestone: M1
- result: open

## Relations
- [[task-014-upgrades-levelup]] — parent global task.
