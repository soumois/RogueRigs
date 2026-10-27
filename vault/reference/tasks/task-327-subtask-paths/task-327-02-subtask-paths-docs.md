---
id: task-327-02-subtask-paths-docs
description: "Doc agent writes the execution exception to the navigation rule, the new task-script output and the Subtasks path format; adds paths to 308, 309, 314."
parent: task-327-subtask-paths
kind: task
status: todo
---
# Execution exception and subtask paths in the crew definitions

- goal: from the brief (commit and change summary of [[task-327-01-subtask-paths-scripts]], owner decisions (1) to (3) and Planner decisions D1 to D5 in [[task-327-subtask-paths]] `request` and `gdd_check`). Edit: `_navigation` (owner intent 2026-10-27, quoted in the global `request`): add the execution exception (during execution the Executor and the agents it launches may open a task or subtask node directly from a path printed by `task-start.js` / `task-next.js` or listed in the global `## Subtasks`, without descending or re-walking; searching with grep, glob or find to locate a node stays forbidden; never build a task path by analogy), and in "Resolving a wikilink" point 3 say the global `## Subtasks` gives each subtask path; reading outside execution unchanged. `orchestration-protocol` (owner intent, same quote): Executor step 0 and step 5.1 (`task-start.js` / `task-next.js` end with `next: <path>`, open that path directly), step 3 (Subtasks lines carry the path, D2). `agent-executor`: `reads` and `start` (subtask nodes opened from the `next:` line or the Subtasks path; never guessed, never searched). `orchestrator-runbook`: Creating tasks (task-create.js writes the D2 form; a hand-written subtask line follows it), Starting a task and Closing a subtask (only D3 kept lines are printed, D4 leftovers, last line `next: <path>` or `next: none`), Delegation (hand the agent its subtask path as given). `agent-planner` `handoff_contract`: every Subtasks line gives the path (D2). Template `task-global.md` `## Subtasks` placeholder in the D2 form. `.claude/agents/executor.md` step 1 and 2: subtask paths come from the global Subtasks list and the scripts' `next:` line; open them directly; never guess a path or search. Global nodes `task-308-chest-lock-light`, `task-309-chest-opening`, `task-314-chest-lid-logo`: rewrite each `## Subtasks` line in the D2 form (13 lines, ids and goals unchanged, nothing else in those nodes). Done global nodes are not touched (D5). `_authoring` is not edited (it does not restate the rule).
- agent: doc
- depends_on: task-327-01-subtask-paths-scripts
- inputs: task-327-subtask-paths (vault/reference/tasks/task-327-subtask-paths.md), _navigation (vault/_navigation.md), orchestration-protocol (vault/reference/agents/orchestration-protocol.md), agent-executor (vault/reference/agents/agent-executor.md), orchestrator-runbook (vault/reference/agents/orchestrator-runbook.md), agent-planner (vault/reference/agents/agent-planner.md), template task-global (vault/templates/task-global.md), executor definition (.claude/agents/executor.md), task-308-chest-lock-light (vault/reference/tasks/task-308-chest-lock-light.md), task-309-chest-opening (vault/reference/tasks/task-309-chest-opening.md), task-314-chest-lid-logo (vault/reference/tasks/task-314-chest-lid-logo.md), subtask folders vault/reference/tasks/task-308-chest-lock-light/, vault/reference/tasks/task-309-chest-opening/, vault/reference/tasks/task-314-chest-lid-logo/
- expected_output: the edited nodes and file above; a short recap.
- done_when: `_navigation` states the execution exception, cites the owner's intent of 2026-10-27, keeps "never search" for every case and leaves non-execution reading unchanged; protocol, agent-executor, runbook, agent-planner, template and executor.md describe the `next: <path>` last line, the kept and dropped output and the D2 Subtasks form consistently with the cpp change summary; every Subtasks line of 308, 309 and 314 carries a path that exists (checked by listing the subtask folders) and no other line of those nodes changed; lint clean; committed by explicit path.
- editor_state: any
- token_ceiling: 0.3 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-327-subtask-paths]] — parent global task.
