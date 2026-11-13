---
id: task-524-02-capture
description: "Capture: alert bar around the console landing and first horde"
parent: task-524-first-horde-after-console
kind: task
status: todo
---
# Première horde un peu après l'arrivée de la console: capture

- goal: alert bar around the console landing and first horde
- agent: capture
- depends_on: task-524-01-horde-timing
- inputs: agent-capture (vault/reference/agents/agent-capture.md), level-flow (vault/reference/gdd/run/level-flow.md)
- expected_output: see done_when
- done_when: C1 the first "!" after the C letter, C whole and in front; C2 banner frame after the console landing; frames sent to the owner.
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
- [[task-524-first-horde-after-console]] — parent global task.
