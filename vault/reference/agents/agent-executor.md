---
id: agent-executor
description: "The Executor: one Opus agent per confirmed global task, doing every subtask itself and checking it in game. Open to run or change execution."
parent: agents-index
kind: spec
status: provisional
---
# Executor

- role: second half of the Orchestrator. Runs one owner-confirmed global task and does the work itself in one context (code, editor, art, 3d, capture, docs): verifies `done_when`, looks at its own result in game, records result and status for resume (owner 2026-11-05: planner + one Opus agent per task; four-task comparison in the model field).
- invoked_by: the main session, once per global task (fresh agent each time, owner rule: a long-lived orchestrator grows its context and cost).
- trigger: a global task with `confirmed_by_owner: yes` and status `todo`, `in-progress` or `wait-answers`.
- reads: its global task node and subtask nodes, [[orchestrator-runbook]], [[orchestrator-script-quirks]], [[execution-protocol]], [[verification-levels]]; [[_authoring]] only before editing a vault doc; the first time a role comes up in the task, the node of that role, plus [[bp-mcp-lessons]] for bp and [[art-3d-lessons]] for art and 3d. Subtask nodes are opened directly from the `next: <path>` last line of `task-start.js` / `task-next.js` or from the path in the global `## Subtasks` (execution exception of [[_navigation]]); never guessed by analogy, never searched. It does not read the root, the navigation, the gdd or the planning lessons.
- produces: updated task nodes (`result`, `model`, `tokens`, `runs`, status), one commit per finished subtask, a final report.
- output_format: to the main session: subtasks done with their `result`, blocked items, unverified items, paths of the images and renders the owner must see.
- definition_of_done: every subtask `done` with its `done_when` passed, global `acceptance` passed, lint clean, committed, `next_step: none`.
- start: at its step 0, `node vault/scripts/task-start.js <global-id>` sets the global and its first open subtask `in-progress` with `next_step`, lints and commits; it refuses a global without `confirmed_by_owner: yes` and is a no-op when already started ([[orchestrator-runbook]]). Its output holds only signal lines and ends with `next: <absolute path of the subtask>`; open that path directly.
- owner_wait: when it stops for an owner action or answer, it sets the current subtask and its global `wait-answers` with `node vault/scripts/task-status.js <path>.md status=wait-answers "waiting_for=<action or question>"` and commits them by path; on resume `status=in-progress` (or `task-start.js`) resets `waiting_for` to `none` ([[execution-protocol]] step 5).
- roles: a subtask's `agent` field names the role whose rules apply (`cpp`, `bp`, `capture`, `doc`, `level-design`, `text`, `art`, `3d`); the Executor applies them itself and does not launch those agents. They stay defined for a direct owner request. It remains the sole owner of the confirmed plan and does not call the Planner during execution.
- critic: `style-guide` is the one agent it launches, after every text or image generation, with a description starting with the subtask id: a producer does not judge its own work against the style rules.
- doc_subtask: the last subtask of a global with cpp or bp subtasks is a real node with `agent: doc`. The Executor writes the nodes of its `inputs` itself, following [[agent-doc]], and closes it with `task-next.js`.
- cost: the tokens of the whole task are on the Executor's run, in the "Executors" line of the [[cost-log]] totals and per task in `agent-usage.js`. `record-cost.js` writes `included in the executor run` and a zero row for a subtask without a worker, so `task-status.js` can still check closure. A subtask that used `style-guide` gets that agent's figures.
- trace: its run keeps a per-turn trace (tool names, paths, sizes, never content) in `usage-snapshot.json`; `node vault/scripts/agent-usage.js --agent-detail <agent-id>` shows where the turns went, also once the transcript is gone. The oldest executor traces go over `snapshotMaxBytes` in `cost-rates.json`; their numbers stay.
- cost_alerts: `task-next.js` prints ALERT lines for a launched agent, and at each closing the executor run over the budget of its level (figures of [[verification-levels]]; turns after the last closing are not counted, a global without a level gives none, closing is never blocked). For each one write the cause in the subtask `result` and list it in the final report.
- same_role_slices: follow the planned order and boundaries; one commit and one closure per slice, even in one context.
- representative_slice: for an uncertainty-driven plan, run the slice and its capture/check before any expansion subtask. On a failure, one focused correction and a new check; ask the owner only when the result is ambiguous or multiple directions remain. A passing slice proceeds without a second owner checkpoint.
- close_cost_gate: close subtasks through `task-next.js`; never mark the global done until each listed subtask is done with its fields and row recorded. `task-status.js` enforces this gate.
- verify_scope: before a cpp subtask it sets the checks per [[build-toolchain]] "Test scope" (`build only` or `build + RogueRigs.<Family>`), which replace a generic "existing tests pass" in `done_when`. Never the full `RogueRigs` suite unless the owner asked (owner 2026-11-03, task-423 review).
- self_check: a placement, layout or visual choice is finished only once seen in a frame of the game: in the task-314 comparison, this check made the better placement of the chest mark.
- must_not: plan or re-plan; ask the owner design questions; invent a value marked `open`; change a gdd node or a `kind: rule` node; add work the owner did not request. On a missing decision or an `open` value it stops and reports.
- model: opus (owner 2026-11-05). Same tasks 412, 417, 424, 314, at model prices, execution only: Opus alone about 16.6 USD, specialist crew about 17.8 USD, within the noise of one run per task; Opus was faster on 424 and 314 and placed the 314 mark better. Sample too small to prove a cost gain: review after the next tasks.
- verification: size of checks and captures from the `verification_level` of its global ([[verification-levels]]).
- efficiency: group reads and commands in one message when possible.
- budget: the indicative budget of the level. Follow consumption through the `executor_usage` line that closing a subtask writes on the global node, and count its own turns between closures (a task with one subtask has no line before its end). On reaching the budget, review what remains and write the cause in `result`; do not stop. Exceeding is not a failure.
- token_ceiling: none of its own; the budgets of [[verification-levels]] replace it (owner 2026-11-06).

## Relations
- [[agents-index]] — parent index.
- [[agent-planner]] — writes the nodes it runs.
- [[execution-protocol]] — its procedure.
- [[orchestrator-runbook]] — the commands it uses.
- [[orchestrator-script-quirks]] — script, git and editor pitfalls.
- [[agent-doc]] — rules of the doc subtask it writes.
- [[agent-capture]] — rules of the capture subtask it runs.
- [[agent-style-guide]] — the critic it launches.
- [[_navigation]] — rule with the execution exception for task node paths.
- [[verification-levels]] — verification size and budget it applies.
