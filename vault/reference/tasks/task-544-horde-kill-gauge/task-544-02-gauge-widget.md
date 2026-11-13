---
id: task-544-02-gauge-widget
description: "Bp: Horde gauge widget under the alert bar and the moved "!""
parent: task-544-horde-kill-gauge
kind: task
status: todo
---
# Hordes: one "!" at half of C, then a kill gauge under the alert bar triggers each horde: bp

- goal: Horde gauge widget under the alert bar and the moved "!"
- agent: bp
- depends_on: task-544-01-gauge-logic
- inputs: level-flow (vault/reference/gdd/run/level-flow.md), umg-layout (vault/reference/tech/conventions/umg-layout.md), Saved/Scratch/owner-ui-batch-2026-11-13/4.webp
- expected_output: see done_when
- done_when: A1, A2 and A7 visible in play; HUD otherwise unchanged.
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
- [[task-544-horde-kill-gauge]] — parent global task.
