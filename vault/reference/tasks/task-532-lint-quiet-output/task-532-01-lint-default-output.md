---
id: task-532-01-lint-default-output
description: "Cpp: Make lint.js print by default only errors, warnings of modified vault files and the summary line, with --all for the full list."
parent: task-532-lint-quiet-output
kind: task
status: todo
---
# lint.js : sortie courte par défaut: cpp

- goal: Make lint.js print by default only errors, warnings of modified vault files and the summary line, with --all for the full list.
- agent: cpp
- depends_on: none
- inputs: vault/scripts/lint.js
- expected_output: see done_when
- done_when: A1 to A6 of the global hold, shown by real lint runs in the subtask report (clean tree, one touched node, --all).
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-532-lint-quiet-output]] — parent global task.
