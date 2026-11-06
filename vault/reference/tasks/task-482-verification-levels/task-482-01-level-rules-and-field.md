---
id: task-482-01-level-rules-and-field
description: "Doc: Write the level rule in the crew nodes and add the level field to the task scripts."
parent: task-482-verification-levels
kind: task
status: todo
---
# Verification levels per global task: doc

- goal: Write the level rule in the crew nodes and add the level field to the task scripts.
- agent: doc
- depends_on: none
- inputs: agent-planner (vault/reference/agents/agent-planner.md), agent-executor (vault/reference/agents/agent-executor.md), orchestration-protocol (vault/reference/agents/orchestration-protocol.md), execution-protocol (vault/reference/agents/execution-protocol.md), orchestrator-lessons (vault/reference/agents/orchestrator-lessons.md), task-spec (vault/reference/agents/task-spec.md), agent-capture (vault/reference/agents/agent-capture.md), task-474-miner-scale-down (vault/reference/tasks/task-474-miner-scale-down.md), vault/scripts/task-create.js, vault/scripts/task-confirm.js
- expected_output: see done_when
- done_when: A1 to A10 of the global pass: a test spec with each level is accepted, one without a level and one with another value are refused, nothing left behind by those tries; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-482-verification-levels]] — parent global task.
