---
id: task-542-02-capture
description: "Capture: Before/after capture of the effect on the floor"
parent: task-542-ground-particle-edges
kind: task
status: in-progress
---
# Ground particles: no visible sprite edges: capture

- goal: Before/after capture of the effect on the floor
- agent: capture
- depends_on: task-542-01-fix-edges
- inputs: Saved/Scratch/owner-ui-batch-2026-11-13/2.webp
- expected_output: see done_when
- done_when: C1 the ground particles of image 2 visible on the floor; C2 no straight or square edge around any of them; C3 their look otherwise matches the before frame.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-13T23:29:03Z
- waiting_for: none
- result: open

## Relations
- [[task-542-ground-particle-edges]] — parent global task.
