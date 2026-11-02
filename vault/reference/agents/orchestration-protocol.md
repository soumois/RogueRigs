---
id: orchestration-protocol
description: "Procedure the Planner and Executor follow for every request: restate, GDD check, plan, confirm, delegate, verify, record; how to resume after a limit."
parent: agents-index
kind: rule
applies_to: orchestrator
---
# Orchestration protocol

## Roles
`/task` splits the procedure between two agents (owner 2026-10-26). The [[agent-planner]] does steps 0 to 4 and writes every decision into the task nodes; the [[agent-executor]] does steps 5 and 6 for one confirmed global task, from those nodes only, in a fresh context. Once confirmed, the Executor owns all execution, resumes and subagent handoffs; it does not recall the Planner mid-task. Executor step 0: check `git status` and the task's `next_step`, then run `node vault/scripts/task-start.js <global-id>` (a no-op when already started) and open the subtask path on its last line `next: <path>` directly (execution exception of [[_navigation]], owner 2026-10-27: no descent, no search); it does not read `tasks-index` or report other tasks.

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
- is listed in the global `## Subtasks` as `N. <id> (<repo-relative path>) — <agent>: <goal>`, so the Executor opens it directly (`task-create.js` writes this form);
- lists its inputs as resolved paths (from `node vault/scripts/path-of.js <id>`), so the agent reads them directly instead of descending again; re-reading them in each subtask is the accepted cost of independent subtasks.

For a large homogeneous workload that would otherwise go to one oversized subtask, keep one global task and split it upfront into multiple bounded subtasks with the same `agent` role. This fits broad C++ work across independent test or system families: give each slice disjoint families, explicit files and checks, and a cost ceiling comfortably below the role's limit. For similarly sized test families, target roughly 4–5 families per C++ slice, fewer when complexity warrants; do not split tightly coupled changes or create per-file microtasks. Each subtask is a fresh agent invocation with its own cost row. The Executor runs same-role slices in listed order by default; parallelize only when the plan establishes independence and the slices share no files, editor state or other mutable resource. The split is a measurement trial, not a guaranteed token saving: compare the sum of slice costs with the equivalent prior workload.

Plan only what the owner asked: no new tests, bot runs, screenshot sessions or features he did not request; only update existing tests that a requested change breaks. The plan shown to the owner lists exactly the tasks and subtasks that will run.
Order subtasks so that every dependency comes first.

Doc subtask (owner 2026-10-27, [[task-325-doc-agent]]): a global with cpp or bp subtasks ends with one subtask for [[agent-doc]], a real node from `vault/templates/task.md` (`agent: doc`, `token_ceiling`, `model`, `tokens`, `usage`, `runs`), `depends_on` every implementing subtask, `inputs` the `sys-*`, `bp-*` and tool doc nodes to create or update. cpp and bp subtasks no longer write those nodes; they return a change summary. Art, 3d, level-design and text keep writing their own nodes. Tasks planned before it (308, 309, 314) are not re-planned.

Capture subtask (owner 2026-10-29, [[task-360-capture-agent]]): a visual check is a subtask for [[agent-capture]] (`agent: capture`), not for cpp. Its `done_when` is a checklist of numbered points C1..Cn taken from the global `acceptance`, each observable on a frame; its `inputs` list the look-before frames, the reference images and the debug commands to use when known. The agent reports each point seen, not seen or ambiguous with the frame or crop path; ambiguous points go to the owner, the Executor does not decide them. Tasks 308, 309, 314 keep their cpp capture subtasks as written.

Uncertainty-driven representative slice (owner 2026-11-01): use this only when the visual result or technical integration is materially uncertain, not for every small change. Keep one global task per owner request. The Planner records the expected result, references, rules to preserve, open decisions and useful resolutions; it says whether correction belongs in an asset, Blueprint, ABP or C++. For an interface, plan one representative element with its key states (for example one shop card with purchase and sold states); for animation, one clip in a game state; for an effect, one effect wired into play; for technical integration, one end-to-end path. A validated mockup may cover the slice and remove the extra check.

Order an implementing slice subtask, then a [[agent-capture]] subtask for one checklist across the useful resolutions, then any expansion subtasks with `depends_on` the passing capture. Nonvisual technical slices use the relevant build, test or integration check instead of a screenshot. A failed capture stops expansion: the Executor sends the observed defect to the same implementer for one focused correction and repeats the check. Ambiguous results or multiple viable directions wait for the owner; a clear passing slice proceeds without another owner checkpoint. The prototype stays inside this one global task; the final doc subtask and per-subtask cost recording still apply.

bp split (owner 2026-10-14): when one owner point needs several editor assets, one bp subtask = one main asset (a Niagara system, a widget, a material with its instances, or a Blueprint), with its own commit; its doc node goes to the doc subtask. Order: materials, then Niagara and widgets, then the Blueprint that wires them, then map placement. Independent bp subtasks on different assets may run in parallel, 3 at most on one editor.

## 4. Confirm
Show the plan once. A "no" to every proposed option means no task: ask or wait, never relaunch work on a reinterpreted target. The owner confirms the global task; subtasks then run without further confirmation. A second global task requested meanwhile waits for its own confirmation.
When questions remain, set each new global to `wait-answers` in the `task-create.js` spec. On a pure owner confirmation, the main session uses `task-confirm.js` to set it to `todo`. Do not call `task-status.js` to repeat a transition those scripts already applied; reserve it for a later required status change not handled by them.
Defaults file (owner 2026-10-29, [[task-358-planner-cost]] Q1): the Planner writes its plan with one `node vault/scripts/task-create.js <spec.js>` call; with `wait-answers` globals that call also writes `Saved/Planner/<first-global-id>.defaults.json` (not committed: the globals, the Questions, the gdd edits their defaults imply as `{file, old, new}`). On a pure "ok" / "tout par défaut" the main session runs `node vault/scripts/task-confirm.js <file> "<owner words verbatim>"` instead of resuming the Planner: it marks the Questions decided with the date and words, appends the words to each `request`, sets `confirmed_by_owner: yes (<date>, "<words>")` and `todo`, applies the edits, lints and commits by path. Any other answer, or a missing file, resumes the Planner as before.

## 5. Execute, one subtask at a time
1. Start the global with `node vault/scripts/task-start.js <global-id>`: it sets the global and its first open subtask `in-progress` and `next_step` to it, lints and commits. Later subtasks are started by `task-next.js` when the previous one closes. Both scripts print only lines that need action (ALERT lines, lint problems, hook and git errors, refusals, the commit line, `left:` leftovers) and end with `next: <absolute path of the started subtask>` or `next: none`; open that path directly. Refusals and failures print no `next:` line.
2. Prepare the editor state (close Unreal before C++ builds; launch it before BP work).
3. Delegate each subtask as a separate fresh agent invocation with the subtask node and resolved input paths as its only reading list; the description starts with the subtask id. Never merge same-role slices into one large delegation. Follow the planned order by default, and do not call the Planner to resume or re-plan during execution. Keep each cpp and bp change summary. The doc subtask is delegated to `doc` the same way, with the brief: commit hashes and changed paths (the agent runs `git show`, the diff is not pasted), a pointer to the global `request` and `gdd_check` for decided values, the change summaries, and the nodes to create or update; it is closed by `task-next.js` like any subtask (cost-log row, `usage` line, model, ALERT lines).
4. Run the `done_when` checks.
5. Pass: fill `result`, `model`, `tokens` (input, cache write, cache read, output from `node vault/scripts/agent-usage.js`) and `runs`, add the subtask to [[cost-log]] (turns, context, resumes; read its ALERT lines and write each cause in `result`), set `done`, move `next_step`, lint, commit `task-<id>: done`.
   Closure gate (owner 2026-11-01): `task-status.js` refuses to mark a global `done` unless every listed subtask is `done`, its `model`, `tokens`, `usage` and `runs` fields are recorded, and its row exists in [[cost-log]]. Close subtasks through `task-next.js`, which records the cost before the status change.
6. Fail: send the error back to the same agent once. Second failure: set `blocked`, record why, stop and report.
7. Owner wait: when the Executor stops for an owner action or answer (close the editor, a missing decision), it sets the current subtask and its global `wait-answers` with `node vault/scripts/task-status.js <path>.md status=wait-answers "waiting_for=<the action or question>"`, commits them by path, and reports. On resume it sets both back with `status=in-progress` (or `task-start.js`; `task-next.js` does it for the next subtask), which resets `waiting_for` to `none`.

## 6. Close
Run the global acceptance checks, verify every subtask cost row and usage field is present, fill `result`, set `done`, `next_step: none`, commit, report to the owner. `task-status.js` enforces the cost gate when the global is closed.

## Relations
- [[orchestrator-runbook]] — the commands that apply this protocol.
- [[orchestrator-lessons]] — owner rules and planning lessons: read before every plan; agent limits are in [[bp-mcp-lessons]] and [[art-3d-lessons]].
- [[agents-index]] — parent index.
- [[agent-planner]] — runs steps 0 to 4.
- [[agent-executor]] — runs steps 5 and 6.
- [[cost-log]] — where step 5 records tokens per subtask.
- [[agent-doc]] — runs the doc subtask of steps 3 and 5.
- [[agent-capture]] — runs the capture subtask of step 3.
- [[_navigation]] — execution exception for opening task nodes by path.
