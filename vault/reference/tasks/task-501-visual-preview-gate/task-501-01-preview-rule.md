---
id: task-501-01-preview-rule
description: "Doc: Write the preview-first rule for the planner and the owner-go wait for the executor."
parent: task-501-visual-preview-gate
kind: task
status: done
---
# Visual requests: preview and owner go before the full build: doc

- goal: Write the preview-first rule for the planner and the owner-go wait for the executor.
- agent: doc
- depends_on: none
- inputs: orchestration-protocol (vault/reference/agents/orchestration-protocol.md), agent-planner (vault/reference/agents/agent-planner.md), agent-executor (vault/reference/agents/agent-executor.md), execution-protocol (vault/reference/agents/execution-protocol.md), verification-levels (vault/reference/agents/verification-levels.md)
- expected_output: see done_when
- done_when: A1 to A4 of the global pass.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-501
- usage: included in the executor run of task-501
- runs: 1
- milestone: M2
- started: 2026-11-11T21:16:19Z
- waiting_for: none
- result: Preview gate written: orchestration-protocol step 3 (visuel global starts with one preview subtask, mockup if look undecided else one in-game slice with one capture, done_when ends on owner go, build subtasks depend on it); execution-protocol step 5.7 and agent-executor preview_gate (always wait-answers with the image path, build only on go, refusal leaves build todo); agent-planner visuel_preview; representative_slice restricted to uncertain non-taste integration (Q5); request-families row points to the rule.

## Relations
- [[task-501-visual-preview-gate]] — parent global task.
