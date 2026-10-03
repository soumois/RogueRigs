---
id: task-016-04-tests-integration
description: "Integration tests with the final Blueprints: alert, console drop, clear marked rocks, hack, shuttle, win; and the timeout loss."
parent: task-016-console-extraction
kind: task
status: done
---
# Objectives integration tests

- goal: the full level loop is covered by headless tests.
- agent: cpp
- depends_on: task-016-03-bp-objectives
- inputs: [[sys-automation-tests]], `sys-objectives`, task-016-03 result.
- expected_output: `RogueRigs.Integration.*` tests driving the real Blueprints with an accelerated alert: console appears, marked rocks destroyed, hack completes, shuttle lands, miner reaches it → win; second test → timeout loss; full test run.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless (new ones included); docs updated; lint clean.
- editor_state: closed
- token_ceiling: 70000
- milestone: M1
- result: RogueRigs.Integration.ConsoleExtractionWin (console drop with 4 marked rocks, alert held at 0.4, HUD texts, rocks cleared, hack Hacking → Complete, shuttle ≥ 25 cells with 45 s, pending level-up then extraction won) and ShuttleTimeoutLoss (3 s override, lost on time); shared end check: real pause and cursor, end title, offers blocked, 2 s frozen, second end refused, Rejouer bound and reloads the map. Fixed a HUD widget GC hazard in the harness (also in LevelUpUpgradeLoop). Build 0, 95/95. Docs sys-automation-tests, sys-objectives. Commit 803ff2c, about 75k tokens (a first run was cut by an API limit).

## Relations
- [[task-016-console-extraction]] — parent global task.
