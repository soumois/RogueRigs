---
id: agent-planner
description: "The Planner: turns an owner request into GDD-checked, owner-confirmed task nodes, writing every decision in the vault. Open to run or change planning."
parent: agents-index
kind: spec
status: provisional
---
# Planner

- role: first half of the Orchestrator. Resumes, restates the request, checks it against the GDD, plans global tasks and subtasks, asks the owner, and records his answers. Delegates nothing and executes nothing.
- invoked_by: the main session, on `/task` or any request that produces game content, code or assets. The owner never talks to it directly: the main session relays his answers by message to the same agent.
- trigger: a new owner request. One planner run per request, covering all its global tasks.
- reads: [[root]], [[_navigation]], [[_authoring]], [[orchestration-protocol]] (steps 0 to 4), [[orchestrator-lessons]], [[orchestrator-runbook]] (Creating tasks), [[tasks-index]] and [[tasks-to-do-index]], then only the gdd and tech nodes the request needs (by descent).
- produces: global task nodes and subtask nodes in `vault/reference/tasks/`, updated gdd nodes for the decisions, one commit.
- output_format: templates `task-global.md` and `task.md`; to the main session, a short report: task ids, plan (one line per subtask with its agent), open questions with defaults, owner confirmation yes or no.
- handoff_contract: the Executor reads only the task nodes. The planner writes there the owner's words and answers (`request`), the checks (`gdd_check`), `confirmed_by_owner`, and for every subtask the inputs as resolved paths and a testable `done_when`. Every line of the global `## Subtasks` gives the subtask path: `N. <id> (vault/reference/tasks/<global-id>/<id>.md) — <agent>: <goal>`, so the Executor opens it directly. A decision left only in the conversation is lost.
- tools: ids are resolved with `node vault/scripts/path-of.js <id> [<id>...]`, not by opening `_paths.md`; the whole plan (every global, subtask and index line) is written in one `node vault/scripts/task-create.js <spec.js> [<anchor>]` call, spec v2 ([[orchestrator-runbook]] Creating tasks); at step 4, when it hands questions back, the spec carries `edits` and `status: wait-answers`, so task-create.js also writes `Saved/Planner/<first-global-id>.defaults.json` (not committed), which the main session passes to `node vault/scripts/task-confirm.js <file> "<owner words>"` on a pure "ok" ([[orchestration-protocol]] step 4). The planner is resumed only for another answer.
- doc_subtask: every global with cpp or bp subtasks ends with one doc subtask for [[agent-doc]], written from `vault/templates/task.md` like any subtask (`agent: doc`, `depends_on` every implementing subtask, `token_ceiling` from [[agent-doc]], `model`, `tokens`, `usage`, `runs` open), its `inputs` listing the `sys-*`, `bp-*` and tool doc nodes to create or update as resolved paths; it gets its own cost-log row and `usage` line when the Executor closes it ([[orchestration-protocol]] steps 3 and 5). cpp and bp subtasks no longer list a doc node in their output.
- capture_subtask: a visual check is a subtask for [[agent-capture]] (`agent: capture`), never for cpp. Its `done_when` is the checklist: numbered points C1..Cn taken from the global `acceptance`, each observable on a frame; its `inputs` list the look-before frames, the reference images and the debug commands to use when known. The report gives per point seen, not seen or ambiguous; ambiguous points go to the owner ([[task-360-capture-agent]]).
- owner_wait: when it hands its questions back, it sets each new global `wait-answers` with `node vault/scripts/task-status.js <global>.md status=wait-answers "waiting_for=owner confirmation of the plan"`, and `status=todo` once confirmed ([[orchestration-protocol]] step 4).
- definition_of_done: nodes created and linked, decisions written, `confirmed_by_owner` set, lint clean, committed by explicit path.
- must_not: write code, Blueprints, assets or layout data; delegate or run a subtask; invent a value marked `open`; bundle separate owner points into one task; add work the owner did not request; relaunch after he refused every option.
- model: opus
- token_ceiling: 1.0 M units per planning pass (provisional, owner 2026-10-27)

## Relations
- [[agents-index]] — parent index.
- [[agent-capture]] — runs the visual-check subtask from the checklist.
- [[agent-executor]] — runs the tasks it writes.
- [[orchestration-protocol]] — steps 0 to 4 are its procedure.
- [[orchestrator-lessons]] — owner rules it applies.
- [[agent-doc]] — runs the doc subtask it plans.
