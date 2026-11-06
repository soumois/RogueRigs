---
id: task-465-02-slice-capture
description: "Capture: Capture the burst rifle impact slice in a fight and send it to the owner before expansion"
parent: task-465-shot-impact-burst
kind: task
status: todo
---
# Player shot impact burst: capture

- goal: Capture the burst rifle impact slice in a fight and send it to the owner before expansion
- agent: capture
- depends_on: task-465-01-laser-impact-slice
- inputs: color-code (vault/reference/gdd/art-audio/color-code.md), Art/Source/impact-burst/owner-ref-impact-burst-2026-11-05.png
- expected_output: see done_when
- done_when: Frames sent to the owner showing: C1 a burst rifle hit on a robot with the green cartoon burst (core, spikes, glow, triangles); C2 a hit on rock; C3 a full volley on a group with robots and damage numbers readable; C4 the Shooter visible among the impacts; C5 the same fight before the change for comparison.
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
- [[task-465-shot-impact-burst]] — parent global task.
