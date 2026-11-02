---
id: task-420-04-doc
description: "Doc: test docs: tests read expected values from the CSV through the helper"
parent: task-420-single-source-game-values
kind: task
status: done
---
# Game values in one place: the CSV tables: doc

- goal: Document the CSV-reading helper and the rule in the test docs; replace expectation numbers that copy CSV values in the sys-tests nodes by the row citation.
- agent: doc
- depends_on: task-420-01-weapon-tests-read-csv, task-420-02-other-tests-read-csv
- inputs: task-420-single-source-game-values (vault/reference/tasks/task-420-single-source-game-values.md), agent-doc (vault/reference/agents/agent-doc.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md), sys-tests-weapons (vault/reference/tech/systems/tools/sys-automation-tests/sys-tests-weapons.md), sys-tests-weapon-types (vault/reference/tech/systems/tools/sys-automation-tests/sys-tests-weapon-types.md), sys-tests-enemies (vault/reference/tech/systems/tools/sys-automation-tests/sys-tests-enemies.md), sys-tests-progression (vault/reference/tech/systems/tools/sys-automation-tests/sys-tests-progression.md)
- expected_output: updated sys-automation-tests and sys-tests-* nodes; one commit.
- done_when: sys-automation-tests names the helper and the rule; lint clean.
- editor_state: any
- token_ceiling: 0.3 M units
- model: sonnet
- tokens: input 20, cache write 51691, cache read 174645, output 8935
- usage: turns 11, context start 11k peak 41k, resumes 0, 1 min, units 0.13 M
- runs: 2
- milestone: M3
- started: 2026-11-02T19:59:33Z
- waiting_for: none
- result: Done: sys-automation-tests documents RRTestStatsRows.h, the single-source rule and 2 known pre-existing failures; 4 sys-tests-* cite CSV rows. Commit 3a98d11c. First doc run cut by usage limit, rerun finished from its uncommitted edits (counted as 2 runs). Rule in _authoring/cpp-standard already present from confirmation.

## Relations
- [[task-420-single-source-game-values]] — parent global task.
