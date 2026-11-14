---
id: task-544-03-gauge-widget
description: "Bp: Horde gauge widget under the alert bar and the moved "!""
parent: task-544-horde-kill-gauge
kind: task
status: in-progress
---
# Hordes: one "!" at half of C, then a kill gauge under the alert bar triggers each horde: bp

- goal: Horde gauge widget under the alert bar and the moved "!"
- agent: bp
- depends_on: task-544-02-gauge-render
- inputs: level-flow (vault/reference/gdd/run/level-flow.md), the render and cut elements of task-544-02-gauge-render, umg-layout (vault/reference/tech/conventions/umg-layout.md), Saved/Scratch/owner-ui-batch-2026-11-13/4.webp
- expected_output: see done_when
- done_when: A1, A2 and A7 visible in play, the gauge built from the task-544-02 elements (or from the brief look if 02 recorded a gap); HUD otherwise unchanged.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-14T00:31:31Z
- waiting_for: none
- result: open

## Relations
- [[task-544-horde-kill-gauge]] — parent global task.
