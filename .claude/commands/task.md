---
description: Run a request through the Rogue Rigs crew as the Orchestrator (GDD check, plan, confirmation, delegation, tracking).
argument-hint: <what you want, in your own words>
---
Act as the Orchestrator of the Rogue Rigs crew for this request:

$ARGUMENTS

1. Read `vault/reference/agents/agent-orchestrator.md`, then the nodes it lists as mandatory: `orchestrator-lessons.md` (the owner's standing rules: one logical point = one global task; subtasks only when one request spans several systems; no unrequested tests, bot runs or screenshots; a refusal of every option means no task), `orchestrator-runbook.md` and `orchestrator-script-quirks.md`, all in `vault/reference/agents/`.
2. Follow `vault/reference/agents/orchestration-protocol.md` step by step, starting with step 0 (resume): report any global task still in progress before taking this request.
3. Stop for the owner's answer at step 2 (GDD check) when it finds conflicts or `open` values, and at step 4 (plan confirmation).
4. Delegate every subtask to the subagent named in it: `cpp`, `bp`, `level-design`, `text`, `style-guide`, `art` or `3d`; never do their work yourself. Before delegating to `bp`, read `bp-mcp-lessons.md`; before `art` or `3d` work, read `art-3d-lessons.md`. Start each delegation's description with the subtask id (cost tracking) and hand the agent the resolved paths of its inputs.
5. Once the owner confirms the plan, show one step per subtask in the app's task list (`update_plan`, subtask id and agent) and advance it as each subtask passes its `done_when` checks. The vault task nodes stay the source of truth.
6. Send every generated image or render to the owner with `SendUserFile` as soon as it exists.
7. After every text or image generation, delegate a review to `style-guide`. Show the owner every fail with its rule: he either asks for the correction or keeps the deviation on purpose; record his choice in the subtask `result`.
