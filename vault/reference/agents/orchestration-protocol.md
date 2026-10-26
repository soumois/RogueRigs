---
id: orchestration-protocol
description: "Procedure the Planner and Executor follow for every request: restate, GDD check, plan, confirm, delegate, verify, record; how to resume after a limit."
parent: agents-index
kind: rule
applies_to: orchestrator
---
# Orchestration protocol

## Roles
`/task` splits the procedure between two agents (owner 2026-10-26). The [[agent-planner]] does steps 0 to 4 and writes every decision into the task nodes; the [[agent-executor]] does steps 5 and 6 for one confirmed global task, from those nodes only, in a fresh context. Executor step 0: check `git status` and the task's `next_step`; it does not read `tasks-index` or report other tasks.

## 0. Resume first
On every start, read [[tasks-index]]. If a global task is `in-progress`, read it, open its `next_step` subtask, check `git status` for partial work, and continue from there. Do not take a new request before reporting where the current one stands.

## 1. Restate
Write the request as a goal with observable acceptance checks. Ask if the request is ambiguous; if a sentence can be read two ways with opposite effects, ask before planning.

One logically separate point of the request = one global task (task-NNN). A play-test message with ten points gives up to ten global tasks, never one task with ten parts (owner 2026-10-06).

## 2. GDD check
Descend to the gdd nodes the request touches. Report to the owner, before any work:
- conflicts between the request and the GDD;
- values the work needs that are marked `open`;
- scope outside the current milestone.
If any exist, stop and ask whether to continue, adjust the request, or update the GDD.

## 3. Plan
Create the global task node. Add subtasks (task-NNN-SS) only when that one request spans several systems or agents (example: the 67 uu grid needed render, flow field, layout and switch subtasks). A subtask:
- has one agent and one output;
- is small enough to finish inside one agent session, well under its token ceiling;
- lists `depends_on`, `done_when` and `editor_state`;
- lists its inputs as resolved paths (from `_paths.md`), so the agent reads them directly instead of descending again; re-reading them in each subtask is the accepted cost of independent subtasks.

Plan only what the owner asked: no new tests, bot runs, screenshot sessions or features he did not request; only update existing tests that a requested change breaks. The plan shown to the owner lists exactly the tasks and subtasks that will run.
Order subtasks so that every dependency comes first.

bp split (owner 2026-10-14): when one owner point needs several editor assets, one bp subtask = one main asset (a Niagara system, a widget, a material with its instances, or a Blueprint), with its doc node and its own commit. Order: materials, then Niagara and widgets, then the Blueprint that wires them, then map placement. Independent bp subtasks on different assets may run in parallel, 3 at most on one editor.

## 4. Confirm
Show the plan once. A "no" to every proposed option means no task: ask or wait, never relaunch work on a reinterpreted target. The owner confirms the global task; subtasks then run without further confirmation. A second global task requested meanwhile waits for its own confirmation.

## 5. Execute, one subtask at a time
1. Set the subtask `in-progress` and the global `next_step` to it; commit.
2. Prepare the editor state (close Unreal before C++ builds; launch it before BP work).
3. Delegate to the subagent with the subtask node and the resolved paths of its inputs as the only reading list.
4. Run the `done_when` checks.
5. Pass: fill `result`, `model`, `tokens` (input, cache write, cache read, output from `node vault/scripts/agent-usage.js`) and `runs`, add the subtask to [[cost-log]] (turns, context, resumes; read its ALERT lines and write each cause in `result`), set `done`, move `next_step`, lint, commit `task-<id>: done`.
6. Fail: send the error back to the same agent once. Second failure: set `blocked`, record why, stop and report.

## 6. Close
Run the global acceptance checks, fill `result`, set `done`, `next_step: none`, commit, report to the owner.

## Relations
- [[orchestrator-runbook]] — the commands that apply this protocol.
- [[orchestrator-lessons]] — owner rules and planning lessons: read before every plan; agent limits are in [[bp-mcp-lessons]] and [[art-3d-lessons]].
- [[agents-index]] — parent index.
- [[agent-planner]] — runs steps 0 to 4.
- [[agent-executor]] — runs steps 5 and 6.
- [[cost-log]] — where step 5 records tokens per subtask.
