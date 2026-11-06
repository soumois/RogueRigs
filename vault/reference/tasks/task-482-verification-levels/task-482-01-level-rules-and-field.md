---
id: task-482-01-level-rules-and-field
description: "Doc: Write the level rule in the crew nodes and add the level field to the task scripts."
parent: task-482-verification-levels
kind: task
status: done
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
- model: opus
- tokens: included in the executor run of task-482
- usage: included in the executor run of task-482
- runs: 1
- milestone: M2
- started: 2026-11-06T22:44:09Z
- waiting_for: none
- result: Done. New rule node verification-levels (table of levels, light shortening with the 474 example, layout never light, exploratory 3 rounds, known limits), linked from agent-planner, agent-executor, agent-capture, orchestration-protocol, execution-protocol, orchestrator-lessons, task-spec; task-create.js takes level per global (spec v2 and old form), writes verification_level, refuses none or another value; template updated. Tested on a throwaway vault copy: light, standard, exploratory accepted; none, leger and old form without level refused with nothing written; task-confirm, task-start and cancel keep the level; lint OK. No game code, Blueprint or asset, no capture. An older node without the field counts as standard. Cost: executor run.

## Relations
- [[task-482-verification-levels]] — parent global task.
