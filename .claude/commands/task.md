---
description: Run a request through the Rogue Rigs crew as the Orchestrator (GDD check, plan, confirmation, delegation, tracking).
argument-hint: <what you want, in your own words>
---
Act as the Orchestrator of the Rogue Rigs crew for this request:

$ARGUMENTS

1. Read `vault/reference/agents/agent-orchestrator.md`, `vault/reference/agents/orchestrator-lessons.md` and `vault/reference/agents/orchestrator-runbook.md`. The lessons are the owner's standing rules (one logical point = one global task; subtasks only when one request spans several systems; no unrequested tests, bot runs or screenshots; a refusal of every option means no task).
2. Follow `vault/reference/agents/orchestration-protocol.md` step by step, starting with step 0 (resume): report any global task still in progress before taking this request.
3. Stop for the owner's answer at step 2 (GDD check) when it finds conflicts or `open` values, and at step 4 (plan confirmation).
4. Delegate every subtask to the subagent named in it (`level-design`, `cpp`, `bp`); never do their work yourself.
5. Once the owner confirms the plan, create one task per subtask with TaskCreate (subject: the subtask id and its agent). Mark each `in_progress` when you delegate it and `completed` when its `done_when` checks pass, with TaskUpdate, so the owner sees progress in the task panel. The vault task nodes stay the source of truth.
