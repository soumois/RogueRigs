---
id: task-323-wait-answers-status
description: "Global task: add the wait-answers task status with a waiting_for bullet, set by Planner and Executor when they stop for the owner."
parent: tasks-done-index
kind: task
status: done
---
# wait-answers task status

- request: addition to the status request of task-320..322, relayed by the main session: a vault status "Wait answers" that agents write themselves when they stop and need the owner to act or answer (example: the task-319 Executor stopped asking the owner to close the Unreal editor; the dashboard cannot tell this from a stalled task, it only guesses it today). Owner: "oui, va pour le 322" (owner, 2026-10-26). Proposed shape: status `wait-answers`, lint-allowed on globals and subtasks, set through task-status.js by the Executor and the Planner when they stop to ask, the question or action recorded, cleared by the next move; protocol steps 4 and 5. The dashboard side (Tools/crew-board) is the main session's job.
- split (Planner, 2026-10-26): owner rule "one logical owner point = one global task" (orchestrator-lessons, mandatory): `cancelled` (end state, index move) and `wait-answers` (live owner-wait state, new field, protocol steps 4 and 5) are separate points, so this is task-323 and not part of task-322; the content is what the owner accepted for 322. Reported to the main session; if the owner insists on one task, merge before running.
- shape check (Planner, 2026-10-26): `next_step` holds a subtask id that task-start.js (task-320) and task-next.js parse, so the question goes in a new `- waiting_for:` bullet, not in `next_step`. Leaving `wait-answers` for any status (todo after the plan is confirmed, in-progress on resume) resets it, not only `in-progress`. The index status regex of task-status.js already accepts `wait-answers`; a waiting global stays in tasks-to-do-index.
- goal: an agent waiting for the owner shows `wait-answers` with what it waits for, distinct from a stalled `in-progress`.
- acceptance: see the subtask `done_when`; committed.
- gdd_check: dev tooling, no gdd node read or changed (Planner, 2026-10-26). No `open` gdd value needed. Edits to `_authoring` and `orchestration-protocol` (kind: rule): owner intent "oui" to this addition, plus Q1 of task-320..322. Do not edit `.claude/agents/*.md` (as Q9).
- confirmed_by_owner: yes (2026-10-26): "oui, va pour le 322" to this addition; planned as task-323 by the splitting rule.
- milestone: M2
- next_step: none
- started: 2026-10-27T01:00:58Z
- executor_tokens: input 14, cache write 23084, cache read 163654, output 3459
- executor_usage: runs 1, turns 7, context start 16k peak 34k, resumes 0, 4 min, units 0.06 M
- planning_tokens: none (no planner run attributed)
- planning_usage: none
- result: Done. wait-answers status added: lint, task-status.js (waiting_for required, reset on any other status), templates, task-create.js, protocol steps 4 and 5, planner/executor/runbook, indexes, crew-board.js; verified on a throwaway vault copy; step 0 resume of waiting globals left unchanged (outside authorisation). ALERT tokens over ceiling: subagent used 72842 vs 40000, cause = 13 files edited plus a full throwaway-copy test matrix.

Run order: after task-319 closes, then task-320, task-321, task-322, task-323, one at a time (all edit task-status.js; 323 extends the field insertion of 321 and the status list of 322).

## Subtasks
1. task-323-01-wait-answers-status — cpp, editor any: _authoring, lint.js, task-status.js, templates, task-create.js, protocol steps 4 and 5, planner and executor nodes, indexes, crew-board.js.

## Relations
- [[tasks-done-index]] — parent index.
- [[task-323-01-wait-answers-status]] — step 1, cpp.
- [[task-322-cancelled-status]] — the sibling status point, run before.
