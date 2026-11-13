---
id: task-544-04-capture
description: "Capture: Captures of the "!" and the gauge through one horde cycle, four resolutions"
parent: task-544-horde-kill-gauge
kind: task
status: todo
---
# Hordes: one "!" at half of C, then a kill gauge under the alert bar triggers each horde: capture

- goal: Captures of the "!" and the gauge through one horde cycle, four resolutions
- agent: capture
- depends_on: task-544-03-gauge-widget
- inputs: umg-layout (vault/reference/tech/conventions/umg-layout.md), Saved/Scratch/owner-ui-batch-2026-11-13/4.webp
- expected_output: see done_when
- done_when: C1 the "!" at half of C at level start; C2 no gauge before the bar reaches it; C3 gauge under the bar, partly filled after kills, "!" gone; C4 gauge full and pulsing with the horde banner; C5 gauge empty and refilling after the horde; C5b gauge look close to the task-544-02 render; C6 alert bar and gauge laid out correctly at the four resolutions of umg-layout (Q10).
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
