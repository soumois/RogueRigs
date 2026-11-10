---
id: task-498-01-capture-before
description: "Capture: A -game capture of several heavy slams to see whether a dust ring and rock chips already appear."
parent: task-498-heavy-slam-impact-fx
kind: task
status: in-progress
---
# Heavy slam impact: dust ring and rock chips: capture

- goal: A -game capture of several heavy slams to see whether a dust ring and rock chips already appear.
- agent: capture
- depends_on: none
- inputs: agent-capture (vault/reference/agents/agent-capture.md), enemy-heavy (vault/reference/gdd/enemies/enemy-heavy.md)
- expected_output: see done_when
- done_when: C1 Dust ring on the ground at the slam frame: seen, not seen or ambiguous, with its colour and size against the telegraph circle. C2 Rock chips at the slam frame: seen, not seen or ambiguous. Frames sent to the owner. The Executor writes in this node which of the next subtasks run.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-10T20:39:35Z
- waiting_for: none
- result: open

## Relations
- [[task-498-heavy-slam-impact-fx]] — parent global task.
