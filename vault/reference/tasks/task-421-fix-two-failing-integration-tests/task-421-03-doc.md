---
id: task-421-03-doc
description: "Doc: test doc nodes: the two fixes, known failures removed"
parent: task-421-fix-two-failing-integration-tests
kind: task
status: todo
---
# Fix the two pre-existing failing integration tests: doc

- goal: From the change summaries of 01 and 02: update the test family nodes with what each test now checks and why (task-229 physics body; task-243-05 deal gate), and remove the "known failures" line of sys-automation-tests (or state none left).
- agent: doc
- depends_on: task-421-01-juice-cube-collision-test, task-421-02-levelup-reroll-button-test
- inputs: agent-doc (vault/reference/agents/agent-doc.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md), sys-tests-juice-damage-numbers (vault/reference/tech/systems/tools/sys-automation-tests/sys-tests-juice-damage-numbers.md), sys-tests-progression (vault/reference/tech/systems/tools/sys-automation-tests/sys-tests-progression.md)
- expected_output: The three doc nodes updated, lint clean, one commit.
- done_when: sys-automation-tests no longer lists the two tests as known failures; sys-tests-juice-damage-numbers and sys-tests-progression describe the new checks with commit hashes; lint clean; committed.
- editor_state: any
- token_ceiling: 0.2 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M3
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-421-fix-two-failing-integration-tests]] — parent global task.
