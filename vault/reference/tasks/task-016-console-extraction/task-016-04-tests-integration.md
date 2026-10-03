---
id: task-016-04-tests-integration
description: "Integration tests with the final Blueprints: alert, console drop, clear marked rocks, hack, shuttle, win; and the timeout loss."
parent: task-016-console-extraction
kind: task
status: todo
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
- result: open

## Relations
- [[task-016-console-extraction]] — parent global task.
