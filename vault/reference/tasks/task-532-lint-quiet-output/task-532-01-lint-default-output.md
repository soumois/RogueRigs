---
id: task-532-01-lint-default-output
description: "Cpp: Make lint.js print by default only errors, warnings of modified vault files and the summary line, with --all for the full list."
parent: task-532-lint-quiet-output
kind: task
status: done
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
- model: opus
- tokens: included in the executor run of task-532
- usage: included in the executor run of task-532
- runs: 1
- milestone: M2
- started: 2026-11-13T18:19:38Z
- waiting_for: none
- result: lint.js: default output = errors (always), warnings of vault files differing from HEAD (modified, staged, untracked via git diff HEAD + ls-files --others), summary line with total and hidden count ('N warning(s) (K hidden, --all shows them)'); --all prints all. Machine-level hook warning hidden by default (needed for A1). Runs: clean --write -> summary only, 74 hidden, exit 0; --all -> 74 warn lines; touched verification-levels description -> its warning shown, also when staged; untracked probe -> its warning plus errors in unchanged agents-index shown, exit 1; dead link -> error, exit 1; restored -> clean. task-quiet filters unchanged (^warn:, ^vault OK). Commit of lint.js above.

## Relations
- [[task-532-lint-quiet-output]] — parent global task.
