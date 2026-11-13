---
id: task-543-02-capture
description: "Capture: Before/after capture of a wall block breaking"
parent: task-543-wall-debris-less-white
kind: task
status: todo
---
# Wall destruction particles: rock colours instead of near white: capture

- goal: Before/after capture of a wall block breaking
- agent: capture
- depends_on: task-543-01-debris-colour
- inputs: Saved/Scratch/owner-ui-batch-2026-11-13/3.png
- expected_output: see done_when
- done_when: C1 debris visible at the breaking block; C2 debris lilac and violet, none near white; C3 no bloom halo around them; C4 amount and size comparable to the before frame.
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
- [[task-543-wall-debris-less-white]] — parent global task.
