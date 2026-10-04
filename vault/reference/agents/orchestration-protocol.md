---
id: orchestration-protocol
description: "Procedure the Orchestrator follows for every request: restate, GDD check, plan, confirm, delegate, verify, record; how to resume after a limit."
parent: agents-index
kind: rule
applies_to: orchestrator
---
# Orchestration protocol

## 0. Resume first
On every start, read [[tasks-index]]. If a global task is `in-progress`, read it, open its `next_step` subtask, check `git status` for partial work, and continue from there. Do not take a new request before reporting where the current one stands.

## 1. Restate
Write the request as a goal with observable acceptance checks. Ask if the request is ambiguous.

## 2. GDD check
Descend to the gdd nodes the request touches. Report to the owner, before any work:
- conflicts between the request and the GDD;
- values the work needs that are marked `open`;
- scope outside the current milestone.
If any exist, stop and ask whether to continue, adjust the request, or update the GDD.

## 3. Plan
Create the global task node and its subtasks. A subtask:
- has one agent and one output;
- is small enough to finish inside one agent session, well under its token ceiling;
- lists `depends_on`, `done_when` and `editor_state`.
Order subtasks so that every dependency comes first.

## 4. Confirm
Show the plan once. The owner confirms the global task; subtasks then run without further confirmation. A second global task requested meanwhile waits for its own confirmation.

## 5. Execute, one subtask at a time
1. Set the subtask `in-progress` and the global `next_step` to it; commit.
2. Prepare the editor state (close Unreal before C++ builds; launch it before BP work).
3. Delegate to the subagent with only the subtask node as input.
4. Run the `done_when` checks.
5. Pass: fill `result`, `model`, `tokens` (input, cache write, cache read, output from `node vault/scripts/agent-usage.js`) and `runs`, add the subtask to [[cost-log]], set `done`, move `next_step`, lint, commit `task-<id>: done`.
6. Fail: send the error back to the same agent once. Second failure: set `blocked`, record why, stop and report.

## 6. Close
Run the global acceptance checks, fill `result`, set `done`, `next_step: none`, commit, report to the owner.

## Relations
- [[agents-index]] — parent index.
- [[agent-orchestrator]] — the agent bound by this protocol.
- [[cost-log]] — where step 5 records tokens per subtask.
