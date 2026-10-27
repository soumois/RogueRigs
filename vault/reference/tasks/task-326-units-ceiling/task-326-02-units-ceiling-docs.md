---
id: task-326-02-units-ceiling-docs
description: "Doc agent writes the units ceiling into templates, agent nodes, cost-log, runbook and executor, and converts the todo subtasks of 308, 309, 314."
parent: task-326-units-ceiling
kind: task
status: in-progress
---
# Units ceiling in the crew definitions

- goal: from the brief (commit and change summary of [[task-326-01-units-ceiling-script]], decided values Q5 to Q7 in [[task-326-units-ceiling]] `gdd_check` as answered by the owner), make every definition of `token_ceiling` say the same thing: the bound on the subtask's weighted `units` (input-token equivalents, weights in `cost-rates.json`), written `<x> M units`. Edit: templates `task.md` and `agent.md` (field gloss); `token_ceiling` of agent-cpp, agent-bp, agent-3d, agent-art, agent-text, agent-style-guide, agent-level-design, agent-doc, agent-planner to the Q6 values marked `provisional, owner 2026-10-27` (agent-executor stays `open`); agents-index intro sentence on ceilings; cost-log `Alerts` line; orchestrator-runbook "Closing a subtask" step 2; agent-executor `cost_alerts` (only the ALERT lines of record-cost.js count, never the Agent tool total); `token_ceiling` of each todo subtask of task-308, task-309 and task-314 set to its agent's Q6 default (Q7). Nothing else in those task nodes.
- agent: doc
- depends_on: task-326-01-units-ceiling-script
- inputs: task-326-units-ceiling (vault/reference/tasks/task-326-units-ceiling.md), template task (vault/templates/task.md), template agent (vault/templates/agent.md), agents-index (vault/reference/agents/agents-index.md), agent-cpp (vault/reference/agents/agent-cpp.md), agent-bp (vault/reference/agents/agent-bp.md), agent-3d (vault/reference/agents/agent-3d.md), agent-art (vault/reference/agents/agent-art.md), agent-text (vault/reference/agents/agent-text.md), agent-style-guide (vault/reference/agents/agent-style-guide.md), agent-level-design (vault/reference/agents/agent-level-design.md), agent-doc (vault/reference/agents/agent-doc.md, created by task-325), agent-planner (vault/reference/agents/agent-planner.md), agent-executor (vault/reference/agents/agent-executor.md), cost-log (vault/reference/production/cost-log.md), orchestrator-runbook (vault/reference/agents/orchestrator-runbook.md), subtask folders vault/reference/tasks/task-308-chest-lock-light/, task-309-chest-opening/, task-314-chest-lid-logo/
- expected_output: the edited nodes above; a short recap.
- done_when: no template, agent node, cost-log or runbook line still describes the ceiling as input plus output or a raw token count; the nine agent ceilings read `<x> M units` with the Q6 values; the 13 todo subtasks of 308, 309, 314 carry `M units` ceilings and no other change; lint clean; committed by explicit path.
- editor_state: any
- token_ceiling: 0.3 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-27T14:22:00Z
- waiting_for: none
- result: open

## Relations
- [[task-326-units-ceiling]] — parent global task.
