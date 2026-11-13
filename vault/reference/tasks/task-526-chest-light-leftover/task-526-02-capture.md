---
id: task-526-02-capture
description: "Capture: next level after an opened chest"
parent: task-526-chest-light-leftover
kind: task
status: in-progress
---
# Lumière du coffre restée du niveau précédent: capture

- goal: next level after an opened chest
- agent: capture
- depends_on: task-526-01-cleanup-fix
- inputs: agent-capture (vault/reference/agents/agent-capture.md)
- expected_output: see done_when
- done_when: C1 level N+1 at the former chest spot: no chest light; C2 an opened chest in its own level still glows; frames sent to the owner.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-13T14:19:09Z
- waiting_for: none
- result: open

## Relations
- [[task-526-chest-light-leftover]] — parent global task.
